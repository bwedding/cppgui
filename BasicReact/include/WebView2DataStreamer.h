#pragma once
#include <Windows.h>
#include <string>
#include <algorithm>
#include <wil/com.h>
#include <WebView2.h>
#include <dcomp.h>
#include <functional>
#include <queue>
#include <memory>
#include <ole2.h>
#include <mutex>
#include <vector>
#include <winnt.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <plog/Log.h>
#include <chrono>
#include <cstdlib>
using namespace Microsoft::WRL;

struct SensorData
{
    float temperature[24096];
    float pressure[24096];
    float humidity[24096];
    float voltage[24096];
    uint64_t timestamp[24096];
};

class WebView2DataStreamer
{
public:
    // Modified constructor to accept a buffer size parameter
    WebView2DataStreamer(wil::com_ptr<ICoreWebView2> webview,
                         wil::com_ptr<ICoreWebView2Environment12> environment,
                         UINT64 bufferSize = sizeof(SensorData))
        : m_webView(webview), m_environment12(environment), m_bufferSize(bufferSize)
    {
        InitializeBuffers();
    }

    // Initialize buffers (call on UI thread)
    void InitializeBuffers()
    {
        if (!m_webView || !m_environment12)
        {
            return;
        }
        m_webView17 = m_webView.try_query<ICoreWebView2_17>();

        if (!m_webView17)
        {
            throw std::runtime_error("WebView2 does not support ICoreWebView2_17 interface");
        }

        HRESULT hr1 = m_environment12->CreateSharedBuffer(m_bufferSize, &m_sharedBuffer1);
        PLOGI << "CreateSharedBuffer 1 result: 0x{:x}" << hr1 << " pointer: " << "size : " << (m_sharedBuffer1 ? "valid" : "NULL") << m_bufferSize;

        HRESULT hr2 = m_environment12->CreateSharedBuffer(m_bufferSize, &m_sharedBuffer2);
        PLOGI << "CreateSharedBuffer 2 result: 0x{:x}" << hr1 << " pointer: " << "size : " << (m_sharedBuffer1 ? "valid" : "NULL") << m_bufferSize;

        if (FAILED(hr1) || !m_sharedBuffer1 || FAILED(hr2) || !m_sharedBuffer2)
        {
            throw std::runtime_error("Failed to create shared buffers");
        }

        // Initialize ping-pong state
        m_waitingForPong = false;
        m_lastPongTime = std::chrono::high_resolution_clock::now();
    }

    // Thread-safe method to prepare data for the shared buffer
    // This can be called from any thread
    template<typename T>
    bool PrepareSharedBufferData(const T& data)
    {
        // Make sure data fits in buffer
        if (sizeof(T) > m_bufferSize)
        {
            PLOGE << "Data size: " << sizeof(T) << " exceeds buffer size: " << m_bufferSize;
            return false;
        }

        std::lock_guard<std::mutex> lock(m_mutex);

        // Create a copy of the data for the queue
        std::vector<uint8_t> dataCopy(sizeof(T));
        memcpy(dataCopy.data(), &data, sizeof(T));

        // Queue the data with its size
        m_dataQueue.push(std::make_pair(dataCopy, sizeof(T)));

        return true;
    }

    // Thread-safe method to prepare arbitrary data for the shared buffer
    // This can be called from any thread
    bool PrepareSharedBufferData(const void* data, size_t dataSize)
    {
        if (dataSize > m_bufferSize)
        {
            PLOGE << "Data size ({} bytes) exceeds buffer size: " << dataSize << m_bufferSize;
            return false;
        }

        std::lock_guard<std::mutex> lock(m_mutex);

        // Create a copy of the data for the queue
        std::vector<uint8_t> dataCopy(dataSize);
        memcpy(dataCopy.data(), data, dataSize);

        // Queue the data with its size
        m_dataQueue.push(std::make_pair(dataCopy, dataSize));

        return true;
    }

    // Process the data queue and post to the shared buffer
    // IMPORTANT: This MUST be called from the UI thread
    // Returns true if a buffer was processed, false if no processing occurred
    bool ProcessQueue(int maxItems = 200) 
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // If we're waiting for a pong response and haven't timed out, don't process more data
        if (m_waitingForPong) 
        {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastPongTime).count();
            
            // If we've been waiting for more than 500ms, assume the pong was lost and continue
            // Reduced from 1000ms to 500ms to improve responsiveness
            if (elapsed < 500) 
            {
                // Still waiting for pong and haven't timed out
                return false;
            }
            
