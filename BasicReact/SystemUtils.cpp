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

std::string SystemUtils::wchar_to_UTF8(const wchar_t* in)
{
    std::string out;
    unsigned int codepoint = 0;
    for (in; *in != 0; ++in)
    {
        if (*in >= 0xd800 && *in <= 0xdbff)
            codepoint = ((*in - 0xd800) << 10) + 0x10000;
        else
        {
            if (*in >= 0xdc00 && *in <= 0xdfff)
                codepoint |= *in - 0xdc00;
            else
                codepoint = *in;

            if (codepoint <= 0x7f)
                out.append(1, static_cast<char>(codepoint));
            else if (codepoint <= 0x7ff)
            {
                out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            else if (codepoint <= 0xffff)
            {
                out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            else
            {
                out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            codepoint = 0;
        }
    }
    return out;
}


std::wstring SystemUtils::UTF8_to_wchar(const char* in)
{
    std::wstring out;
    unsigned int codepoint;
    while (*in != 0)
    {
        unsigned char ch = static_cast<unsigned char>(*in);
        if (ch <= 0x7f)
            codepoint = ch;
        else if (ch <= 0xbf)
            codepoint = (codepoint << 6) | (ch & 0x3f);
        else if (ch <= 0xdf)
            codepoint = ch & 0x1f;
        else if (ch <= 0xef)
            codepoint = ch & 0x0f;
        else
            codepoint = ch & 0x07;
        ++in;
        if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff))
        {
            if (sizeof(wchar_t) > 2)
                out.append(1, static_cast<wchar_t>(codepoint));
            else if (codepoint > 0xffff)
            {
                codepoint -= 0x10000;
                out.append(1, static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
                out.append(1, static_cast<wchar_t>(0xdc00 + (codepoint & 0x03ff)));
            }
            else if (codepoint < 0xd800 || codepoint >= 0xe000)
                out.append(1, static_cast<wchar_t>(codepoint));
        }
    }
    return out;
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

std::string SystemUtils::WideToUtf8(const std::wstring& wstr)
{
    if (wstr.empty())
        return std::string();

    // Get the required buffer size
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    
    // Allocate the buffer
    std::string utf8str(size_needed, 0);
    
    // Perform the conversion
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &utf8str[0], size_needed, nullptr, nullptr);
    
    return utf8str;
}

double SystemUtils::GetDiskSpaceUsage(const std::wstring& drive)
{
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    GetDiskFreeSpaceEx(drive.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes);
    return static_cast<double>(totalBytes.QuadPart - totalFreeBytes.QuadPart) / static_cast<double>(totalBytes.QuadPart) * 100;
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
    metrics["diskSpaceUsage"] = GetDiskSpaceUsage(L"C:\\"); // Assuming C: drive
    return metrics;
}