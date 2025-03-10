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
void InitializeDataSender(HWND hwnd) {
    PLOGI << "InitializeDataSender called with window handle: " << hwnd;
    g_targetWindow = hwnd;
}

// Queue a message to be processed by the UI thread
void QueueWebViewMessage(const WebViewMessage& message) {
    if (!g_targetWindow || !IsWindow(g_targetWindow)) {
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
LRESULT ProcessWebViewMessage(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    // Extract the message pointer
    WebViewMessage* pMessage = reinterpret_cast<WebViewMessage*>(lParam);
    if (!pMessage) {
        return 0;
    }
    
    // Get the global WebViewManager
    extern std::unique_ptr<WebViewManager> g_webViewManager;
    if (!g_webViewManager) {
        std::cerr << "WebViewManager not available" << std::endl;
        delete pMessage;
        return 0;
    }
    
    // Verify we're on the UI thread
    if (GetCurrentThreadId() != g_webViewManager->GetUIThreadId()) {
        std::cerr << "ERROR: ProcessWebViewMessage called from wrong thread!" << std::endl;
        delete pMessage;
        return 0;
    }
    
    // Process the message based on its type
    try {
        // Static variables for rate calculation
        static size_t messageCount = 0;
        static size_t totalBytes = 0;
        static auto startTime = std::chrono::high_resolution_clock::now();
        static auto lastLogTime = std::chrono::high_resolution_clock::now();
        
        // Increment counters
        messageCount++;
        totalBytes += pMessage->data.size();
        
        if (pMessage->type == MessageType::String) {
            HRESULT hr = g_webViewManager->PostMessageToWebView(pMessage->data);
            if (FAILED(hr)) {
                std::cerr << "Failed to post string message to WebView: 0x" << std::hex << hr << std::endl;
            }
            
            // Calculate and log rate every 1000 messages
            if (messageCount % 1000 == 0) {
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
        } else if (pMessage->type == MessageType::Json) {
            // For JSON messages, we need to wrap the JSON data in an object with a type field
            // This helps the frontend identify it correctly
            std::wstring jsonWrapper = L"{\"type\":\"json\",\"data\":";
            jsonWrapper += pMessage->data;
            jsonWrapper += L"}";
            
            HRESULT hr = g_webViewManager->PostJSONMessageToWebView(jsonWrapper.c_str());
            if (FAILED(hr)) {
                std::cerr << "Failed to post JSON message to WebView: 0x" << std::hex << hr << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing WebView message: " << e.what() << std::endl;
    }
    
    // Free the message
    delete pMessage;
    return 0;
}

// Function to send string data to WebView in a loop
void SendStringData(HWND targetWindow) {
    if (!targetWindow) {
        std::cerr << "Target window is null, cannot send string data" << std::endl;
        return;
    }

    std::cout << "Starting string data sender thread" << std::endl;

    // Setup random number generator for varying string sizes
    std::random_device rd;
    std::mt19937 generator(rd());
    
    // Generate strings between 1KB and MAX_STRING_SIZE
    std::uniform_int_distribution<size_t> sizeDistribution(1024, MAX_STRING_SIZE);
    
    while (g_keepSending) {
        try {
            // Generate a random string with random size
            size_t stringSize = sizeDistribution(generator);
            std::wstring randomData = StringUtils::GenerateRandomString(stringSize);
            
            // Create a message and queue it for the UI thread
            WebViewMessage message;
            message.type = MessageType::String;
            message.data = randomData;
            
            QueueWebViewMessage(message);
            
            // Small delay to not overwhelm the system
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in string sender thread: " << e.what() << std::endl;
            // Longer delay on exception
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    
    std::cout << "String data sender thread stopped" << std::endl;
}

// Example JSON structure for testing
struct DataMessage {
    std::string type;
    std::string timestamp;
    std::vector<double> values;
    std::map<std::string, std::string> metadata;
};

// Glaze JSON meta definition
namespace glz {
    template<>
    struct meta<DataMessage> {
        using T = DataMessage;
        static constexpr auto value = object(
            "type", &T::type,
            "timestamp", &T::timestamp,
            "values", &T::values,
            "metadata", &T::metadata
        );
    };
}

// Function to send JSON data to WebView in a loop
void SendJSONData(HWND targetWindow) {
    if (!targetWindow) {
        std::cerr << "Target window is null, cannot send JSON data" << std::endl;
        return;
    }

    std::cout << "Starting JSON data sender thread" << std::endl;

    // Setup random number generators
    std::random_device rd;
    std::mt19937 generator(rd());
    
    // Random distribution for number of values in array
    std::uniform_int_distribution<size_t> valueCountDist(10, 100);
    
    // Random distribution for values
    std::uniform_real_distribution<double> valueDist(0.0, 1000.0);
    
    // Random distribution for metadata entries
    std::uniform_int_distribution<size_t> metadataCountDist(5, 20);
    
    // String size for metadata keys/values
    std::uniform_int_distribution<size_t> stringSizeDist(10, 100);
    
    while (g_keepSending) {
        try {
            // Create a timestamp
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            
            // Use localtime_s instead of localtime for thread safety
            std::tm timeinfo;
            localtime_s(&timeinfo, &time_t_now);
            ss << std::put_time(&timeinfo, "%Y-%m-%dT%H:%M:%S");
            
            std::string timestamp = ss.str();
            
            // Create the message structure
            DataMessage message;
            message.type = "json";
            message.timestamp = timestamp;
            
            // Add random values
            size_t valueCount = valueCountDist(generator);
            message.values.reserve(valueCount);
            for (size_t i = 0; i < valueCount; ++i) {
                message.values.push_back(valueDist(generator));
            }
            
            // Add random metadata
            size_t metadataCount = metadataCountDist(generator);
            for (size_t i = 0; i < metadataCount; ++i) {
                std::wstring key = L"key_" + StringUtils::GenerateRandomString(stringSizeDist(generator));
                std::wstring value = StringUtils::GenerateRandomString(stringSizeDist(generator));
                message.metadata[std::string(key.begin(), key.end())] = std::string(value.begin(), value.end());
            }
            
            // Serialize to JSON using Glaze
            std::string jsonStr;
            auto result = glz::write_json(message, jsonStr);
            
            if (result) {
                // Convert to wstring for WebView2
                std::wstring wJsonStr(jsonStr.begin(), jsonStr.end());
                
                // Create a message and queue it for the UI thread
                WebViewMessage webviewMsg;
                webviewMsg.type = MessageType::Json;
                webviewMsg.data = wJsonStr;
                
                QueueWebViewMessage(webviewMsg);
            } else {
                std::cerr << "Failed to serialize JSON data" << std::endl;
            }
            
            // Small delay to not overwhelm the system
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in JSON sender thread: " << e.what() << std::endl;
            // Longer delay on exception
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    
    std::cout << "JSON data sender thread stopped" << std::endl;
}
