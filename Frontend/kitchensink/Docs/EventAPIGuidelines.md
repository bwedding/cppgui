# Event System Architecture & Usage Guidelines

## System Architecture Overview

The event system provides a robust, decoupled mechanism for communication between components in your application. It follows a publisher-subscriber pattern with asynchronous event processing.

### Key Components

1. **UIEvent**: A data structure representing events with:
   - Type (string identifier)
   - Target (UI element identifier)
   - Payload (JSON data associated with the event)
   - Timestamp

2. **EventQueue**: Manages event processing in a background thread:
   - Thread-safe enqueuing of events
   - Asynchronous event processing
   - Control over the processing lifecycle

3. **EventDispatcher**: Routes events to registered handlers:
   - Subscription management
   - Event distribution to appropriate handlers
   - Thread-safe operation

4. **CallbackRegistry**: Facilitates cross-thread communication:
   - Registration of callbacks, events, and subscription parameters
   - Thread-safe storage and retrieval mechanisms
   - Management of subscription results

5. **EventManager**: Provides a simplified interface for the team:
   - Hides complex implementation details
   - Offers intuitive subscription methods
   - Centralized access point for event operations
   - Integration with the Windows messaging system

6. **UIMapper**: Specializes in UI-specific event mapping:
   - Maps UI controls to business logic
   - Handles common UI patterns (buttons, sliders, etc.)
   - Provides parsing utilities for control values

### Flow Diagram

```
WebView2/UI Events → UIEvent objects → EventQueue (background thread)
                                           ↓
Subscribers ← EventDispatcher ← Event processing
```

### Complete Event Flow

1. **Frontend Initiation**: 
   - React component triggers an event via the EventStore
   - JSON payload is created and sent via WebView2 bridge with SendClick()

2. **Native Bridge**:
   - NativeWindowControls::SendClick receives the JSON data
   - Creates a UIEvent with the data
   - Registers the event with EventManager
   - Posts a WM_USER_EVENT Windows message with the event ID

3. **Event Processing**:
   - WindowApp's message handler processes the WM_USER_EVENT
   - Retrieves the UIEvent from EventManager
   - Enqueues it into the EventQueue

4. **Background Processing**:
   - EventQueue processes events in a background thread
   - EventDispatcher routes events to registered handlers
   - Subscribers receive and handle the events

## Usage Guidelines

### Basic Event Subscription

To subscribe to events:

```cpp
// Get the event manager
auto& eventManager = WindowApp::GetInstance()->GetEventManager();

// Subscribe with a lambda
int subscriptionId = eventManager.subscribe("event-type", [](const HeartControl::UIEvent& evt) {
    // Handle the event
    spdlog::info("Event received: {}", evt.type);
    
    // Parse payload if needed
    try {
        // Now payload is already a json object so no need to parse
        auto& data = evt.payload;
        // Process the data
    } catch (const std::exception& e) {
        spdlog::error("Error processing event data: {}", e.what());
    }
});

// Save the subscription ID if you need to unsubscribe later
```

### Triggering Events

To trigger an event:

```cpp
// Create an event
HeartControl::UIEvent evt{
    "event-type",          // Event type
    "component-name",      // Target component
    json::parse("{\"key\":\"value\"}"), // JSON payload (already parsed)
    std::chrono::system_clock::now()
};

// Register event and post a message to process it
auto& eventManager = WindowApp::GetInstance()->GetEventManager();
const int eventId = eventManager.registerEvent(std::move(evt));
PostMessage(hwnd, WM_USER_EVENT, 0, eventId);

// Or use the triggerEvent helper method
eventManager.triggerEvent("event-type", "component-name", "{\"key\":\"value\"}");
```

### Using the UI Mapper

For UI controls, use the UIMapper for cleaner code:

```cpp
// Get the UI mapper
UIMapper uiMapper(WindowApp::GetInstance()->GetEventManager());

// Map a button to a simple function
uiMapper.mapButton("save", []() {
    // Handle save button click
    saveDocument();
});

// Map a slider to a function that takes a value
uiMapper.mapSlider("volume", [](double volume) {
    // Handle volume change
    setVolume(volume);
});
```

