#include "WebView2.h"
#include "WebView.h"
#include <wrl.h>
#include <memory>
#include "spdlog.h"
#include <functional>
#include "nlohmann/json.hpp"
#include "CommunicationManager.h"

using namespace Microsoft::WRL;
using json = nlohmann::json;


// Schema for a simple button click event with a message
constexpr std::string_view EventSchema = R"({
"type": "object",
"properties": {
	"metadata": {
		"type": "object",
		"properties": {
			"action": { "type": "string" },
			"component": { "type": "string" },
			"control": { "type": "string" },
			"currentState": { "type": "number" },
			"mode": { "type": "string" },
			"parameter": { "type": "string" }
		},
		"required": ["action", "component", "control"]
	},
	"type": { "type": "string" },
	"value": { "type": "boolean" }
},
"required": ["metadata", "type", "value"]
})";

// Function type for event callbacks
using EventCallback = std::function<void(const json&)>;

CommunicationManager& CommunicationManager::Instance()
{
    SPDLOG_TRACE("Entering");

    static CommunicationManager instance;
    return instance;
}

bool CommunicationManager::SendScriptToFrontend(const std::wstring& script)
{
    SPDLOG_TRACE("Entering");

    if (HRESULT hr = WebView2Manager::GetInstance().ExecuteScript(script); FAILED(hr))
    {
        spdlog::critical("Failed to execute JavaScript: {:#x}", hr);
        return false;
    }
    return true;
}

bool CommunicationManager::SendScriptToFrontend(const std::string& script)
{
    SPDLOG_TRACE("Entering");

    auto wscript = s2ws(script);

    if (HRESULT hr = WebView2Manager::GetInstance().ExecuteScript(wscript); FAILED(hr))
    {
        spdlog::critical("Failed to execute JavaScript: {:#x}", hr);
        return false;
    }
    return true;
}

bool CommunicationManager::SendMessageToFrontend(const std::wstring& message)
{
    //SPDLOG_TRACE("Entering");

    if (HRESULT hr = WebView2Manager::GetInstance().PostMessageToWebView(message); FAILED(hr))
    {
        spdlog::critical("Failed to send message to frontend: {:#x}", hr);
        return false;
    }
    return true;
}

bool CommunicationManager::Initialize(HWND hwnd)
{
    SPDLOG_TRACE("Entering");

    m_hwnd = hwnd;
    return true;
}

// Use default schema
EventToken CommunicationManager::SubscribeToEvent(
    const std::wstring& eventName,
    const std::string& componentName,
    const EventCallback callback)
{
    SPDLOG_TRACE("Entering");

    return SubscribeToEventSchema(eventName, componentName, EventSchema, callback);
}

EventToken CommunicationManager::SubscribeToEvent(
    const std::wstring& eventName,
    const EventCallback callback)
{
    SPDLOG_TRACE("Entering");

    return SubscribeToEventSchema(eventName, EventSchema, callback);
}

EventToken CommunicationManager::SubscribeToEventSchema(
    const std::wstring& eventName,
    const std::string& componentName,  // Added parameter
    const std::string_view& schema,
    EventCallback callback)
{
    SPDLOG_TRACE("Entering");

    static size_t nextId = 0;
    EventToken token{ nextId++, eventName, componentName };

    auto& eventInfo = m_eventCallbacks[eventName];
    eventInfo.schema = json::parse(schema);
    eventInfo.callbacks.push_back({ token.id, componentName, callback });

    spdlog::info("Subscribed to event:{0} for component: {1}", std::string(eventName.begin(), eventName.end()), componentName);
    return token;
}

// Subscribe to frontend events with schema validation
EventToken CommunicationManager::SubscribeToEventSchema(
    const std::wstring& eventName,
    const std::string_view& schema,
    EventCallback callback)
{
    SPDLOG_TRACE("Entering");

    static size_t nextId = 0;
    EventToken token{ nextId++, eventName };

    auto& eventInfo = m_eventCallbacks[eventName];
    eventInfo.schema = json::parse(schema);
    eventInfo.callbacks.push_back({ token.id, "", callback });

    spdlog::info("Subscribed to event:{0} with schema {1}", std::string(eventName.begin(), eventName.end()), schema);
    return token;
}

// Subscribe to all events for a specific component
EventToken CommunicationManager::SubscribeToComponentEvents(
    const std::string& componentName,
    EventCallback callback)
{
    SPDLOG_TRACE("Entering");

    static size_t nextId = 0;
    EventToken token{ nextId++, L"", componentName };  // Empty event name indicates component-wide subscription

    // Store in a special map for component-wide subscriptions
    m_componentCallbacks[componentName].push_back({ token.id, componentName, callback });

    spdlog::info("Subscribed to all events for component: :{0}", componentName);
    return token;
}

