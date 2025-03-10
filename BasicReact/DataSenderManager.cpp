#include "DataSenderManager.h"
#include "WebViewManager.h"
//#include "WebViewMessages.h"
#include "DataSender.h" // For SendWebViewMessage
#include "StringUtils.h" // Include the common utility header
#include "plog/Log.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <glaze/glaze.hpp>

using json = nlohmann::json;

// Data message class for JSON tests - similar to the one in DataSender.cpp
struct DataMessage {
    std::string type;
    std::string timestamp;
    std::vector<double> values;
    std::map<std::string, std::string> metadata;
};

// Glaze JSON meta definition
namespace glz {
    template <>
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

DataSenderManager::DataSenderManager(HWND targetWindow)
    : m_targetWindow(targetWindow) {
    // Initialize thread run flags
    m_threadRunFlags["string"] = false;
    m_threadRunFlags["json"] = false;
    m_threadRunFlags["nativeObject"] = false;
    m_threadRunFlags["sharedBuffer"] = false;
}

DataSenderManager::~DataSenderManager() {
    StopAllSenders();
}

std::string DataSenderManager::HandleDataSenderEvent(const CPPGUI::UIEvent& evt) {
    try {
        // Get the JSON data from the event
        std::string jsonStr = evt.payload;
        PLOGI << "Received data sender control event: " << jsonStr;
        
        // Parse the JSON
        json jsonData = json::parse(jsonStr);
        
        // Extract the sender type and action
        std::string senderType = jsonData["senderType"];
        std::string action = jsonData["action"];
        
        PLOGI << "Data sender control: " << senderType << " " << action;
        
        json response = {
            {"senderType", senderType},
            {"action", action},
            {"success", false}
        };
        
        if (action == "start") {
            bool success = StartSender(senderType);
            response["success"] = success;
        } 
        else if (action == "stop") {
            bool success = StopSender(senderType);
            response["success"] = success;
        }
        
        return response.dump();
    }
    catch (const std::exception& e) {
        PLOGE << "Error handling data sender event: " << e.what();
        json errorResponse = {
            {"error", e.what()}
        };
        return errorResponse.dump();
    }
}

bool DataSenderManager::StartSender(const std::string& senderType) {
    std::lock_guard<std::mutex> lock(m_threadMutex);
    
    // Check if the sender is already running
    if (m_threadRunFlags[senderType]) {
        PLOGI << "Sender " << senderType << " is already running";
        return false;
    }
    
    // Set the run flag to true
    m_threadRunFlags[senderType] = true;
    
    // Start the appropriate thread
    try {
        if (senderType == "string") {
            m_senderThreads[senderType] = std::thread(&DataSenderManager::StringSenderThreadFunc, this);
            PLOGI << "Started string sender thread";
        }
        else if (senderType == "json") {
            m_senderThreads[senderType] = std::thread(&DataSenderManager::JsonSenderThreadFunc, this);
            PLOGI << "Started JSON sender thread";
        }
        else if (senderType == "nativeObject") {
            m_senderThreads[senderType] = std::thread(NativeObjectSenderThreadFunc, m_targetWindow, &m_threadRunFlags[senderType]);
            PLOGI << "Started native object sender thread";
        }
        else if (senderType == "sharedBuffer") {
            m_senderThreads[senderType] = std::thread(SharedBufferSenderThreadFunc, m_targetWindow, &m_threadRunFlags[senderType]);
            PLOGI << "Started shared buffer sender thread";
        }
        else {
            PLOGE << "Unknown sender type: " << senderType;
            m_threadRunFlags[senderType] = false;
            return false;
        }
        
        return true;
    }
    catch (const std::exception& e) {
        PLOGE << "Error starting " << senderType << " sender: " << e.what();
        m_threadRunFlags[senderType] = false;
        return false;
    }
}

bool DataSenderManager::StopSender(const std::string& senderType) {
    std::lock_guard<std::mutex> lock(m_threadMutex);
    
    // Check if the sender is running
    if (!m_threadRunFlags[senderType]) {
        PLOGI << "Sender " << senderType << " is not running";
        return false;
    }
    
    // Set the run flag to false to signal the thread to stop
    m_threadRunFlags[senderType] = false;
    
    // Join the thread if it's joinable
    if (m_senderThreads[senderType].joinable()) {
        PLOGI << "Joining " << senderType << " sender thread";
        m_senderThreads[senderType].join();
    }
    
    PLOGI << "Stopped " << senderType << " sender";
    return true;
}

bool DataSenderManager::IsSenderRunning(const std::string& senderType) {
    std::lock_guard<std::mutex> lock(m_threadMutex);
    return m_threadRunFlags[senderType];
}

void DataSenderManager::StopAllSenders() {
    PLOGI << "Stopping all data senders";
    
    // Stop all running senders
    StopSender("string");
    StopSender("json");
    StopSender("nativeObject");
    StopSender("sharedBuffer");
}

void DataSenderManager::StringSenderThreadFunc() {
    LOGI << "Starting string data sender thread";

    // Setup random number generators for generating random strings
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<size_t> stringSizeDist(5 * 1024, 10 * 1024); // 5-10KB strings

    // For rate calculation
    size_t messageCount = 0;
    size_t totalBytes = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastLogTime = std::chrono::high_resolution_clock::now();

    while (m_threadRunFlags["string"]) {
        try {
            // Generate a random string of varying size
            size_t stringSize = stringSizeDist(generator);
            std::wstring randomString = StringUtils::GenerateRandomString(stringSize);

            // Create a WebViewMessage for the string data
            WebViewMessage webviewMsg;
            webviewMsg.type = MessageType::String;
            webviewMsg.data = randomString;
            
            // Queue the message to be processed on the UI thread
            QueueWebViewMessage(webviewMsg);

            // Update counters for rate calculation
            messageCount++;
            totalBytes += randomString.size() * sizeof(wchar_t); // Calculate actual bytes (2 bytes per wchar_t)

            // Calculate and log the rate every second
            auto now = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastLogTime).count() >= 1) {
                auto elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime).count();
                double messagesPerSecond = messageCount / elapsedSeconds;
                double bytesPerSecond = totalBytes / elapsedSeconds;
                double megaBytesPerSecond = bytesPerSecond / (1024 * 1024);
                
                // Calculate UTF-8 equivalent rate (approx)
                double utf8MegaBytesPerSecond = megaBytesPerSecond / 2; // Rough estimation assuming average 1 byte per char in UTF-8
                
                LOGI << "String sender rate: " << messagesPerSecond << " msgs/sec, " 
                     << megaBytesPerSecond << " MB/sec (" << totalBytes << " bytes in " 
                     << elapsedSeconds << " seconds), UTF-8 equivalent: " << utf8MegaBytesPerSecond << " MB/sec";
                
                lastLogTime = now;
            }