### Unsubscribing

To unsubscribe from events:

```cpp
// Unsubscribe using the subscription ID from earlier
eventManager.unsubscribe("event-type", subscriptionId);
```

### Processing JSON Events

When handling events with JSON payloads:

```cpp
eventManager.subscribe("data-update", [](const HeartControl::UIEvent& evt) {
    try {
        // Access the JSON payload directly
        auto& data = evt.payload;
        
        // Extract values safely
        if (data.contains("value") && data["value"].is_number()) {
            double value = data["value"].get<double>();
            processValue(value);
        }
        
        if (data.contains("metadata") && data["metadata"].is_object()) {
            std::string source = data["metadata"].value("source", "unknown");
            processMetadata(source);
        }
    } catch (const std::exception& e) {
        spdlog::error("Error processing data-update event: {}", e.what());
    }
});
```

## WebView2 Integration

Events from WebView2 are automatically integrated with our system:

```javascript
// JavaScript in WebView2 using useEventStore
import { useEventStore } from '../lib/EventStore';

function handleButtonClick() {
  const eventStore = useEventStore();
  eventStore.sendEvent({
    type: "button-clicked",
    target: "save-button",
    payload: {
      id: "save-button",
      timestamp: Date.now()
    }
  });
}
```

The WebView2 bridge sends these events via the SendClick method:

```typescript
// EventStore.ts
export const useEventStore = create<EventStore>((set, get) => ({
  sendEvent: async (event: ApplicationEvent) => {
    if (window.chrome?.webview) {
      // Stringify the event object before sending
      const messageString = JSON.stringify(event);
      console.log('Sending event from useEventStore:', event);
      try {
        // Use sync call for button clicks for best performance
        const result = window.chrome.webview.hostObjects.sync.native.SendClick(messageString);
        // Async has an overhead of 250-300ms, sync is 2ms or less
      } catch (error) {
        console.error("Error sending click:", error);
      }
    } else {
      console.warn('WebView2 not available');
    }
  },
}));
```

On the native side, the SendClick method processes these messages:

```cpp
STDMETHODIMP NativeWindowControls::SendClick(const BSTR jsonData)
{
    if (!jsonData) return E_INVALIDARG;

    std::wstring wstr(jsonData);
    std::string str(wstr.begin(), wstr.end());
    // Parse the JSON data
    const auto json = nlohmann::json::parse(str);
    
    // Create a UIEvent
    HeartControl::UIEvent evt{
        "auto-manual-control",  // Event type
        "User interface",       // Target
        str,                    // JSON payload
        system_clock::time_point{}
    };
    
    // Register the event with EventManager
    auto& mEvtMgr = WindowApp::GetInstance()->GetEventManager();
    const int eventId = mEvtMgr.registerEvent(std::move(evt));

    // Post a message to process the event
    PostMessage(hwnd, WM_USER_EVENT, 0, eventId);

    return S_OK;
}
```

The WindowApp then processes messages and dispatches events:

```cpp
case WM_USER_EVENT:
{
    int eventId = static_cast<int>(lParam);
    auto event = m_eventManager.retrieveEvent(eventId);
    m_eventQueue.enqueue(std::move(event));
    return 0;
}
```

## Performance Considerations

- **Synchronous vs. Asynchronous**: The WebView2 bridge supports both sync and async calls:
  - Sync calls provide much better performance for UI events (~2ms vs 250-300ms)
  - Always use sync for button clicks and other interactive elements
  - Use async only for non-blocking operations where performance is less critical

- **Message Processing**: The application processes queued events in batches to avoid blocking the UI:
  ```cpp
  case WM_USER_PROCESS_DATA_QUEUE:
  {
      if (m_dataStreamer) 
      {
          // Process up to 50 items at a time to avoid blocking UI
          m_dataStreamer->ProcessQueue(50);
      }
      return 0;
  }
  ```

## Best Practices

1. **Event Naming**: Use consistent naming conventions for event types:
   - Hyphenated, lowercase strings (e.g., "button-clicked", "data-updated")
   - Group related events with prefixes (e.g., "user-login", "user-logout")

