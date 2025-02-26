#pragma once
#include <chrono>
#include <string>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace HeartControl {

    struct UIEvent
    {
        std::string type;       // Event type identifier
        std::string target;     // UI element identifier
        json payload; // Event data
        std::chrono::system_clock::time_point timestamp;
    };
}