            // Add a small delay to prevent overwhelming the system
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        catch (const std::exception& e) {
            LOGE << "Exception in string sender thread: " << e.what();
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep longer on error
        }
    }

    LOGI << "String data sender thread stopped";
}

void DataSenderManager::JsonSenderThreadFunc() {
    LOGI << "Starting JSON data sender thread";

    // Setup random number generators
    std::random_device rd;
    std::mt19937 generator(rd());
    
    // Random distribution for number of values in array
    std::uniform_int_distribution<size_t> valueCountDist(50, 200);
    
    // Random distribution for values
    std::uniform_real_distribution<double> valueDist(0.0, 1000.0);
    
    // Random distribution for metadata entries
    std::uniform_int_distribution<size_t> metadataCountDist(10, 30);
    
    // String size for metadata keys/values
    std::uniform_int_distribution<size_t> stringSizeDist(20, 100);

    // For rate calculation
    size_t messageCount = 0;
    size_t totalBytes = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastLogTime = std::chrono::high_resolution_clock::now();

    while (m_threadRunFlags["json"]) {
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
                
                // Queue the message
                QueueWebViewMessage(webviewMsg);
                
                // Update counters for rate calculation
                messageCount++;
                totalBytes += wJsonStr.size() * sizeof(wchar_t); // Calculate actual bytes (2 bytes per wchar_t)

                // Calculate and log the rate every second
                auto currentTime = std::chrono::high_resolution_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastLogTime).count() >= 1) {
                    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime).count();
                    double messagesPerSecond = messageCount / elapsedSeconds;
                    double bytesPerSecond = totalBytes / elapsedSeconds;
                    double megaBytesPerSecond = bytesPerSecond / (1024 * 1024);
                    
                    // Calculate UTF-8 equivalent rate (approx)
                    double utf8MegaBytesPerSecond = megaBytesPerSecond / 2; // Rough estimation assuming average 1 byte per char in UTF-8
                    
                    LOGI << "JSON sender rate: " << messagesPerSecond << " msgs/sec, " 
                         << megaBytesPerSecond << " MB/sec (" << totalBytes << " bytes in " 
                         << elapsedSeconds << " seconds), UTF-8 equivalent: " << utf8MegaBytesPerSecond << " MB/sec";
                    
                    lastLogTime = currentTime;
                }
            } else {
                LOGE << "Failed to serialize JSON data";
            }

            // Add a small delay to prevent overwhelming the system
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        catch (const std::exception& e) {
            LOGE << "Exception in JSON sender thread: " << e.what();
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep longer on error
        }
    }

    LOGI << "JSON data sender thread stopped";
}

void DataSenderManager::NativeObjectSenderThreadFunc(HWND targetWindow, std::atomic<bool>* runFlag) {
    PLOGI << "Native object sender thread started (placeholder - not yet implemented)";
    
    while (*runFlag) {
        // Implementation needs to be added for native object sending
        // This will depend on how native objects are currently handled
        
        // Sleep to avoid spinning the CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    PLOGI << "Native object sender thread stopped";
}

void DataSenderManager::SharedBufferSenderThreadFunc(HWND targetWindow, std::atomic<bool>* runFlag) {
    PLOGI << "Shared buffer sender thread started (placeholder - not yet implemented)";
    
    while (*runFlag) {
        // Implementation needs to be added for shared buffer sending
        // This will depend on how shared buffers are currently handled
        
        // Sleep to avoid spinning the CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    PLOGI << "Shared buffer sender thread stopped";
}
