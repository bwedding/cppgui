#include <windows.h>
#include <lmcons.h>
#include "SystemUtils.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <pdh.h>
#include <wbemcli.h>
#include <shlobj.h>
#include <comdef.h>
#include <pdh.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "advapi32.lib")


system_clock::time_point SystemUtils::TimePointFromJSTimestamp(const std::int64_t js_timestamp)
{
    // Convert milliseconds to seconds (as std::chrono::system_clock works with seconds)
    const auto duration = milliseconds(js_timestamp);
    const auto time_point = system_clock::time_point(duration);

    return time_point;
}

std::string SystemUtils::FormatTimeStamp(const system_clock::time_point& time_point)
{
	const std::time_t time = system_clock::to_time_t(time_point);
    std::tm tm;

    // Use localtime_s for thread safety and to avoid warnings
    localtime_s(&tm, &time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%m-%d-%Y %H:%M:%S");

    return oss.str();
}

double SystemUtils::GetDiskSpaceUsage(const std::wstring& drive)
{
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    GetDiskFreeSpaceEx(drive.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes);
    return static_cast<double>(totalBytes.QuadPart - totalFreeBytes.QuadPart) / static_cast<double>(totalBytes.QuadPart) * 100;
}

double SystemUtils::GetCPUTemperature() 
{
	IWbemLocator* pLoc = nullptr;
    IWbemServices* pSvc = nullptr;
    IEnumWbemClassObject* pEnumerator = nullptr;
    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    // Step 1: Initialize COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM library. Error code = 0x" << std::hex << hr << std::endl;
        return -1;
    }

    // Step 2: Initialize Security
    hr = CoInitializeSecurity(
	    nullptr,
        -1,
        nullptr,
        nullptr,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE,
        nullptr
    );

    if (FAILED(hr)) {
        std::cerr << "Failed to initialize security. Error code = 0x" << std::hex << hr << std::endl;
        CoUninitialize();
        return -1;
    }

    // Step 3: Obtain the initial locator to WMI
    hr = CoCreateInstance(
        CLSID_WbemLocator,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, reinterpret_cast<LPVOID*>(&pLoc)
    );

    if (FAILED(hr)) {
        std::cerr << "Failed to create IWbemLocator object. Error code = 0x" << std::hex << hr << std::endl;
        CoUninitialize();
        return -1;
    }

    // Step 4: Connect to WMI through the IWbemLocator::ConnectServer method
    hr = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        nullptr,
        nullptr,
        nullptr,
        NULL,
        nullptr,
        nullptr,
        &pSvc
    );

    if (FAILED(hr)) {
        std::cerr << "Could not connect. Error code = 0x" << std::hex << hr << std::endl;
        pLoc->Release();
        CoUninitialize();
        return -1;
    }

    // Step 5: Set security levels on the proxy
    hr = CoSetProxyBlanket(
        pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        nullptr,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE
    );

    if (FAILED(hr)) {
        std::cerr << "Could not set proxy blanket. Error code = 0x" << std::hex << hr << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return -1;
    }

    // Step 6: Use the IWbemServices pointer to make requests to WMI
    hr = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_TemperatureProbe"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator
    );

    if (FAILED(hr)) {
        std::cerr << "Query for temperature probes failed. Error code = 0x" << std::hex << hr << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return -1;
    }

    // Step 7: Get the data from the query
    double temperature = -1;
    while (pEnumerator) {
        hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn == 0) {
            break;
        }
        VARIANT vtProp;
        VariantInit(&vtProp);  // Initialize the VARIANT
        hr = pclsObj->Get(L"CurrentReading", 0, &vtProp, nullptr, nullptr);
        if (SUCCEEDED(hr)) {
            temperature = vtProp.intVal;
            VariantClear(&vtProp);
        }
        else {
            VariantClear(&vtProp);  // Clean up even if Get() fails
        }
        pclsObj->Release();
    }

    // Cleanup
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();

    return temperature;
}