2. **Payload Structure**: Use a consistent JSON structure for payloads:
   - Include all necessary data for processing
   - Use nested objects for complex data
   - Include metadata when useful

3. **Error Handling**: Always use try-catch blocks when working with JSON payloads

4. **Resource Management**: Save subscription IDs and unsubscribe when components are destroyed

5. **Threading**: Remember that event handlers run in the background thread
   - Don't directly manipulate UI elements from event handlers
   - Use PostMessage to communicate back to the UI thread if needed

6. **Testing**: Test event flow with logging to ensure events are being properly processed and distributed

7. **Performance**: Be mindful of sync vs async bridge calls based on your performance needs

This architecture provides a robust foundation for decoupled communication between components in your application, making it easier to maintain and extend the codebase.

## Automated Callback Generation

The application features an automated build system that generates callback stubs for frontend elements marked with the `data-backend` attribute. This system significantly reduces the manual work required to handle UI events from the frontend.

### How It Works

1. **Frontend Markup**: Add the `data-backend` attribute to any frontend component that needs to communicate with the backend:

```jsx
<Button
  name={`change-heartrate-${action}`}
  variant="ghost"
  size="icon"
  disabled={disabled}
  onClick={handleClick}
  data-backend='{"control":"change-heartrate","parameter":"bpm","currentState":"decrease or increase depending on action"}'
  className="bg-neutral-800 text-gray-400 hover:text-white hover:bg-neutral-600"
  noEvent={true}
>
  {action === 'decrease' ? <Minus className="h-6 w-6" /> : <Plus className="h-6 w-6" />}
</Button>
```

2. **Build Process**: When frontend files change, MSVC build scripts automatically scan these files and generate an `InputHandler.h` file with callback stubs for all elements with the `data-backend` attribute.

3. **Generated Code**: The system creates a virtual method for each control identified by the `control` property in the `data-backend` JSON:

```cpp
// AUTO-GENERATED - DO NOT MODIFY
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
};
```

### Using Generated Handlers

To implement the generated handlers:

1. **Create a Handler Class**: Derive from the generated `InputHandler` class:

```cpp
class MyInputHandler : public InputHandler {
public:
    void HandleChangeHeartrate() override {
        // Handle heart rate change
        spdlog::info("Heart rate change requested");
        // Access other parameters from the event if needed
    }
    
    void HandleAutoManualControl() override {
        // Handle auto/manual control toggle
        spdlog::info("Auto/manual control toggle requested");
    }
};
```

2. **Register Your Handler**: Ensure your handler is connected to the event system:

```cpp
auto& eventManager = WindowApp::GetInstance()->GetEventManager();

// Create your handler
auto inputHandler = std::make_shared<MyInputHandler>();

// Subscribe to frontend events
eventManager.subscribe("auto-manual-control", [inputHandler](const HeartControl::UIEvent& evt) {
    inputHandler->HandleEvent("auto-manual-control");
});
```

### Benefits

- **Reduced Boilerplate**: No need to manually create handler methods for each UI control
- **Better Maintainability**: Clear separation between UI definition and event handling
- **Automatic Updates**: When frontend controls change, handlers are automatically updated
- **Compile-Time Safety**: Missing handlers throw `NotImplementedException` rather than failing silently

### Best Practices

1. **Use Descriptive Control Names**: The `control` property should clearly indicate the action being performed
2. **Include Relevant Parameters**: Add any parameters needed by the handler in the `data-backend` JSON
3. **Implement All Handlers**: Override all generated virtual methods to avoid runtime exceptions
4. **Document Control Purpose**: Add comments to explain what each control does when marking with `data-backend`

This automated system provides a seamless bridge between the frontend and backend, ensuring that all UI interactions can be properly handled with minimal manual coding effort.

## Communication Manager

The Communication Manager is a crucial component of the event system architecture that serves as a coordinating layer between your application's different subsystems. Let's integrate it into the existing architecture:

### Communication Manager Role & Responsibilities

The Communication Manager:

1. **Centralized Communication Hub**: Acts as a mediator for cross-module communication
   - Provides a single point of contact for inter-module messaging
   - Reduces direct dependencies between components

