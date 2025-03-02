#pragma once
#include <Windows.h>
#include <string>
#include <wil/com.h>
#include <WebView2.h>
#include <dcomp.h>
#include <functional>
#include <memory>
#include <ole2.h>
#include <string>
#include <vector>
#include <winnt.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.ViewManagement.h>

struct SensorData {
    float temperature;
    float pressure;
    float humidity;
    float voltage;
    uint64_t timestamp;  // Added timestamp for tracking
};
using namespace Microsoft::WRL;

class WebView2DataStreamer {
public:
    // Modified constructor to accept a buffer size parameter
    WebView2DataStreamer(wil::com_ptr<ICoreWebView2> webview,
        wil::com_ptr<ICoreWebView2Environment12> environment,
        UINT64 bufferSize = sizeof(SensorData))  // Default to SensorData size, but can be overridden
        : m_webView(webview), m_environment12(environment), m_bufferSize(bufferSize) {

        // Initialize on UI thread
        InitializeBuffers();
    }

    // Initialize buffers (call on UI thread)
    void InitializeBuffers() {
		if (!m_webView || !m_environment12) {
            return;
		}
        m_webView17 = m_webView.try_query<ICoreWebView2_17>();
        if (!m_webView17) {
            throw std::runtime_error("WebView2 does not support ICoreWebView2_17 interface");
        }

        HRESULT hr1 = m_environment12->CreateSharedBuffer(m_bufferSize, &m_sharedBuffer1);
        spdlog::info("CreateSharedBuffer 1 result: 0x{:x}, pointer: {}, size: {} bytes",
            hr1, (m_sharedBuffer1 ? "valid" : "NULL"), m_bufferSize);

        HRESULT hr2 = m_environment12->CreateSharedBuffer(m_bufferSize, &m_sharedBuffer2);
        spdlog::info("CreateSharedBuffer 2 result: 0x{:x}, pointer: {}, size: {} bytes",
            hr2, (m_sharedBuffer2 ? "valid" : "NULL"), m_bufferSize);

        if (FAILED(hr1) || !m_sharedBuffer1 || FAILED(hr2) || !m_sharedBuffer2) {
            throw std::runtime_error("Failed to create shared buffers");
        }
    }

    // Generic template method to queue any data type
    template<typename T>
    void QueueData(const T& data) {
        // Make sure data fits in buffer
        if (sizeof(T) > m_bufferSize) {
            spdlog::error("Data size ({} bytes) exceeds buffer size ({} bytes)",
                sizeof(T), m_bufferSize);
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        // Create a byte array copy of the data
        std::vector<uint8_t> dataCopy(sizeof(T));
        memcpy(dataCopy.data(), &data, sizeof(T));

        // Queue the data along with its size
        m_dataQueue.push(std::make_pair(dataCopy, sizeof(T)));
    }

    // Queue a string or arbitrary data
    void QueueData(const void* data, size_t dataSize) {
        if (dataSize > m_bufferSize) {
            spdlog::error("Data size ({} bytes) exceeds buffer size ({} bytes)",
                dataSize, m_bufferSize);
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<uint8_t> dataCopy(dataSize);
        memcpy(dataCopy.data(), data, dataSize);

        m_dataQueue.push(std::make_pair(dataCopy, dataSize));
    }

    // Process queued data (call on UI thread)
    void ProcessQueue(int maxItems = 10) {
        std::lock_guard<std::mutex> lock(m_mutex);

        int processed = 0;
        while (!m_dataQueue.empty() && processed < maxItems) {
            const auto& dataItem = m_dataQueue.front();
            const std::vector<uint8_t>& dataCopy = dataItem.first;
            size_t dataSize = dataItem.second;

            StreamData(dataCopy.data(), dataSize);
            m_dataQueue.pop();
            processed++;
        }
    }

    // Get the current buffer size
    UINT64 GetBufferSize() const {
        return m_bufferSize;
    }

private:
    // Stream data to WebView (UI thread only)
    void StreamData(const void* data, size_t dataSize) {
        // Determine which buffer to use
		if (!m_sharedBuffer1 || !m_sharedBuffer2) {
			//spdlog::error("Shared buffers not initialized");
			return;
		}
        auto activeBuffer = m_useBuffer1 ? m_sharedBuffer1.get() : m_sharedBuffer2.get();

        // Get stream for writing to shared buffer
        wil::com_ptr<IStream> stream;
        HRESULT hr = activeBuffer->OpenStream(&stream);
        if (FAILED(hr)) {
            spdlog::error("OpenStream failed: 0x{:x}", hr);
            return;
        }

        // Write data to the stream
        hr = stream->Write(data, static_cast<ULONG>(dataSize), nullptr);
        if (FAILED(hr)) {
            spdlog::error("Stream->Write failed: 0x{:x}", hr);
            return;
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

        // Post the buffer to the script
        hr = m_webView17->PostSharedBufferToScript(
            activeBuffer,
            COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY,
            metadata.c_str());

        if (FAILED(hr)) {
            spdlog::error("PostSharedBufferToScript failed: 0x{:x}", hr);
            return;
        }

        // Switch buffers for next iteration
        m_useBuffer1 = !m_useBuffer1;
    }

    // Member variables
    wil::com_ptr<ICoreWebView2> m_webView;
    wil::com_ptr<ICoreWebView2_17> m_webView17;
    wil::com_ptr<ICoreWebView2Environment12> m_environment12;
    wil::com_ptr<ICoreWebView2SharedBuffer> m_sharedBuffer1;
    wil::com_ptr<ICoreWebView2SharedBuffer> m_sharedBuffer2;
    bool m_useBuffer1 = true;
    UINT64 m_bufferSize;

    // Thread synchronization
    std::mutex m_mutex;
    std::queue<std::pair<std::vector<uint8_t>, size_t>> m_dataQueue;
};