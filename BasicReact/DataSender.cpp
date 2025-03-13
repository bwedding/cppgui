#include "DataSender.h"
#include <chrono>
#include <iostream>
#include <queue>
#include <mutex>
#include "WebViewManager.h"
#include "StringUtils.h"

// Global atomic flag to control the sender threads
std::atomic<bool> g_keepSending(true);

// Global message queue
std::queue<WebViewMessage> g_messageQueue;
std::mutex g_queueMutex;
HWND g_targetWindow = NULL;

// Initialize the data sender system with the target window handle
void InitializeDataSender(HWND hwnd)
{
    PLOGI << "InitializeDataSender called with window handle: " << hwnd;
    g_targetWindow = hwnd;
}

// Queue a message to be processed by the UI thread
void QueueWebViewMessage(const WebViewMessage& message)
{
    if (!g_targetWindow || !IsWindow(g_targetWindow))
    {
        PLOGE << "Invalid target window handle: " << g_targetWindow << ", cannot queue messages";
        return;
    }

    // Allocate a copy of the message on the heap (will be freed in the UI thread)
    WebViewMessage* pMessage = new WebViewMessage(message);

    //PLOGI << "Posting message to window " << g_targetWindow << ", type: " << (message.type == MessageType::String ? "String" : "JSON")
    //      << ", data length: " << message.data.length();

    // Post a message to the UI thread with a pointer to the data
    if (!PostMessage(g_targetWindow, WM_PROCESS_WEBVIEW_MESSAGE, 0, reinterpret_cast<LPARAM>(pMessage)))
    {
        PLOGE << "Failed to post message to UI thread, error: " << GetLastError();
        delete pMessage; // Clean up if PostMessage fails
    }
}