2. **Protocol Management**: Handles different communication protocols
   - WebView2 message passing
   - Native event dispatching
   - External API communications (if applicable)

3. **Message Transformation**: Converts between different message formats
   - Transforms WebView2 messages to UIEvents
   - Converts UIEvents to appropriate formats for external systems

4. **Session Management**: Maintains communication session state
   - Tracks connection status
   - Handles reconnection scenarios
   - Manages communication timeouts

### Integration with Existing Architecture

```
                     ┌─────────────────┐
                     │ Communication   │
WebView2 ───────────▶│    Manager      │───────────┐
                     └─────────────────┘           │
                             │                     │
                             ▼                     ▼
                     ┌─────────────────┐   ┌─────────────────┐
                     │                 │   │                 │
                     │   Event Queue   │◀──│ Event Manager   │
                     │                 │   │                 │
                     └─────────────────┘   └─────────────────┘
                             │                     ▲
                             ▼                     │
                     ┌─────────────────┐           │
                     │                 │           │
                     │Event Dispatcher │───────────┘
                     │                 │
                     └─────────────────┘
                             │
                             ▼
                     ┌─────────────────┐
                     │  Subscribers    │
                     └─────────────────┘
```

### Implementation Guidelines

```cpp
class CommunicationManager {
public:
    CommunicationManager() {
        // Initialize and connect to event system
        auto& eventManager = WindowApp::GetInstance()->GetEventManager();
        
        // Subscribe to events that need to be communicated externally
        eventManager.subscribe("external-message", [this](const HeartControl::UIEvent& evt) {
            sendExternalMessage(evt);
        });
    }
    
    // Handle incoming WebView2 messages
    void handleWebViewMessage(const std::string& message) {
        try {
            auto data = json::parse(message);
            
            // Transform and route the message
            HeartControl::UIEvent evt{
                data.value("type", "web-message"),
                "WebView2",
                message,
                std::chrono::system_clock::now()
            };
            
            // Queue the event
            auto& eventQueue = WindowApp::GetInstance()->GetEventQueue();
            eventQueue.enqueue(std::move(evt));
            
        } catch (const std::exception& e) {
            spdlog::error("Error processing WebView message: {}", e.what());
        }
    }
    
    // Send message to WebView2
    void sendToWebView(const std::string& eventType, const json& data) {
        if (m_webview) {
            json message = {
                {"type", eventType},
                {"data", data},
                {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()}
            };
            
            m_webview->PostWebMessageAsJson(toWideString(message.dump()));
        }
    }

private:
    ICoreWebView2* m_webview = nullptr;
    
    void sendExternalMessage(const HeartControl::UIEvent& evt) {
        // Handle sending messages to external systems
    }
};
```

### Usage Examples

```cpp
// In your WebView2 initialization code
m_webview->add_WebMessageReceived(
    Callback<ICoreWebView2WebMessageReceivedEventHandler>(
        [this](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
            wil::unique_cotaskmem_string message;
            args->TryGetWebMessageAsString(&message);
            
            // Let the Communication Manager handle the message
            m_communicationManager.handleWebViewMessage(WideToUtf8(message.get()));
            
            return S_OK;
        }
    ).Get());

// Sending data to the WebView
m_communicationManager.sendToWebView("data-update", {
    {"temperature", 72.5},
    {"humidity", 45},
    {"status", "normal"}
});
```

### Best Practices for Communication Manager

1. **Protocol Abstraction**: Hide the details of specific communication protocols
   - Clients shouldn't need to know if they're communicating with WebView2 or a native component

2. **Error Handling**: Implement robust error handling for all communications
   - Log communication failures
   - Implement retry mechanisms where appropriate
   - Provide clear error messages

3. **Versioning**: Consider implementing message versioning for forward/backward compatibility
   - Include version information in messages
   - Handle different message versions appropriately

4. **Security**: Validate all incoming messages
   - Sanitize data from external sources
   - Verify message integrity when appropriate

5. **Performance**: Be mindful of performance implications
   - Batch small messages when possible
   - Consider compression for large payloads
   - Implement throttling for high-frequency communications

