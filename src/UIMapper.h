#pragma once
#include <string>
#include "EventManager.h"
#include "UIEvent.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace EventParsers {
    // Simple function to extract numeric values from json, isolated from templates
    inline double extractSliderValue(const std::string& payload, const std::string& sliderId) {
        try {
            if (payload.empty()) {
                spdlog::warn("Empty payload for slider: {}", sliderId);
                return 0.0;
            }

            nlohmann::json parsed = nlohmann::json::parse(payload);

            if (parsed.contains("value")) {
                if (parsed["value"].is_number()) {
                    return parsed["value"].get<double>();
                }
                if (parsed["value"].is_string()) {
	                try {
		                return std::stod(parsed["value"].get<std::string>());
	                }
	                catch (...) {
		                spdlog::error("Cannot convert string value to number for slider: {}", sliderId);
	                }
                }
            }

            spdlog::warn("Missing or invalid 'value' field for slider: {}", sliderId);
            return 0.0;
        }
        catch (...) {
            spdlog::error("Failed to parse JSON for slider: {}", sliderId);
            return 0.0;
        }
    }
}

class UIMapper {
public:
    explicit UIMapper(HeartControl::EventManager& eventManager) : m_eventManager(eventManager) {}

    // Map button ID to a simple void function
    template<typename Func>
    int mapButton(const std::string& buttonId, Func&& func) {
        return m_eventManager.subscribe("button-" + buttonId, [func](const HeartControl::UIEvent&) {
            func();
            });
    }

    // Map slider to a function taking a value
    template<typename Func>
    int mapSlider(const std::string& sliderId, Func&& func) {
        std::string id = sliderId; // Create a copy for capture

        return m_eventManager.subscribe("slider-" + id,
            [func, id](const HeartControl::UIEvent& evt) {
                // Use the standalone parser to avoid template issues
                double value = EventParsers::extractSliderValue(evt.payload, id);
                func(value);
            }
        );
    }

private:
    HeartControl::EventManager& m_eventManager;
};
