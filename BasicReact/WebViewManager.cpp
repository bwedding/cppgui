#include "WebViewManager.h"
#include <plog/Log.h>
#include "SystemUtils.h"
#include <WebView2EnvironmentOptions.h>
#include <wrl/event.h>

WebViewManager::WebViewManager(HWND hWnd, HINSTANCE hInstance) 
    : m_hWnd(hWnd), m_hInstance(hInstance), m_uiThreadId(GetCurrentThreadId()) {}

void WebViewManager::Initialize() {
    InitializeWebView();
}

void WebViewManager::InitializeWebView() {
    auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
    options->put_AdditionalBrowserArguments(L"--allow-file-access-from-files --hide-scrollbars --msWebView2EnableDraggableRegions");

    CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, options.Get(),
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                env->CreateCoreWebView2Controller(m_hWnd,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                            if (controller) {
                                m_controller = controller;
                                m_controller->get_CoreWebView2(&m_webview);
                                
                                InitializeNativeControls();

                                // Now start processing
                                if (m_nativeControls) {
                                    m_nativeControls->StartEventProcessing();
                                }

                                wil::com_ptr<ICoreWebView2Settings> settings;
                                m_webview->get_Settings(&settings);
                                settings->put_IsScriptEnabled(TRUE);
                                settings->put_AreDevToolsEnabled(TRUE);
                                settings->put_AreHostObjectsAllowed(TRUE);

                                m_webview->add_NavigationStarting(
                                    Callback<ICoreWebView2NavigationStartingEventHandler>(
                                        [this](ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args) {
                                            wil::unique_cotaskmem_string uri;
                                            args->get_Uri(&uri);
                                            LOGI << L"Navigating to: " << uri.get();
                                            return S_OK;
                                        }).Get(), &m_navigationToken);

                                m_webview->add_WebMessageReceived(
                                    Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                                        [this](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) {
                                            wil::unique_cotaskmem_string message;
                                            args->TryGetWebMessageAsString(&message);
                                            
                                            // Parse event type from JSON
                                            try {
                                                auto msg = message.get();
                                                // Convert wide string to UTF-8
                                                std::string utf8Msg = SystemUtils::wchar_to_UTF8(msg);
                                                auto json = nlohmann::json::parse(utf8Msg);
                                                std::string eventType = json["type"];
                                                m_nativeControls->HandleWebViewEvent(eventType, json.dump());
                                            } catch (const std::exception& e) {
                                                LOGW << "Failed to parse WebView message: " << e.what();
                                            }
                                            return S_OK;
                                        }).Get(), &m_messageToken);

                                RECT bounds;
                                GetClientRect(m_hWnd, &bounds);
                                Resize(bounds);
                                m_webview->Navigate(L"file:///C:/Users/bruce/source/cppgui/Frontend/UI/dist/index.html");
                            }
                            return S_OK;
                        }).Get());
                return S_OK;
            }).Get());
}

void WebViewManager::InitializeNativeControls() {
    m_nativeControls = Microsoft::WRL::Make<NativeWindowControls>(m_hWnd);
    
    VARIANT var = {};
    var.vt = VT_DISPATCH;
    if (SUCCEEDED(m_nativeControls->QueryInterface(IID_IDispatch, reinterpret_cast<void**>(&var.pdispVal)))) {
        m_webview->AddHostObjectToScript(L"native", &var);
        VariantClear(&var);
    }

    // Example of subscribing to an event - now using the EventManager directly
    if (auto eventManager = m_nativeControls->GetEventManager()) {
        eventManager->subscribe("auto-manual-control", [](const CPPGUI::UIEvent& evt) {
            LOGI << "Handling control event: " << evt.type;
            return "success";
        });
    }
}

void WebViewManager::Resize(const RECT& bounds) {
    if (m_controller) {
        m_controller->put_Bounds(bounds);
    }
}

HRESULT WebViewManager::ExecuteScript(const std::wstring& script) {
    if (!m_webview) return E_POINTER;
    return m_webview->ExecuteScript(script.c_str(), nullptr);
}

HRESULT WebViewManager::PostMessageToWebView(const std::wstring& message) {
    if (!m_webview) return E_POINTER;
    return m_webview->PostWebMessageAsString(message.c_str());
}