6. **Monitoring**: Add monitoring capabilities
   - Track message volumes
   - Monitor response times
   - Alert on communication failures

The Communication Manager completes your architecture by providing a clean, centralized way to handle all external communications, further decoupling your application components and making the system more maintainable and extensible.

## NativeWindowControls and System Integration Guide

### NativeWindowControls Component

The NativeWindowControls component provides a bridge between your native Win32 application and UI controls, offering a standardized way to create, manage, and interact with native window controls.

### Key Responsibilities

1. **Native Control Management**:

- Creation and management of Win32 controls (buttons, textboxes, sliders, etc.)
- Styling and positioning of controls
- Handling control visibility and state

2. **Event Translation**:

- Conversion of Win32 messages (WM_COMMAND, WM_NOTIFY, etc.) to UIEvents
- Handling of control-specific messages (BN_CLICKED, EN_CHANGE, etc.)

3. **Layout Management**:

- Window resizing and control repositioning
- DPI awareness for controls
- Support for different layouts and orientations

### Implementation Example

```cpp
class NativeWindowControls {
public:
    NativeWindowControls(HWND parentHwnd) : m_parentHwnd(parentHwnd) {
        // Initialize controls
        initializeControls();
        
        // Connect to event system
        auto& eventManager = WindowApp::GetInstance()->GetEventManager();
        
        // Listen for events that need to be communicated externally
        eventManager.subscribe("update-ui-control", [this](const HeartControl::UIEvent& evt) {
            updateControlFromEvent(evt);
        });
    }
    
    // Create a button control
    HWND createButton(int id, const std::string& text, int x, int y, int width, int height) {
        HWND buttonHwnd = CreateWindow(
            L"BUTTON", toWideString(text).c_str(),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x, y, width, height,
            m_parentHwnd, (HMENU)id, GetModuleHandle(NULL), NULL
        );
        
        m_controlMap[id] = buttonHwnd;
        return buttonHwnd;
    }
    
    // Create a slider control
    HWND createSlider(int id, int x, int y, int width, int height, int min, int max) {
        HWND sliderHwnd = CreateWindow(
            TRACKBAR_CLASS, L"",
            WS_CHILD | WS_VISIBLE | TBS_HORZ,
            x, y, width, height,
            m_parentHwnd, (HMENU)id, GetModuleHandle(NULL), NULL
        );
        
        SendMessage(sliderHwnd, TBM_SETRANGE, TRUE, MAKELPARAM(min, max));
        m_controlMap[id] = sliderHwnd;
        return sliderHwnd;
    }
    
    // Process Win32 messages for controls
    bool processMessage(UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
            case WM_COMMAND: {
                int controlId = LOWORD(wParam);
                int notificationCode = HIWORD(wParam);
                
                if (notificationCode == BN_CLICKED && m_controlMap.count(controlId)) {
                    // Create and dispatch button click event
                    HeartControl::UIEvent evt{
                        "button-clicked",
                        "NativeControl",
                        json{{"controlId", controlId}}.dump(),
                        std::chrono::system_clock::now()
                    };
                    
                    // Queue the event
                    auto& eventQueue = WindowApp::GetInstance()->GetEventQueue();
                    eventQueue.enqueue(std::move(evt));
                    return true;
                }
                break;
            }
            
            case WM_HSCROLL: {
                // Handle slider changes
                HWND sliderHwnd = (HWND)lParam;
                int controlId = GetDlgCtrlID(sliderHwnd);
                
                if (m_controlMap.count(controlId)) {
                    int position = SendMessage(sliderHwnd, TBM_GETPOS, 0, 0);
                    
                    HeartControl::UIEvent evt{
                        "slider-changed",
                        "NativeControl",
                        json{{"controlId", controlId}, {"value", position}}.dump(),
                        std::chrono::system_clock::now()
                    };
                    
                    // Queue the event
                    auto& eventQueue = WindowApp::GetInstance()->GetEventQueue();
                    eventQueue.enqueue(std::move(evt));
                    return true;
                }
                break;
            }
        }
        
        return false;
    }

private:
    HWND m_parentHwnd;
    std::unordered_map<int, HWND> m_controlMap;
    
    void initializeControls() {
        // Create application controls
        createButton(101, "Start", 10, 10, 100, 30);
        createSlider(102, 10, 50, 200, 30, 0, 100);
    }
    
    void updateControlFromEvent(const HeartControl::UIEvent& evt) {
        try {
            auto data = json::parse(evt.payload);
            
            if (data.contains("controlId") && data.contains("action")) {
                int controlId = data["controlId"].get<int>();
                std::string action = data["action"].get<std::string>();
                
                if (m_controlMap.count(controlId)) {
                    HWND hwnd = m_controlMap[controlId];
                    
                    if (action == "enable") {
                        EnableWindow(hwnd, TRUE);
                    } else if (action == "disable") {
                        EnableWindow(hwnd, FALSE);
                    } else if (action == "setvalue" && data.contains("value")) {
                        // Set control value based on type
                        if (GetClassName(hwnd) == L"TRACKBAR_CLASS") {
                            int value = data["value"].get<int>();
                            SendMessage(hwnd, TBM_SETPOS, TRUE, value);
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            spdlog::error("Error updating control: {}", e.what());
        }
    }
};
```

