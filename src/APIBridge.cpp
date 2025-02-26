#include <APIBridge.h>
#include <iostream>
#include <fstream>

std::wstring ApiBridge::GetUserDomain() {
    WCHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    DWORD domain_len = DNLEN + 1;
    DWORD sid_size = SECURITY_MAX_SID_SIZE;
    const PSID sid = LocalAlloc(LMEM_FIXED, sid_size);
    SID_NAME_USE snu;

    // Get the username directly using Windows API
    if (!GetUserNameW(username, &username_len)) {
        LocalFree(sid);
        return L"";
    }

    if (WCHAR domain[DNLEN + 1]; LookupAccountNameW(nullptr, username, sid, &sid_size, domain, &domain_len, &snu)) {
        LocalFree(sid);
        return std::wstring(domain);
    }
    LocalFree(sid);
    return L"";
}

std::wstring ApiBridge::GetLoggedInUserName() {
	DWORD username_len = UNLEN + 1;
    if (WCHAR username[UNLEN + 1]; GetUserNameW(username, &username_len)) {
        return std::wstring(username);
    }
    return L"";
}

json ApiBridge::GetLoggedInUserInfo() {
    json userInfo;
    userInfo["username"] = GetLoggedInUserName();
    userInfo["domain"] = GetUserDomain();
    return userInfo;
}

bool ApiBridge::AuthenticateUser(const std::wstring& username, const std::wstring& password, const std::wstring& domain) {
    HANDLE hToken = nullptr;
    const bool isAuthenticated = LogonUser(
        username.c_str(),
        domain.c_str(),
        password.c_str(),
        LOGON32_LOGON_INTERACTIVE,
        LOGON32_PROVIDER_DEFAULT,
        &hToken
    );

    if (isAuthenticated) {
        CloseHandle(hToken);
    }

    return isAuthenticated;
}

double ApiBridge::GetDiskSpaceUsage(const std::wstring& drive) {
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    GetDiskFreeSpaceEx(drive.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes);
    return (double)(totalBytes.QuadPart - totalFreeBytes.QuadPart) / (double)totalBytes.QuadPart * 100;
}

double ApiBridge::GetCPUTemperature() const
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
        IID_IWbemLocator, (LPVOID*)&pLoc
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
        pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
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

double ApiBridge::GetSystemTemperature() {
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

double ApiBridge::GetMemoryUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    const DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    const DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    return static_cast<double>(physMemUsed) / static_cast<double>(totalPhysMem) * 100;
}

double ApiBridge::GetCpuUsage() {
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

json ApiBridge::GetSystemMetrics() {
    json metrics;
    metrics["cpuUsage"] = GetCpuUsage();
    metrics["memoryUsage"] = GetMemoryUsage();
    metrics["systemTemperature"] = GetSystemTemperature();
    metrics["diskSpaceUsage"] = GetDiskSpaceUsage(L"C:\\"); // Assuming C: drive
    return metrics;
}

std::wstring ApiBridge::BrowseForFolder() {
    BROWSEINFO bi = { nullptr };
    bi.lpszTitle = L"Browse for Folder";
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != nullptr) {
	    if (WCHAR path[MAX_PATH]; SHGetPathFromIDList(pidl, path)) {
            CoTaskMemFree(pidl);
            return std::wstring(path);
        }
        CoTaskMemFree(pidl);
    }
    return L"";
}

std::string ApiBridge::ReadFileContent(const std::wstring& filePath) {
	const std::wifstream fileStream(filePath);
    std::wstringstream buffer;
    buffer << fileStream.rdbuf();
    std::wstring fileContent = buffer.str();
    return std::string(fileContent.begin(), fileContent.end());
}

std::wstring ApiBridge::OpenFolderDialog() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        return L"";
    }
    IFileOpenDialog* pFileOpen = nullptr;
    hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (SUCCEEDED(hr)) {
        // Set the options to allow folder selection
        DWORD dwOptions;
        hr = pFileOpen->GetOptions(&dwOptions);
        if (SUCCEEDED(hr)) {
            pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
        }
        hr = pFileOpen->Show(nullptr);
        if (SUCCEEDED(hr)) {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFolderPath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
                if (SUCCEEDED(hr) && pszFolderPath != nullptr) {
                    std::wstring folderPath(pszFolderPath);
                    CoTaskMemFree(pszFolderPath);
                    pItem->Release();
                    pFileOpen->Release();
                    CoUninitialize();
                    return folderPath;
                }
                if (pszFolderPath != nullptr) {
                    CoTaskMemFree(pszFolderPath);
                }
                pItem->Release();
            }
            pItem->Release();
        }
        pFileOpen->Release();
    }
    CoUninitialize();
    return L"";
}

std::wstring ApiBridge::SaveFileDialog() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        return L"";
    }
    IFileSaveDialog* pFileSave = nullptr;
    hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
    if (SUCCEEDED(hr)) {
        hr = pFileSave->Show(nullptr);
        if (SUCCEEDED(hr)) {
            IShellItem* pItem;
            hr = pFileSave->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr) && pszFilePath != nullptr) {
                    std::wstring filePath(pszFilePath);
                    CoTaskMemFree(pszFilePath);
                    pItem->Release();
                    pFileSave->Release();
                    CoUninitialize();
                    return filePath;
                }
                if (pszFilePath != nullptr) {
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
            pItem->Release();
        }
        pFileSave->Release();
    }
    CoUninitialize();
    return L"";
}

std::wstring ApiBridge::OpenFileDialog() {
    // Initialize COM library
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        return L"";
    }
    // Create the File Open Dialog object
    IFileOpenDialog* pFileOpen = nullptr;
    hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (SUCCEEDED(hr)) {
        // Show the Open dialog box
        hr = pFileOpen->Show(nullptr);
        // Get the file name from the dialog box
        if (SUCCEEDED(hr)) {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr) && pszFilePath != nullptr) {
                    // Convert PWSTR to std::wstring
                    std::wstring filePath(pszFilePath);
                    CoTaskMemFree(pszFilePath);
                    pItem->Release();
                    pFileOpen->Release();
                    CoUninitialize();
                    return filePath;
                }
                if (pszFilePath != nullptr) {
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
            pItem->Release();
        }
        pFileOpen->Release();
    }
    CoUninitialize();
    return L"";
}