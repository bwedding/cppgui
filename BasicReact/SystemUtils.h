#pragma once

#include <string>
#include <wchar.h>
#include <chrono>
#include <nlohmann/json.hpp>
#include <wrl/implements.h>

using namespace Microsoft::WRL;
using json = nlohmann::json;
using namespace std::chrono;

struct SystemUtils
{
    static double GetDiskSpaceUsage(const std::wstring& drive);
    static double GetMemoryUsage();
    static double GetCpuUsage();
    static json GetSystemMetrics();
    static system_clock::time_point TimePointFromJSTimestamp(std::int64_t js_timestamp);
    static std::string FormatTimeStamp(const system_clock::time_point& time_point);
    static std::string wchar_to_UTF8(const wchar_t* in);
    static std::wstring UTF8_to_wchar(const char* in);
    static std::string WideToUtf8(const std::wstring& wstr);
};