## NativeWindowControls vs. CommunicationManager: When to Use Each

These two components serve different but complementary purposes in your architecture:

## NativeWindowControls

**Primary Purpose:** Interface with native Win32 controls and handle UI interactions
**Use When:**

- Creating and managing native Windows controls (buttons, sliders, etc.)
- Handling direct Win32 UI messages and events
- Need to apply Win32-specific UI styling and behaviors
- Working with modal dialogs or custom controls
- Managing UI control state (enabled/disabled, visibility, etc.)

## Example Scenario:

```cpp
// Creating a settings dialog with native controls
NativeWindowControls controls(dialogHwnd);
controls.createButton(101, "Save", 200, 250, 100, 30);
controls.createSlider(102, "Volume", 50, 100, 200, 30);

// Processing Win32 messages in your window procedure
case WM_COMMAND:
case WM_HSCROLL:
    if (controls.processMessage(message, wParam, lParam)) {
        return 0; // Message was handled
    }
    break;
```

## CommunicationManager

**Primary Purpose:** Handle messaging between different parts of your application
**Use When:**

- Communicating between native code and WebView2
- Sending/receiving data to external systems
- Translating between different message formats
- Managing communication protocols
- Implementing connection state management

### Example Scenario:

```cpp
// Sending data to WebView2
m_communicationManager.sendToWebView("update-chart", {
    {"dataset", temperatureReadings},
    {"title", "Temperature Over Time"}
});

// Receiving data from remote sensors
m_communicationManager.subscribe("sensor-data", [](const json& data) {
    processNewSensorReading(data);
});
```

## Working Together

These components work together in your architecture:

1. NativeWindowControls generates UI events:

- User interacts with a native slider
- NativeWindowControls converts WM_HSCROLL to a "slider-changed" UIEvent
- The event is enqueued in the EventQueue

2. EventDispatcher routes the event:

- Subscribers receive the "slider-changed" event
- Business logic processes the new value

3. CommunicationManager relays appropriate information:

- Business logic might decide to update the UI in WebView2
- CommunicationManager sends the updated value to WebView2
- WebView2 updates its display

## Best Practices for Integration

1. **Clear Separation of Concerns**:

- NativeWindowControls should not know about WebView2
- CommunicationManager should not directly manipulate UI controls
- Both should communicate through the event system

2. **Consistent Event Types**:

- Use consistent event naming across the system
- "button-clicked" from NativeWindowControls should match "button-clicked" from WebView2

3. **Unified Control IDs**:

- Use a consistent ID scheme for controls across native and web
- Document the ID ranges for different control types

4. **Centralized State Management**:

- Consider using a state manager that both components can access
- Ensures UI reflects the same state regardless of source

5. **Error Handling**:

- Both components should implement robust error handling
- Log errors appropriately
- Provide fallback behaviors when communication fails

This architecture provides a clean separation between UI control management (NativeWindowControls) and inter-component communication (CommunicationManager), making your system more maintainable and allowing specialized handling for each concern.