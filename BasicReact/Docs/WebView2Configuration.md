# WebView2 Configuration Guide

This document provides a comprehensive guide to configuring and customizing WebView2 controls in your application using the `WebViewManager` class.

## Table of Contents

1. [Basic Configuration](#basic-configuration)
2. [Navigation Options](#navigation-options)
3. [WebView2 Settings](#webview2-settings)
4. [Error Handling](#error-handling)
5. [Event Handling](#event-handling)
6. [JavaScript Integration](#javascript-integration)
7. [Complete Examples](#complete-examples)

## Basic Configuration

The `WebViewManager` class manages the WebView2 control with various configuration options:

```cpp
// Create the WebView manager
WebViewManager webViewManager(hWnd, hInstance);

// Customize WebView before initialization
webViewManager.SetNavigationURL(L"https://example.com");
webViewManager.SetDevToolsEnabled(true);

// Initialize WebView2 with the current settings
webViewManager.Initialize();
```

## Navigation Options

### Setting Navigation URL

You can set the initial navigation URL before initialization:

```cpp
// Navigate to a web URL
webViewManager.SetNavigationURL(L"https://example.com");

// Or navigate to a local file
webViewManager.SetNavigationURL(L"file:///C:/Path/To/index.html");
```

### Local Folder Navigation

For local file-based navigation with path sanitization and 404 handling:

```cpp
// Set the base folder for local navigation
webViewManager.SetLocalFolder(L"C:\\Path\\To\\Your\\WebAppFiles\\");

// Navigate to specific pages relative to the local folder
webViewManager.NavigateToPage(L"index.html");  // Navigates to C:\Path\To\Your\WebAppFiles\index.html
webViewManager.NavigateToPage(L"pages/about.html");  // Navigates to C:\Path\To\Your\WebAppFiles\pages\about.html
```

The `NavigateToPage` method includes:
- Path sanitization to prevent directory traversal attacks
- Path length validation
- Automatic 404 handling for missing files

### Custom 404 Pages

You can customize the 404 error page:

```cpp
// Set a custom 404 page template (use %PAGE% as a placeholder for the requested page)
webViewManager.SetCustom404Page(L"<html><head><title>Not Found</title></head>"
    L"<body><h1>Page Not Found</h1>"
    L"<p>Sorry, the page '%PAGE%' could not be found.</p></body></html>");
```

## WebView2 Settings

### Developer Tools

```cpp
// Enable or disable developer tools (F12)
webViewManager.SetDevToolsEnabled(true);  // Enable developer tools
webViewManager.SetDevToolsEnabled(false); // Disable developer tools
```

### Context Menus

```cpp
// Enable or disable default context menus (right-click menu)
webViewManager.SetDefaultContextMenusEnabled(true);  // Enable context menus
webViewManager.SetDefaultContextMenusEnabled(false); // Disable context menus
```

### Error Pages

```cpp
// Enable or disable built-in error pages
webViewManager.SetBuiltInErrorPageEnabled(true);  // Use WebView2's built-in error pages
webViewManager.SetBuiltInErrorPageEnabled(false); // Disable built-in error pages (use custom handling)
```

### Status Bar

```cpp
// Enable or disable the status bar (shown when hovering links)
webViewManager.SetStatusBarEnabled(true);  // Show status bar on hover
webViewManager.SetStatusBarEnabled(false); // Hide status bar
```

### Zoom Control

```cpp
// Enable or disable zoom control (Ctrl+/- and mouse wheel)
webViewManager.SetZoomControlEnabled(true);  // Allow users to zoom
webViewManager.SetZoomControlEnabled(false); // Disable zooming
```

## Error Handling

### Navigation Callbacks

You can register callbacks to handle navigation results:

```cpp
// Option 1: Detailed callback with error status codes
webViewManager.SetNavigationCompletedCallback(
    [](const std::wstring& uri, bool isSuccess, COREWEBVIEW2_WEB_ERROR_STATUS errorStatus) {
        if (!isSuccess) {
            // Handle error with specific error status
            switch (errorStatus) {
                case COREWEBVIEW2_WEB_ERROR_STATUS_CANNOT_CONNECT:
                    // Handle connection error
                    break;
                // Handle other error types
            }
        }
    }
);

// Option 2: Simplified callback with user-friendly error messages
webViewManager.SetSimpleNavigationCallback(
    [](const std::wstring& uri, bool isSuccess, const std::wstring& errorMessage) {
        if (!isSuccess) {
            // Display user-friendly error message
            MessageBox(nullptr, errorMessage.c_str(), L"Navigation Error", MB_OK | MB_ICONERROR);
        }
    }
);
```

## Event Handling

### Subscribing to UI Events

You can subscribe to events from your web UI:

```cpp
// Subscribe to events from the WebView
webViewManager.Subscribe("button-click", [](const CPPGUI::UIEvent& evt) {
    // Handle button click event from web content
    auto data = evt.data; // JSON data from the event
    
    // Process event data
    // ...
    
    return "success"; // Return value to JavaScript
});
```

### Event Flow

1. JavaScript triggers events using the native bridge
2. Events are processed by the `NativeWindowControls` class
3. Your C++ callback is executed
4. Return values are sent back to JavaScript (if applicable)

## JavaScript Integration

### Executing JavaScript

```cpp
// Execute JavaScript in the WebView
webViewManager.ExecuteScript(L"document.body.style.backgroundColor = 'lightblue';");
```

### Posting Messages to WebView

```cpp
// Send data to the WebView
nlohmann::json data = {
    {"type", "update-theme"},
    {"theme", "dark"},
    {"accent", "#0078D7"}
};
webViewManager.PostMessageToWebView(L"" + SystemUtils::UTF8_to_wstring(data.dump()));
```

### Native Object Bridge

The `WebViewManager` automatically exposes a JavaScript object called `native` to your web content:

```javascript
// In JavaScript
async function callNative() {
    // Call a method on the native object
    const result = await native.myNativeMethod("parameter1", 42);
    console.log("Native result:", result);
    
    // Or send an event
    native.sendEvent("button-click", { id: "submit-button", value: "Submit" });
}
```

## Complete Examples

### Basic WebView Application

```cpp
// Create and configure WebView
WebViewManager webViewManager(hWnd, hInstance);
webViewManager.SetNavigationURL(L"https://example.com");
webViewManager.SetDevToolsEnabled(true);
webViewManager.SetZoomControlEnabled(true);
webViewManager.Initialize();
```

### Local Web Application

```cpp
// Create and configure WebView for local content
WebViewManager webViewManager(hWnd, hInstance);
webViewManager.SetLocalFolder(L"C:\\MyApp\\WebContent\\");
webViewManager.SetCustom404Page(L"<html><body><h1>Page Not Found</h1><p>Could not find %PAGE%</p></body></html>");
webViewManager.SetBuiltInErrorPageEnabled(false);
webViewManager.Initialize();

// Navigate to the main page
webViewManager.NavigateToPage(L"index.html");

// Handle navigation events
webViewManager.SetSimpleNavigationCallback([](const std::wstring& uri, bool isSuccess, const std::wstring& errorMessage) {
    if (!isSuccess) {
        LOGE << "Navigation failed: " << SystemUtils::WideToUtf8(errorMessage);
    } else {
        LOGI << "Successfully navigated to: " << SystemUtils::WideToUtf8(uri);
    }
});
```

### Interactive Application with Two-Way Communication

```cpp
// Create and configure WebView
WebViewManager webViewManager(hWnd, hInstance);
webViewManager.SetNavigationURL(L"file:///C:/MyApp/index.html");
webViewManager.Initialize();

// Subscribe to events from web UI
webViewManager.Subscribe("theme-changed", [&webViewManager](const CPPGUI::UIEvent& evt) {
    // Extract theme from event data
    std::string theme = evt.data["theme"];
    
    // Update application theme
    if (theme == "dark") {
        // Set dark theme...
    } else {
        // Set light theme...
    }
    
    // Send confirmation back to WebView
    nlohmann::json response = {
        {"type", "theme-update-complete"},
        {"success", true}
    };
    webViewManager.PostMessageToWebView(L"" + SystemUtils::UTF8_to_wstring(response.dump()));
    
    return "success";
});

// Resize WebView when window is resized
void OnSize(HWND hWnd) {
    RECT bounds;
    GetClientRect(hWnd, &bounds);
    webViewManager.Resize(bounds);
}