// Process WebView messages on the UI thread
LRESULT ProcessWebViewMessage(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    // Extract the message pointer
    WebViewMessage* pMessage = reinterpret_cast<WebViewMessage*>(lParam);
    if (!pMessage)
    {
        return 0;
    }

    // Get the global WebViewManager
    extern std::unique_ptr<WebViewManager> g_webViewManager;
    if (!g_webViewManager)
    {
        std::cerr << "WebViewManager not available" << std::endl;
        delete pMessage;
        return 0;
    }

    // Verify we're on the UI thread
    if (GetCurrentThreadId() != g_webViewManager->GetUIThreadId())
    {
        std::cerr << "ERROR: ProcessWebViewMessage called from wrong thread!" << std::endl;
        delete pMessage;
        return 0;
    }

    // Process the message based on its type
    try
    {
        // Static variables for rate calculation
        static size_t messageCount = 0;
        static size_t totalBytes = 0;
        static auto startTime = std::chrono::high_resolution_clock::now();
        static auto lastLogTime = std::chrono::high_resolution_clock::now();

        // Increment counters
        messageCount++;
        totalBytes += pMessage->data.size();

        if (pMessage->type == MessageType::String)
        {
            HRESULT hr = g_webViewManager->PostMessageToWebView(pMessage->data);
            if (FAILED(hr))
            {
                std::cerr << "Failed to post string message to WebView: 0x" << std::hex << hr << std::endl;
            }

            // Calculate and log rate every 1000 messages
            if (messageCount % 1000 == 0)
            {
                auto now = std::chrono::high_resolution_clock::now();
                auto elapsedSec = std::chrono::duration<double>(now - startTime).count();
                auto sinceLastLog = std::chrono::duration<double>(now - lastLogTime).count();

                // When calculating bytes, we need to be clear about character encoding
                // For a std::wstring on Windows, the character count doesn't equal the byte count
                size_t byteSize = pMessage->data.size() * sizeof(wchar_t);

                // Calculate overall average rate (using byte size, not character count)
                double overallRateMbps = (totalBytes * sizeof(wchar_t) * 8.0) / (elapsedSec * 1000000);

                // Calculate rate since last log (using byte size, not character count)
                double instantRateMbps = ((1000 * byteSize) * 8.0) / (sinceLastLog * 1000000);

                // Calculate what the frontend should be receiving if it's getting all our data
                double avgMsgSizeBytes = static_cast<double>(totalBytes * sizeof(wchar_t)) / messageCount;
                double msgsPerSecond = 1000.0 / sinceLastLog;
                double expectedFrontendRateMbps = (avgMsgSizeBytes * msgsPerSecond * 8.0) / 1000000;

                // Convert to UTF-8 equivalent size for comparison with frontend
                // Roughly: wide char (UTF-16) is ~2x the size of UTF-8 for Latin text
                // But this varies with the actual content
                double utf8EquivalentMbps = overallRateMbps / 2.0;

                PLOGI << "BACKEND RATE: Sent " << messageCount << " messages ("
                      << (totalBytes * sizeof(wchar_t)) << " bytes / " << totalBytes << " chars) in "
                      << elapsedSec << " seconds. Overall rate: " << overallRateMbps
                      << " Mb/s, Current rate: " << instantRateMbps << " Mb/s";

                PLOGI << "RATE ANALYSIS: Avg msg size: " << avgMsgSizeBytes << " bytes ("
                      << static_cast<double>(totalBytes) / messageCount << " chars), "
                      << msgsPerSecond << " msgs/sec, Expected frontend rate: "
                      << expectedFrontendRateMbps << " Mb/s, UTF-8 equivalent: "
                      << utf8EquivalentMbps << " Mb/s";

                lastLogTime = now;
            }
        }
        else if (pMessage->type == MessageType::Json)
        {
            // For JSON messages, we need to wrap the JSON data in an object with a type field
            // This helps the frontend identify it correctly
            std::wstring jsonWrapper = L"{\"type\":\"json\",\"data\":";
            jsonWrapper += pMessage->data;
            jsonWrapper += L"}";

            HRESULT hr = g_webViewManager->PostJSONMessageToWebView(jsonWrapper.c_str());
            if (FAILED(hr))
            {
                std::cerr << "Failed to post JSON message to WebView: 0x" << std::hex << hr << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error processing WebView message: " << e.what() << std::endl;
    }

    // Free the message
    delete pMessage;
    return 0;
}

// Process shared buffer messages on the UI thread
LRESULT ProcessSharedBufferMessage(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    // Get the WebViewManager instance from the global variable
    extern std::unique_ptr<WebViewManager> g_webViewManager;

    if (!g_webViewManager)
    {
        PLOGE << "Failed to get WebViewManager: global variable is null in ProcessSharedBufferMessage";
        return 0;
    }

    // Get the data streamer from the WebViewManager
    WebView2DataStreamer* dataStreamer = g_webViewManager->GetDataStreamer();
    if (!dataStreamer)
    {
        PLOGE << "Failed to get WebView2DataStreamer in ProcessSharedBufferMessage";
        return 0;
    }

    // Track if we're already processing to prevent recursive calls
    static bool isProcessing = false;
    if (isProcessing)
    {
        // Already processing, just return
        return 0;
    }

    // Set the processing flag
    isProcessing = true;

    // Process the queued data (up to 10 items at a time to prevent UI freezing)
    // The ProcessQueue function now returns a boolean indicating if data was processed
    bool processed = dataStreamer->ProcessQueue(10); // Process up to 10 items at once
    
    // If we processed data and there's more in the queue, and we're not waiting for a pong,
    // post another message to continue processing
    if (dataStreamer->HasQueuedData() && !dataStreamer->IsWaitingForPong())
    {
        // Post another message to continue processing if there's more data
        // Use a slight delay to give the UI thread time to breathe
        PostMessage(hwnd, WM_PROCESS_SHARED_BUFFER, 0, 0);
    }

    // Clear the processing flag
    isProcessing = false;

    return 0;
}
