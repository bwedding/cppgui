#pragma once
#include <WebView2.h>
#include <wil/com.h>
#include <string>
#include <thread>
#include <atomic>
#include <random>
#include <sstream>
#include <glaze/glaze.hpp>
#include <queue>
#include <mutex>
#include <windows.h>
#include "AppMessageIDs.h" // Include for message IDs

// Maximum size for string data in bytes
constexpr size_t MAX_STRING_SIZE = 512 * 1024; // 512KB

// Message types
enum class MessageType {
    String,
    Json
};

// Custom Windows messages
// WM_PROCESS_WEBVIEW_MESSAGE and WM_PROCESS_SHARED_BUFFER are defined in AppMessageIDs.h

// Structure to hold pending messages
struct WebViewMessage {
    MessageType type;
    std::wstring data;
};

// Function to initialize the data sender system
void InitializeDataSender(HWND hwnd);

// Functions to send data to WebView in a loop
void SendStringData(HWND targetWindow);
void SendJSONData(HWND targetWindow);

// Function to queue a message for sending on the UI thread
void QueueWebViewMessage(const WebViewMessage& message);

// Function to process messages on the UI thread
LRESULT ProcessWebViewMessage(HWND hwnd, WPARAM wParam, LPARAM lParam);

// Function to process shared buffer messages
LRESULT ProcessSharedBufferMessage(HWND hwnd, WPARAM wParam, LPARAM lParam);

// Global atomic flag to control the sender threads
extern std::atomic<bool> g_keepSending;