// Unsubscribe from all events for a specific component
void CommunicationManager::UnsubscribeFromComponentEvents(const std::string& componentName)
{
    SPDLOG_TRACE("Entering");

    // Remove from component-wide subscriptions
    m_componentCallbacks.erase(componentName);

    // Also remove any specific event subscriptions for this component
    for (auto& eventInfo : m_eventCallbacks | std::views::values) {
        auto& callbacks = eventInfo.callbacks;
        std::erase_if(callbacks,
                      [&](const auto& tuple) { return std::get<1>(tuple) == componentName; });
    }

    spdlog::info("Unsubscribed to all events for component: :{0}", componentName);

}

void CommunicationManager::UnsubscribeFromEvent(const EventToken& token)
{
    SPDLOG_TRACE("Entering");

    auto& callbacks = m_eventCallbacks[token.eventName].callbacks;
    std::erase_if(callbacks,
                  [&](const auto& tuple) { return std::get<0>(tuple) == token.id; });
    spdlog::info("Unsubscribed from event: {0}", std::string(token.eventName.begin(), token.eventName.end()));
}

void CommunicationManager::HandleWebMessage(ICoreWebView2WebMessageReceivedEventArgs* args)
{
    SPDLOG_TRACE("Entering");
    LPWSTR message = nullptr;
    HRESULT hr = args->TryGetWebMessageAsString(&message);

    spdlog::info("TryGetWebMessageAsString result:  {0:x}", hr);


    if (FAILED(hr) || !message)
    {
        spdlog::info("Failed to get message string. HRESULT: {0:x}", hr);
        return;
    }

    // Use a smart pointer to handle the COM memory
    struct CoTaskMemDeleter
    {
        void operator()(void* p) const
        {
            if (p) CoTaskMemFree(p);
        }
    };
    std::unique_ptr<wchar_t, CoTaskMemDeleter> messagePtr(message);

    // Now we can safely use the message without worrying about manually freeing it
    std::wstring wmessage(message);

    spdlog::info("Message received, length: {0}", wmessage.length());
    spdlog::info("Message content: {0}", std::string(wmessage.begin(), wmessage.end()));

    try {
        auto j = json::parse(std::string(wmessage.begin(), wmessage.end()));

        if (!j["type"].is_string()) {
            spdlog::error("Type is not a string value");
            return;
        }
        auto typeStr = j["type"].get<std::string>();
        const auto eventType = std::wstring(typeStr.begin(), typeStr.end());

        // Get component name from metadata if present
        std::string componentName;
        if (j.contains("metadata") && j["metadata"].contains("component")) {
            componentName = j["metadata"]["component"].get<std::string>();
        }

        // First, handle component-wide subscriptions
        if (!componentName.empty())
        {
	        const auto componentIt = m_componentCallbacks.find(componentName);
            if (componentIt != m_componentCallbacks.end())
            {
                for (const auto& [id, comp, callback] : componentIt->second)
                {
                    callback(j);
                }
            }
        }

        // Then handle specific event subscriptions
        const auto it = m_eventCallbacks.find(eventType);
        if (it != m_eventCallbacks.end())
        {
            const auto& eventInfo = it->second;

            // Validate against schema if present
            if (!eventInfo.schema.is_null())
            {
                // TODO: Add schema validation here
            }

            // Call all registered callbacks for this event
            for (const auto& [id, comp, callback] : eventInfo.callbacks)
            {
                // Skip if component filter doesn't match
                if (!comp.empty() && comp != componentName)
                {
                    continue;
                }
                callback(j);
            }
        }
    }
    catch (const std::exception& e)
    {
        spdlog::error("Error processing message: {0}", e.what());
    }
}

// Structure to hold event information
struct EventInfo {
    json schema;
};


bool CommunicationManager::ValidateAgainstSchema(const json& data, const json& schema)
{
    SPDLOG_TRACE("Entering");

    // Implement JSON schema validation here
    return true; // Placeholder
}

// String conversion helpers
std::wstring CommunicationManager::s2ws(const std::string& str)
{
    SPDLOG_TRACE("Entering");

    if (str.empty()) {
        return {};
    }

    // Calculate required buffer size
    const int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
                                                static_cast<int>(str.size()), nullptr, 0);

    if (size_needed <= 0)
    {
        spdlog::error("MultiByteToWideChar failed to calculate size");
        return {};
    }

    // Create the buffer
    std::wstring wstr(size_needed, 0);

    // Do the actual conversion
    const int result = MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
                                           static_cast<int>(str.size()), wstr.data(), size_needed);

    if (result <= 0) {
        spdlog::error("MultiByteToWideChar failed to convert string");
        return {};
    }

    return wstr;
}

