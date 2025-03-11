#include "DataSenderManager.h"
#include "WebViewManager.h"
#include "DataSender.h" // For SendWebViewMessage
#include "StringUtils.h" // Include the common utility header
#include "plog/Log.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <glaze/glaze.hpp>
#include "AppMessageIDs.h" // Include for message IDs

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
    
    // Initialize the global data sender with the target window handle
    InitializeDataSender(targetWindow);
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

bool DataSenderManager::StartSender(const std::string& senderType) 
{
    std::lock_guard<std::mutex> lock(m_threadMutex);
    
    // Check if the sender is already running
    if (m_threadRunFlags[senderType]) 
    {
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
    if (!m_threadRunFlags[senderType]) 
    {
        PLOGI << "Sender " << senderType << " is not running";
        return false;
    }
    
    // For shared buffer sender, clear the queue before stopping
    if (senderType == "sharedBuffer") 
    {
        // Get the WebViewManager and data streamer
        extern std::unique_ptr<WebViewManager> g_webViewManager;
        if (g_webViewManager) 
        {
            WebView2DataStreamer* dataStreamer = g_webViewManager->GetDataStreamer();
            if (dataStreamer) 
            {
                // Clear the queue to stop immediately
                dataStreamer->ClearQueue();
            }
        }
        
        // Purge all WM_PROCESS_SHARED_BUFFER messages from the Windows message queue
        // This prevents the message loop from processing queued messages after stopping
        if (m_targetWindow)
        {
            MSG msg;
            int purgedCount = 0;
            // Remove all WM_PROCESS_SHARED_BUFFER messages for this window
            while (PeekMessage(&msg, m_targetWindow, WM_PROCESS_SHARED_BUFFER, WM_PROCESS_SHARED_BUFFER, PM_REMOVE)) 
            {
                purgedCount++;
            }
            PLOGD << "Purged " << purgedCount << " shared buffer messages from Windows message queue";
        }
    }
    
    // Set the run flag to false to signal the thread to stop
    m_threadRunFlags[senderType] = false;
    
    // Join the thread if it's joinable
    if (m_senderThreads[senderType].joinable()) 
    {
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
    std::uniform_int_distribution<size_t> stringSizeDist(250 * 1024, 500 * 1024); // 5-10KB strings

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
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastLogTime).count() >= 1) 
            {
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

            // Just a minimal delay to allow other threads to run
            std::this_thread::sleep_for(std::chrono::microseconds(50));
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
    std::uniform_int_distribution<size_t> valueCountDist(500, 2000);
    
    // Random distribution for values
    std::uniform_real_distribution<double> valueDist(0.0, 1000.0);
    
    // Random distribution for metadata entries
    std::uniform_int_distribution<size_t> metadataCountDist(100, 300);
    
    // String size for metadata keys/values
    std::uniform_int_distribution<size_t> stringSizeDist(2000, 10000);

    // For rate calculation
    size_t messageCount = 0;
    size_t totalBytes = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastLogTime = std::chrono::high_resolution_clock::now();

    while (m_threadRunFlags["json"]) 
    {
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
            for (size_t i = 0; i < valueCount; ++i) 
            {
                message.values.push_back(valueDist(generator));
            }
            
            // Add random metadata
            size_t metadataCount = metadataCountDist(generator);
            for (size_t i = 0; i < metadataCount; ++i) {
                // Use ASCII string directly instead of converting from wstring
                std::string key = "key_" + StringUtils::GenerateRandomAsciiString(stringSizeDist(generator));
                std::string value = StringUtils::GenerateRandomAsciiString(stringSizeDist(generator));
                message.metadata[key] = value;
            }
            
            // Serialize to JSON using Glaze
            std::string jsonStr;
            auto result = glz::write_json(message, jsonStr);
            
            if (!result) 
            {
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
                if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastLogTime).count() >= 1) 
                {
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

            // Just a minimal delay to allow other threads to run
            std::this_thread::sleep_for(std::chrono::microseconds(50));
            //Sleep(0);
            
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

void DataSenderManager::SharedBufferSenderThreadFunc(HWND targetWindow, std::atomic<bool>* runFlag) 
{
    PLOGI << "Shared buffer sender thread started";
    
    // Declare the external global variable
    extern std::unique_ptr<WebViewManager> g_webViewManager;
    
    // Check if the WebViewManager is available
    if (!g_webViewManager) 
    {
        PLOGE << "Failed to get WebViewManager: global variable is null";
        return;
    }
    
    // Get the data streamer from the WebViewManager
    WebView2DataStreamer* dataStreamer = g_webViewManager->GetDataStreamer();
    if (!dataStreamer) 
    {
        PLOGE << "Failed to get WebView2DataStreamer";
        return;
    }
    
    // Setup random number generators for generating random sensor data
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<float> tempDist(-10.0f, 40.0f);      // Temperature range
    std::uniform_real_distribution<float> pressureDist(980.0f, 1030.0f); // Pressure range in hPa
    std::uniform_real_distribution<float> humidityDist(0.0f, 100.0f);    // Humidity range in %
    std::uniform_real_distribution<float> voltageDist(0.0f, 5.0f);       // Voltage range in V
    
    // For rate calculation
    size_t messageCount = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastLogTime = std::chrono::high_resolution_clock::now();
    
    while (*runFlag) 
    {
        try 
        {
            // Create sensor data structure
            SensorData sensorData;
            
            // Fill with random data
            uint64_t currentTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
                
            for (int i = 0; i < 4096; i++) 
            {
                sensorData.temperature[i] = tempDist(generator);
                sensorData.pressure[i] = pressureDist(generator);
                sensorData.humidity[i] = humidityDist(generator);
                sensorData.voltage[i] = voltageDist(generator);
                sensorData.timestamp[i] = currentTimestamp + i;
            }
            
            // Queue the data to the streamer
            dataStreamer->QueueData(sensorData);
            
            // Post a message to the UI thread to process the shared buffer queue
            PostMessage(targetWindow, WM_PROCESS_SHARED_BUFFER, 0, 0);
            
            // Send a notification to the frontend that data is ready
            // Create a simple JSON message
            json dataReadyMsg = 
            {
                {"type", "sharedBuffer"},
                {"action", "dataready"},
                {"timestamp", std::to_string(currentTimestamp)},
                {"size", sizeof(SensorData)}
            };
            
            // Convert to string and send
            std::string jsonStr = dataReadyMsg.dump();
            WebViewMessage webviewMsg;
            webviewMsg.type = MessageType::Json;
            webviewMsg.data = StringUtils::Utf8ToWide(jsonStr);
            QueueWebViewMessage(webviewMsg);
            
            // Update message count for rate calculation
            messageCount++;
            
            // Calculate and log rate every second
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastLogTime).count();
            
            if (elapsed >= 1) 
            {
                double rate = static_cast<double>(messageCount) / elapsed;
                double mbPerSec = (rate * sizeof(SensorData)) / (1024.0 * 1024.0);
                
                PLOGI << "Shared buffer sending rate: " << std::fixed << std::setprecision(2) 
                      << rate << " msgs/sec (" << mbPerSec << " MB/sec)";
                
                messageCount = 0;
                lastLogTime = now;
            }
            
            // Sleep to control the rate (adjust as needed)
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Reduced from 100ms to 1ms for much higher throughput
        }
        catch (const std::exception& e) 
        {
            PLOGE << "Error in shared buffer sender thread: " << e.what();
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep longer on error
        }
    }
    
    PLOGI << "Shared buffer sender thread stopped";
}
