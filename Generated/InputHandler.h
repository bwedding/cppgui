// AUTO-GENERATED - DO NOT MODIFY
#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>

// Custom exception for unimplemented handlers
class NotImplementedException : public std::runtime_error {
public:
    NotImplementedException(const std::string& handlerName) 
        : std::runtime_error("Handler not implemented: " + handlerName) {}
};

class InputHandler {
public:
    using HandlerFunc = std::function<void()>;

    void HandleEvent(const std::string& elementId) {
        static const std::unordered_map<std::string, HandlerFunc> handlers = {
        {"change-heartrate-${action}", [this] { HandleChangeHeartrate(); }},
        {"auto-manual-control", [this] { HandleAutoManualControl(); }}
        };

        if (auto it = handlers.find(elementId); it != handlers.end()) {
            it->second();
        } else {
            LogError("Unknown element:", elementId);
        }
    }

    virtual void HandleChangeHeartrate() {
        throw NotImplementedException("HandleChangeHeartrate");
    }
    virtual void HandleAutoManualControl() {
        throw NotImplementedException("HandleAutoManualControl");
    }

private:
    void LogError(const std::string& message, const std::string& elementId) {
        // Add your logging implementation here
    }
};
