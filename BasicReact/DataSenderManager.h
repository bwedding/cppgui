#pragma once

#include <Windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <map>
#include <mutex>
#include "WebViewManager.h" // This includes CPPGUI::UIEvent

class DataSenderManager
{
private:
    std::map<std::string, std::thread> m_senderThreads;
    std::map<std::string, std::atomic<bool>> m_threadRunFlags;
    std::mutex m_threadMutex;
    HWND m_targetWindow;

public:
    DataSenderManager(HWND targetWindow);
    ~DataSenderManager();

    // Event handler for data sender controls
    std::string HandleDataSenderEvent(const CPPGUI::UIEvent& evt);

    // Start/stop methods
    bool StartSender(const std::string& senderType);
    bool StopSender(const std::string& senderType);
    bool IsSenderRunning(const std::string& senderType);
    void StopAllSenders();

    // Thread functions for different sender types
    void StringSenderThreadFunc();
    void JsonSenderThreadFunc();
    static void NativeObjectSenderThreadFunc(HWND targetWindow, std::atomic<bool>* runFlag);
    static void SharedBufferSenderThreadFunc(HWND targetWindow, std::atomic<bool>* runFlag);
};