double SystemUtils::GetSystemTemperature() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        return -1.0;
    }

    hr = CoInitializeSecurity(
        nullptr,
        -1,
        nullptr,
        nullptr,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE,
        nullptr
    );

    if (FAILED(hr)) {
        CoUninitialize();
        return -1.0;
    }

    IWbemLocator* pLocator = nullptr;
    hr = CoCreateInstance(
        CLSID_WbemLocator,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        reinterpret_cast<void**>(&pLocator)
    );

    if (FAILED(hr)) {
        CoUninitialize();
        return -1.0;
    }

    IWbemServices* pNamespace = nullptr;
    hr = pLocator->ConnectServer(
        const_cast<BSTR>(L"ROOT\\WMI"),
        nullptr,
        nullptr,
        nullptr,
        0,
        nullptr,
        nullptr,
        &pNamespace
    );

    if (FAILED(hr)) {
        pLocator->Release();
        CoUninitialize();
        return -1.0;
    }

    hr = CoSetProxyBlanket(
        pNamespace,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        nullptr,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE
    );

    if (FAILED(hr)) {
        pNamespace->Release();
        pLocator->Release();
        CoUninitialize();
        return -1.0;
    }

    IEnumWbemClassObject* pEnumerator = nullptr;
    hr = pNamespace->ExecQuery(
        const_cast<BSTR>(L"WQL"),
        //BSTR(L"SELECT * FROM MSAcpi_ThermalZoneTemperature"),
        const_cast<BSTR>(L"SELECT * FROM FROM Win32_TemperatureProbe"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator
    );

    if (FAILED(hr)) {
        pNamespace->Release();
        pLocator->Release();
        CoUninitialize();
        return -1.0;
    }

    double temperature = 0.0;
    IWbemClassObject* pClassObject = nullptr;
    ULONG uReturn = 0;
    while (pEnumerator->Next(WBEM_INFINITE, 1, &pClassObject, &uReturn) == S_OK) {
        VARIANT vtProp;
        VariantInit(&vtProp);  // Initialize the VARIANT
        hr = pClassObject->Get(L"CurrentTemperature", 0, &vtProp, nullptr, nullptr);
        if (SUCCEEDED(hr)) {
            temperature = (static_cast<double>(vtProp.intVal) - 2732) / 10.0;
            VariantClear(&vtProp);
        }
        else {
            VariantClear(&vtProp);  // Clean up even if Get() fails
        }
        pClassObject->Release();
    }

    pEnumerator->Release();
    pNamespace->Release();
    pLocator->Release();
    CoUninitialize();

    return temperature;
}

double SystemUtils::GetMemoryUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    const DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    const DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    return static_cast<double>(physMemUsed) / static_cast<double>(totalPhysMem) * 100;
}

double SystemUtils::GetCpuUsage() {
    PDH_HQUERY hQuery;
    PDH_HCOUNTER hCounter;
    PDH_FMT_COUNTERVALUE counterVal;

    // Open a query
    PdhOpenQuery(nullptr, NULL, &hQuery);
    // Add a counter to query CPU usage
    PdhAddCounter(hQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &hCounter);
    // Collect query data initially
    PdhCollectQueryData(hQuery);
    // Sleep for 1 second to get the next reading
    Sleep(1000);
    // Collect query data again
    PdhCollectQueryData(hQuery);
    // Get the formatted counter value
    PdhGetFormattedCounterValue(hCounter, PDH_FMT_DOUBLE, nullptr, &counterVal);
    // Close the query
    PdhCloseQuery(hQuery);

    return counterVal.doubleValue;
}

json SystemUtils::GetSystemMetrics() {
    json metrics;
    metrics["cpuUsage"] = GetCpuUsage();
    metrics["memoryUsage"] = GetMemoryUsage();
    metrics["systemTemperature"] = GetSystemTemperature();
    metrics["diskSpaceUsage"] = GetDiskSpaceUsage(L"C:\\"); // Assuming C: drive
    return metrics;
}