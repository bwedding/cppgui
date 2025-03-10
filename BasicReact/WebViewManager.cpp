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
    // --enable-features=SkiaGraphite,VulkanImplementation --use-vulkan=native 
    options->put_AdditionalBrowserArguments(L"--allow-file-access-from-files --enable-gpu-rasterization --hide-scrollbars --enable-javascript-virtual-host-mapping-bytecode-caching --msWebView2NativeEventDispatch --msWebView2EnableDraggableRegions");

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

                                // Configure settings
                                wil::com_ptr<ICoreWebView2Settings> settings;
                                m_webview->get_Settings(&settings);
                                settings->put_AreHostObjectsAllowed(TRUE);
                                settings->put_IsScriptEnabled(TRUE);
                                settings->put_AreDefaultScriptDialogsEnabled(TRUE);
                                settings->put_IsWebMessageEnabled(TRUE);
                                settings->put_AreDevToolsEnabled(m_devToolsEnabled);

                                settings->put_AreDefaultContextMenusEnabled(m_defaultContextMenusEnabled);
                                settings->put_IsBuiltInErrorPageEnabled(m_builtInErrorPageEnabled);
                                settings->put_IsStatusBarEnabled(m_statusBarEnabled);
                                settings->put_IsZoomControlEnabled(m_zoomControlEnabled);   

                                // Register event handlers
                                m_webview->add_NavigationStarting(
                                    Callback<ICoreWebView2NavigationStartingEventHandler>(
                                        [this](ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args) {
                                            LOGD << "Navigation starting";
                                            return S_OK;
                                        }).Get(), &m_navigationToken);

                                // Add NavigationCompleted event handler
                                m_webview->add_NavigationCompleted(
                                    Callback<ICoreWebView2NavigationCompletedEventHandler>(
                                        [this](ICoreWebView2* webview, ICoreWebView2NavigationCompletedEventArgs* args) {
                                            BOOL success;
                                            args->get_IsSuccess(&success);
                                            
                                            COREWEBVIEW2_WEB_ERROR_STATUS errorStatus;
                                            args->get_WebErrorStatus(&errorStatus);
                                            
                                            wil::unique_cotaskmem_string uri;
                                            webview->get_Source(&uri);
                                            
                                            std::wstring uriStr = uri.get();
                                            LOGD << "Navigation completed to: " << SystemUtils::WideToUtf8(uriStr) 
                                                 << " Success: " << (success ? "true" : "false");
                                            
                                            if (!success) {
                                                LOGE << "Navigation error: " << errorStatus;
                                            }
                                            
                                            // Call the user-provided callback if set
                                            if (m_navigationCompletedCallback) {
                                                m_navigationCompletedCallback(uriStr, success == TRUE, errorStatus);
                                            }
                                            
                                            // Call the simplified callback if set
                                            if (m_simpleNavigationCallback) {
                                                std::wstring errorMessage;
                                                if (!success) {
                                                    errorMessage = GetUserFriendlyErrorMessage(errorStatus);
                                                }
                                                m_simpleNavigationCallback(uriStr, success == TRUE, errorMessage);
                                            }
                                            
                                            return S_OK;
                                        }).Get(), &m_navigationCompletedToken);

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
                                m_webview->Navigate(m_navigationURL.c_str());
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

void WebViewManager::Resize(const RECT& bounds) 
{
    if (m_controller) 
    {
        m_controller->put_Bounds(bounds);
    }
}

HRESULT WebViewManager::ExecuteScript(const std::wstring& script) {
    if (!m_webview) return E_POINTER;
    return m_webview->ExecuteScript(script.c_str(), nullptr);
}

HRESULT WebViewManager::PostMessageToWebView(const std::wstring& message) {
    if (!m_webview)
        return E_FAIL;
    
    return m_webview->PostWebMessageAsString(message.c_str());
}

HRESULT WebViewManager::PostJSONMessageToWebView(const std::wstring& message) {
    if (!m_webview)
        return E_FAIL;

    return m_webview->PostWebMessageAsJson(message.c_str());
}