            // Timed out waiting for pong
            m_waitingForPong = false;
        }

        // Don't process if queue is empty
        if (m_dataQueue.empty())
        {
            return false;
        }

        // Limit how many items we process at once to avoid UI freezing
        int processed = 0;
        int maxToProcess = (maxItems < 10) ? maxItems : 10; // Process at most 10 items at once to prevent UI freezing
        
        while (!m_dataQueue.empty() && processed < maxToProcess && !m_waitingForPong) 
        {
            const auto& dataItem = m_dataQueue.front();
            const std::vector<uint8_t>& dataCopy = dataItem.first;
            size_t dataSize = dataItem.second;

            // Determine which buffer to use
            if (!m_sharedBuffer1 || !m_sharedBuffer2) 
            {
                PLOGE << "Shared buffers not initialized";
                return false;
            }
            
            auto activeBuffer = m_useBuffer1 ? m_sharedBuffer1.get() : m_sharedBuffer2.get();

            // Get stream for writing to shared buffer
            wil::com_ptr<IStream> stream;
            HRESULT hr = activeBuffer->OpenStream(&stream);
            if (FAILED(hr)) 
            {
                PLOGE << "OpenStream failed: 0x{:x}" << hr;
                m_dataQueue.pop(); // Remove failed item
                processed++;
                continue;
            }

            // Write data to the stream
            hr = stream->Write(dataCopy.data(), static_cast<ULONG>(dataSize), nullptr);
            if (FAILED(hr)) 
            {
                PLOGE << "Stream->Write failed: 0x{:x}" << hr;
                m_dataQueue.pop(); // Remove failed item
                processed++;
                continue;
            }

            // Create metadata JSON with size information
            std::wstring metadata = L"{\"bufferId\":";
            metadata += m_useBuffer1 ? L"1" : L"2";
            metadata += L", \"timestamp\":";
            metadata += std::to_wstring(GetTickCount64());
            metadata += L", \"activeBufferIndex\":";
            metadata += m_useBuffer1 ? L"0" : L"1";
            metadata += L", \"dataSize\":";
            metadata += std::to_wstring(dataSize);
            metadata += L"}";

            // Post the buffer to the script (must be on UI thread)
            hr = m_webView17->PostSharedBufferToScript(
                activeBuffer,
                COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY,
                metadata.c_str());

            if (FAILED(hr)) 
            {
                PLOGE << "PostSharedBufferToScript failed: 0x{:x}" << hr;
                m_dataQueue.pop(); // Remove failed item
                processed++;
                continue;
            }

            // Switch buffers for next iteration
            m_useBuffer1 = !m_useBuffer1;
            m_dataQueue.pop();
            processed++;
            
            // After sending a buffer, wait for pong before sending more
            // This implements flow control
            m_waitingForPong = true;
            m_lastPongTime = std::chrono::high_resolution_clock::now();
            m_lastBufferId = m_useBuffer1 ? 2 : 1; // The buffer we just sent
            
            // We only process one buffer at a time in ping-pong mode
            break;
        }
        
        if (processed > 0) 
        {
            return true;
        }
        
        return false;
    }

    // Handle a pong response from the frontend
    void HandlePongResponse(int bufferId) 
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_waitingForPong && bufferId == m_lastBufferId) 
        {
            m_waitingForPong = false;
            m_lastPongTime = std::chrono::high_resolution_clock::now();
        }
        else if (m_waitingForPong) 
        {
        }
        else 
        {
        }
    }

    // For backward compatibility - queue data
    template<typename T>
    void QueueData(const T& data)
    {
        PrepareSharedBufferData(data);
    }

    // For backward compatibility - queue arbitrary data
    void QueueData(const void* data, size_t dataSize)
    {
        PrepareSharedBufferData(data, dataSize);
    }

    // Clear all queued data (call when stopping sender)
    void ClearQueue()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Create an empty queue and swap with the current queue to clear it
        std::queue<std::pair<std::vector<uint8_t>, size_t>> emptyQueue;
        std::swap(m_dataQueue, emptyQueue);

        // Reset ping-pong state
        m_waitingForPong = false;

        PLOGD << "Cleared shared buffer queue with " << emptyQueue.size() << " pending items";
    }

    // Get the current buffer size
    UINT64 GetBufferSize() const
    {
        return m_bufferSize;
    }

    // Check if queue has data
    bool HasQueuedData() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return !m_dataQueue.empty();
    }

    // Get queue size
    size_t GetQueueSize() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_dataQueue.size();
    }

    // Check if waiting for pong
    bool IsWaitingForPong() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_waitingForPong;
    }

private:
    // Member variables
    wil::com_ptr<ICoreWebView2> m_webView;
    wil::com_ptr<ICoreWebView2_17> m_webView17;
    wil::com_ptr<ICoreWebView2Environment12> m_environment12;
    wil::com_ptr<ICoreWebView2SharedBuffer> m_sharedBuffer1;
    wil::com_ptr<ICoreWebView2SharedBuffer> m_sharedBuffer2;
    bool m_useBuffer1 = true;
    mutable std::mutex m_mutex;
    UINT64 m_bufferSize;
    std::queue<std::pair<std::vector<uint8_t>, size_t>> m_dataQueue;

    // Ping-pong flow control
    bool m_waitingForPong = false;
    std::chrono::high_resolution_clock::time_point m_lastPongTime;
    int m_lastBufferId = 0;
};
