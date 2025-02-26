#pragma once
#include "WebView2.h"
#include "WebView.h"
#include <functional>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <tuple>
#include "nlohmann/json.hpp"

using namespace Microsoft::WRL;
using json = nlohmann::json;

// Function type for event callbacks
using EventCallback = std::function<void(const json&)>;
// Token structure to track event subscriptions
struct EventToken
{
    size_t id;
    std::wstring eventName;
    std::string componentFilter;  // Optional component to filter on


    // Comparison operators for container operations
    bool operator!=(const EventToken& other) const
    {
        return !(*this == other);
    }

    bool operator==(const EventToken& other) const
    {
        return id == other.id &&
            eventName == other.eventName &&
            componentFilter == other.componentFilter;
    }
};

class CommunicationManager
{
public:
    static CommunicationManager& Instance();
    static bool SendScriptToFrontend(const std::wstring& script);
    static bool SendScriptToFrontend(const std::string& script);
    static bool SendMessageToFrontend(const std::wstring& message);
    bool Initialize(HWND hwnd);

    // Use default schema
    EventToken SubscribeToEvent(
        const std::wstring& eventName,
        const std::string& componentName,
        EventCallback callback);

    EventToken SubscribeToEvent(
        const std::wstring& eventName,
        EventCallback callback);

    EventToken SubscribeToEventSchema(
        const std::wstring& eventName,
        const std::string& componentName,  // Added parameter
        const std::string_view& schema,
        EventCallback callback);

    // Subscribe to frontend events with schema validation
    EventToken SubscribeToEventSchema(
        const std::wstring& eventName,
        const std::string_view& schema,
        EventCallback callback);

    // Subscribe to all events for a specific component
    EventToken SubscribeToComponentEvents(
        const std::string& componentName,
        EventCallback callback);

    // Unsubscribe from all events for a specific component
    void UnsubscribeFromComponentEvents(const std::string& componentName);

    void UnsubscribeFromEvent(const EventToken& token);

    void HandleWebMessage(ICoreWebView2WebMessageReceivedEventArgs* args);


private:
    HWND m_hwnd = nullptr;

    // Structure to hold event information
    struct EventInfo {
        json schema;
        std::vector<std::tuple<size_t, std::string, EventCallback>> callbacks;
    };

    static bool ValidateAgainstSchema(const json& data, const json& schema);

    // String conversion helpers
    static std::wstring s2ws(const std::string& str);

    // Map of event names to their callbacks and schema
    std::unordered_map<std::wstring, EventInfo> m_eventCallbacks{};

    // Map of component names to their callbacks for component-wide subscriptions
    std::unordered_map<std::string, std::vector<std::tuple<size_t, std::string, EventCallback>>> m_componentCallbacks{};

};
