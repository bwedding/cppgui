#pragma once
#include <WebView2.h>
#include <wrl/client.h>
#include "NativeWindowControls.h"
#include <functional>
#include "EventDispatcher.h"
#include <unordered_map>
#include <memory>
#include <wrl.h>
#include "../packages/Microsoft.Windows.ImplementationLibrary.1.0.240803.1/include/wil/com.h"
#include "../packages/Microsoft.Web.WebView2.1.0.3065.39/build/native/include/WebView2.h"
#include "../packages/Microsoft.Web.WebView2.1.0.3065.39/build/native/include/WebView2EnvironmentOptions.h"

// Define a custom timer ID for subscription retries
#define TIMER_ID_SUBSCRIPTION_RETRY 1001

class WebViewManager {
public:
    WebViewManager(HWND hWnd, HINSTANCE hInstance);
    void Initialize();
    HRESULT ExecuteScript(const std::wstring& script);
    HRESULT PostMessageToWebView(const std::wstring& message);
    void Resize(const RECT& bounds);
    
    ICoreWebView2* GetWebView() const { return m_webview.get(); }
    NativeWindowControls* GetNativeControls() const { return m_nativeControls.get(); }
    
    // Structure to hold pending subscriptions
    struct PendingSubscription {
        std::string eventType;
        std::function<void(const CPPGUI::UIEvent&)> callback;
    };
    
    void Subscribe(const std::string& eventType, std::function<std::string(const CPPGUI::UIEvent&)> callback) {
        // Create a wrapper function that returns the result of the callback
        auto wrappedCallback = [callback](const CPPGUI::UIEvent& evt) {
            callback(evt);
        };
        
        // If NativeWindowControls is initialized, subscribe directly through its EventManager
        if (m_nativeControls) {
            auto eventManager = m_nativeControls->GetEventManager();
            if (eventManager) {
                eventManager->subscribe(eventType, std::move(wrappedCallback));
                return;
            }
        }
        
        // If we get here, NativeWindowControls isn't ready yet
        // Store the subscription request for later retry
        m_pendingSubscriptions.push_back(std::make_unique<PendingSubscription>(
            PendingSubscription{eventType, std::move(wrappedCallback)}
        ));
        
        // Start the retry timer if it's not already running
        if (!m_subscriptionTimerActive) {
            SetTimer(m_hWnd, TIMER_ID_SUBSCRIPTION_RETRY, 100, nullptr);
            m_subscriptionTimerActive = true;
        }
    }
    
    // Process pending subscriptions - called from WM_TIMER handler
    void ProcessPendingSubscriptions() {
        if (m_pendingSubscriptions.empty()) {
            // No more pending subscriptions, stop the timer
            KillTimer(m_hWnd, TIMER_ID_SUBSCRIPTION_RETRY);
            m_subscriptionTimerActive = false;
            return;
        }
        
        if (!m_nativeControls) {
            // NativeWindowControls not ready yet, keep the timer running
            return;
        }
        
        auto eventManager = m_nativeControls->GetEventManager();
        if (!eventManager) {
            // EventManager not ready yet, keep the timer running
            return;
        }
        
        // Process all pending subscriptions
        auto it = m_pendingSubscriptions.begin();
        while (it != m_pendingSubscriptions.end()) {
            eventManager->subscribe((*it)->eventType, std::move((*it)->callback));
            it = m_pendingSubscriptions.erase(it);
        }
        
        // All subscriptions processed, stop the timer
        KillTimer(m_hWnd, TIMER_ID_SUBSCRIPTION_RETRY);
        m_subscriptionTimerActive = false;
    }

private:
    HWND m_hWnd;
    HINSTANCE m_hInstance;
    wil::com_ptr<ICoreWebView2Controller> m_controller;
    wil::com_ptr<ICoreWebView2> m_webview;
    wil::com_ptr<NativeWindowControls> m_nativeControls;
    DWORD m_uiThreadId;
    EventRegistrationToken m_navigationToken;
    EventRegistrationToken m_messageToken;
    
    // Storage for pending subscriptions
    std::vector<std::unique_ptr<PendingSubscription>> m_pendingSubscriptions;
    bool m_subscriptionTimerActive = false;

    void InitializeWebView();
    void InitializeNativeControls();
};
