#pragma once

#include <string>
#include "spdlog/spdlog.h"
#include <nlohmann/json.hpp>
#include <wrl/implements.h>

using namespace Microsoft::WRL;
using json = nlohmann::json;
using namespace std::chrono;
class SystemUtils
{
public:
    static double GetDiskSpaceUsage(const std::wstring& drive);
    static double GetCPUTemperature();
    static double GetSystemTemperature();
    static double GetMemoryUsage();
    static double GetCpuUsage();
    static json GetSystemMetrics();
    static system_clock::time_point TimePointFromJSTimestamp(std::int64_t js_timestamp);
    static std::string FormatTimeStamp(const system_clock::time_point& time_point);

};

