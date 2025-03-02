#pragma once
#include <Windows.h>
#include <string>
#include <cstring>
#include <memory>

class DataStream
{
public:
    DataStream() = default;
    ~DataStream() = default;
    bool hasData() const
    {
        return m_data.size() > 0;
    }
    void AddData(const std::string& data)
    {
        m_data += data;
    }
    void ClearData()
    {
        m_data.clear();
    }
    std::string GetData() const
    {
        return m_data;
    }
private:
    std::string m_data;
};

class BufferStatus
{
public:
    BufferStatus() = default;
    ~BufferStatus() = default;
    bool IsBuffer1Ready() const
    {
        return m_buffer1Ready;
    }
    bool IsBuffer2Ready() const
    {
        return m_buffer2Ready;
    }
    void SetBuffer1Ready(bool ready)
    {
        m_buffer1Ready = ready;
    }
    void SetBuffer2Ready(bool ready)
    {
        m_buffer2Ready = ready;
    }
    int activeBufferIndex = 0;
    SYSTEMTIME dataTimestamp{};
    bool newDataAvailable = false;
    // Add data size to know how much data is in the buffer
    size_t dataSize = 0;
private:
    bool m_buffer1Ready = false;
    bool m_buffer2Ready = false;
};

class DataBuffer
{
public:
    DataBuffer() = default;
    ~DataBuffer()
    {
        // Clean up resources
        if (pBuf1) UnmapViewOfFile(pBuf1);
        if (pBuf2) UnmapViewOfFile(pBuf2);
        if (pStatus) UnmapViewOfFile(pStatus);
        if (hMapFile1) CloseHandle(hMapFile1);
        if (hMapFile2) CloseHandle(hMapFile2);
        if (hStatusMap) CloseHandle(hStatusMap);
    }

    HANDLE MakeDoubleBuffer(size_t sz, const std::wstring& name)
    {
        m_bufferSize = sz;
        std::wstring buf1 = name + L"1";
        std::wstring buf2 = name + L"2";
        hMapFile1 = CreateFileMapping(
            INVALID_HANDLE_VALUE,    // use paging file
            NULL,                    // default security
            PAGE_READWRITE,          // read/write access
            0,                       // maximum object size (high-order DWORD)
            sz,                      // maximum object size (low-order DWORD)
            buf1.c_str());           // name of mapping object
        if (hMapFile1 == NULL)
        {
            return NULL;
        }
        hMapFile2 = CreateFileMapping(
            INVALID_HANDLE_VALUE,    // use paging file
            NULL,                    // default security
            PAGE_READWRITE,          // read/write access
            0,                       // maximum object size (high-order DWORD)
            sz,                      // maximum object size (low-order DWORD)
            buf2.c_str());          // name of mapping object
        if (hMapFile2 == NULL)
        {
            CloseHandle(hMapFile1);
            return NULL;
        }
        hStatusMap = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            sizeof(BufferStatus),
            L"BufferStatus");

        if (hStatusMap == NULL)
        {
            CloseHandle(hMapFile1);
            CloseHandle(hMapFile2);
            return NULL;
        }

        // Map the status buffer
        pStatus = (BufferStatus*)MapViewOfFile(hStatusMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(BufferStatus));
        if (pStatus == NULL)
        {
            CloseHandle(hMapFile1);
            CloseHandle(hMapFile2);
            CloseHandle(hStatusMap);
            return NULL;
        }

        // Initialize status
        memset(pStatus, 0, sizeof(BufferStatus));

        // Get pointers to the buffers
        pBuf1 = MapViewOfFile(hMapFile1, FILE_MAP_ALL_ACCESS, 0, 0, sz);
        pBuf2 = MapViewOfFile(hMapFile2, FILE_MAP_ALL_ACCESS, 0, 0, sz);

        if (pBuf1 == NULL || pBuf2 == NULL)
        {
            if (pBuf1) UnmapViewOfFile(pBuf1);
            if (pBuf2) UnmapViewOfFile(pBuf2);
            UnmapViewOfFile(pStatus);
            CloseHandle(hMapFile1);
            CloseHandle(hMapFile2);
            CloseHandle(hStatusMap);
            return NULL;
        }

        // Clear the buffers initially
        memset(pBuf1, 0, sz);
        memset(pBuf2, 0, sz);