void WebViewManager::NavigateToPage(const std::wstring& page) 
{
    LOGD << "Navigating to page: " << SystemUtils::WideToUtf8(page);

    if (!m_webview)
        return;

    if (m_localFolder.empty()) {
        LOGE << "LocalFolder is empty";
        return;
    }

    // Basic path sanitization - prevent directory traversal
    if (page.find(L"..") != std::wstring::npos) {
        LOGE << "Invalid page path contains '..'";
        return;
    }

    std::wstring destination = m_localFolder + page;

    if (destination.length() >= MAX_PATH) {
        LOGE << "Path exceeds MAX_PATH";
        return;
    }

    const DWORD attrs = GetFileAttributes(destination.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        
        // If no custom 404 page is set, create a default one
        if (m_html404.empty()) {
            const std::wstring html404 = L"<html><head><title>404 - Page Not Found</title>"
                L"<style>"
                L"body { font-family: Arial, sans-serif; text-align: center; padding-top: 50px; }"
                L"h1 { color: #333; }"
                L".error-code { font-size: 72px; color: #666; margin: 20px 0; }"
                L".error-path { color: #999; word-break: break-all; padding: 0 20px; }"
                L"</style></head>"
                L"<body>"
                L"<div class='error-code'>404</div>"
                L"<h1>Page Not Found</h1>"
                L"<p class='error-path'>The requested page cannot be found: " + page + L"</p>"
                L"<p class='error-path'>Rebuild the front end UI and/or check your paths.</p>"
                L"</body></html>";
            
            m_webview->NavigateToString(html404.c_str());
        } else {
            // Use the custom 404 page
            std::wstring customized404 = m_html404;
            // Replace any %PAGE% placeholder with the actual page path
            size_t pos = customized404.find(L"%PAGE%");
            if (pos != std::wstring::npos) {
                customized404.replace(pos, 6, page);
            }
            m_webview->NavigateToString(customized404.c_str());
        }

        LOGE << "Failed to find page: " << SystemUtils::WideToUtf8(destination) << " Error: " << error;
        return;
    }

    if (!(attrs & FILE_ATTRIBUTE_NORMAL) && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        LOGE << "Path is a directory, not a file: " << SystemUtils::WideToUtf8(destination);
        return;
    }

    m_webview->Navigate(destination.c_str());
}

std::wstring WebViewManager::GetUserFriendlyErrorMessage(COREWEBVIEW2_WEB_ERROR_STATUS errorStatus)
{
    switch (errorStatus) {
    case COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED:
    case COREWEBVIEW2_WEB_ERROR_STATUS_CONNECTION_ABORTED:
    case COREWEBVIEW2_WEB_ERROR_STATUS_CONNECTION_RESET:
    case COREWEBVIEW2_WEB_ERROR_STATUS_SERVER_UNREACHABLE:
    case COREWEBVIEW2_WEB_ERROR_STATUS_CANNOT_CONNECT:
        return L"Network connection error. Please check your internet connection and try again.";
        
    case COREWEBVIEW2_WEB_ERROR_STATUS_HOST_NAME_NOT_RESOLVED:
        return L"Could not find the website. Please check the address and try again.";
        
    case COREWEBVIEW2_WEB_ERROR_STATUS_TIMEOUT:
        return L"The connection timed out. Please try again later.";
        
    case COREWEBVIEW2_WEB_ERROR_STATUS_ERROR_HTTP_INVALID_SERVER_RESPONSE:
        return L"The server returned an invalid response. Please try again later.";
        
    case COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_COMMON_NAME_IS_INCORRECT:
    case COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_EXPIRED:
    case COREWEBVIEW2_WEB_ERROR_STATUS_CLIENT_CERTIFICATE_CONTAINS_ERRORS:
    case COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_REVOKED:
    case COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_IS_INVALID:
        return L"There is a problem with the website's security certificate. This might not be a secure connection.";
        
    case COREWEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED:
        return L"The operation was canceled.";
        
    case COREWEBVIEW2_WEB_ERROR_STATUS_REDIRECT_FAILED:
        return L"The website redirect failed. Please try again.";
        
    case COREWEBVIEW2_WEB_ERROR_STATUS_UNEXPECTED_ERROR:
        return L"An unexpected error occurred. Please try again later.";
        
    case COREWEBVIEW2_WEB_ERROR_STATUS_VALID_AUTHENTICATION_CREDENTIALS_REQUIRED:
    case COREWEBVIEW2_WEB_ERROR_STATUS_VALID_PROXY_AUTHENTICATION_REQUIRED:
        return L"Authentication required. Please sign in to continue.";
        
    default:
        return L"Navigation error occurred. Please try again later.";
    }
}