        return hMapFile1;
    }

    // Add string data to the specified buffer
    void AddData(LPVOID buffer, const std::string& data)
    {
        if (!buffer || data.empty() || data.size() > m_bufferSize)
            return;

        // Copy the string data to buffer, including null terminator
        memcpy(buffer, data.c_str(), data.size() + 1);

        // Update the status with the correct size
        if (buffer == pBuf1 && pStatus)
        {
            pStatus->dataSize = data.size() + 1;  // Include null terminator
            pStatus->SetBuffer1Ready(true);
        }
        else if (buffer == pBuf2 && pStatus)
        {
            pStatus->dataSize = data.size() + 1;  // Include null terminator
            pStatus->SetBuffer2Ready(true);
        }
    }

    // Add DataStream data to the specified buffer
    void AddData(LPVOID buffer, const DataStream* dataStream)
    {
        if (!buffer || !dataStream || !dataStream->hasData())
            return;

        std::string data = dataStream->GetData();
        if (data.size() > m_bufferSize)
            return;

        // Copy the string data to buffer
        memcpy(buffer, data.c_str(), data.size() + 1);

        // Update the status with the correct size
        if (buffer == pBuf1 && pStatus)
        {
            pStatus->dataSize = data.size() + 1;  // Include null terminator
            pStatus->SetBuffer1Ready(true);
        }
        else if (buffer == pBuf2 && pStatus)
        {
            pStatus->dataSize = data.size() + 1;  // Include null terminator
            pStatus->SetBuffer2Ready(true);
        }
    }

    // Generic template for user-defined data types
    template<typename T>
    void AddData(LPVOID buffer, const T& userData)
    {
        if (!buffer || sizeof(T) > m_bufferSize)
            return;

        // Copy the user data to buffer
        memcpy(buffer, &userData, sizeof(T));

        // Update the status with the correct size
        if (buffer == pBuf1 && pStatus)
        {
            pStatus->dataSize = sizeof(T);
            pStatus->SetBuffer1Ready(true);
        }
        else if (buffer == pBuf2 && pStatus)
        {
            pStatus->dataSize = sizeof(T);
            pStatus->SetBuffer2Ready(true);
        }
    }

    // Clear data in the specified buffer
    void ClearData(LPVOID buffer)
    {
        if (!buffer)
            return;

        // Clear the buffer by zeroing it
        memset(buffer, 0, m_bufferSize);

        // Update status
        if (buffer == pBuf1 && pStatus)
        {
            pStatus->dataSize = 0;
            pStatus->SetBuffer1Ready(false);
        }
        else if (buffer == pBuf2 && pStatus)
        {
            pStatus->dataSize = 0;
            pStatus->SetBuffer2Ready(false);
        }
    }

    // Clear both buffers
    void ClearData()
    {
        ClearData(pBuf1);
        ClearData(pBuf2);
        m_data.clear();
    }

    // Get string data from the specified buffer
    std::string GetDataString(LPVOID buffer) const
    {
        if (!buffer || !pStatus)
            return "";

        size_t size = (buffer == pBuf1) ?
            (pStatus->IsBuffer1Ready() ? pStatus->dataSize : 0) :
            (pStatus->IsBuffer2Ready() ? pStatus->dataSize : 0);

        if (size == 0)
            return "";

        // Assuming the buffer contains a null-terminated string
        return std::string(static_cast<const char*>(buffer));
    }

    // Get typed data from the specified buffer
    template<typename T>
    T GetData(LPVOID buffer) const
    {
        T result{};
        if (!buffer || !pStatus)
            return result;

        size_t size = (buffer == pBuf1) ?
            (pStatus->IsBuffer1Ready() ? pStatus->dataSize : 0) :
            (pStatus->IsBuffer2Ready() ? pStatus->dataSize : 0);

        if (size == 0 || size < sizeof(T))
            return result;

        // Copy the data from the buffer to result
        memcpy(&result, buffer, sizeof(T));
        return result;
    }

    void ProcessDataStream(DataStream* stream)
    {
        bool useBuffer1 = true;
        while (stream->hasData()) {
            SYSTEMTIME systemTime;
            GetSystemTime(&systemTime);

            // Determine which buffer to write to
            LPVOID activeBuffer = useBuffer1 ? pBuf1 : pBuf2;

            // Clear the buffer before adding new data
            ClearData(activeBuffer);

            // Fill the active buffer with new data
            AddData(activeBuffer, stream);

            // Update status to tell WebView2 which buffer to read
            pStatus->activeBufferIndex = useBuffer1 ? 0 : 1;
            pStatus->dataTimestamp = systemTime;
            pStatus->newDataAvailable = true;

            // Switch buffers for next iteration
            useBuffer1 = !useBuffer1;

            // Wait for next data collection cycle (maintain 1kHz rate)
            Sleep(1);
        }
    }

    // Accessors to get buffer pointers
    LPVOID GetBuffer1() const { return pBuf1; }
    LPVOID GetBuffer2() const { return pBuf2; }
    BufferStatus* GetStatus() const { return pStatus; }

private:
    std::string m_data;
    HANDLE hMapFile1 = NULL;
    HANDLE hMapFile2 = NULL;
    HANDLE hStatusMap = NULL;
    BufferStatus* pStatus = nullptr;
    LPVOID pBuf1 = nullptr;
    LPVOID pBuf2 = nullptr;
    size_t m_bufferSize = 0;
};