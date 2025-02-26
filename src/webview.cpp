#include <windows.h>
#include <WebView.h>
#include <pathcch.h>
#include <psapi.h>
#include "WindowApp.h"
#include "APIBridge.h"
#include "CommunicationManager.h"
#include <WebView2EnvironmentOptions.h>
#include <wrl/event.h>

#include "UIEvent.h"

using namespace Microsoft::WRL;
                                   
extern HANDLE g_webviewReadyEvent;

std::wstring WebView2Manager::GetUIPath()
{
    SPDLOG_TRACE("Entering");

    std::wstring exePath(MAX_PATH, L'\0');
    GetCurrentDirectory(MAX_PATH, exePath.data());
    std::wstring localFolder = exePath;

    PathCchAppend(localFolder.data(), MAX_PATH, L"Frontend\\UI\\dist2\\");

    // Clean up any null padding. This IS necessary!
    localFolder.erase(std::ranges::find(localFolder, L'\0'), localFolder.end());

    return localFolder;
}

constexpr long MEMORY_THRESHOLD = 0x1FFFFFFFFL;  // 512Mb

// Add periodic memory monitoring
void WebView2Manager::CheckMemoryUsage() const
{
    SPDLOG_TRACE("Entering");

    UINT32 browserPid;
    if (SUCCEEDED(m_webview->get_BrowserProcessId(&browserPid)))
    {
	    const HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, browserPid);
        if (hProcess != nullptr)
        {
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
            {
	            // Memory usage in bytes
                if (const SIZE_T memoryUsage = pmc.WorkingSetSize; memoryUsage > MEMORY_THRESHOLD)
                {
                    // Take action - perhaps reload or suspend WebView
                }
            }
            CloseHandle(hProcess);
        }
    }
}

void WebView2Manager::NavigateToPage(const std::wstring& page) const
{
    SPDLOG_TRACE("Entering");

    if (!m_webview)
        return;

    if (LocalFolder.empty())
    {
        //spdlog::error("LocalFolder is empty");
        return;
    }

    // Basic path sanitization - prevent directory traversal
    if (page.find(L"..") != std::wstring::npos)
    {
        //spdlog::error("Invalid page path contains '..'");
        return;
    }

    std::wstring destination = LocalFolder + page;

    if (destination.length() >= MAX_PATH)
    {
        //spdlog::error("Path exceeds MAX_PATH");
        return;
    }

    const DWORD attrs = GetFileAttributes(destination.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        DWORD error = GetLastError();
        // Convert wstring to string for spdlog
        std::string narrow_dest(destination.begin(), destination.end());
        // Create a 404 page with the attempted path
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
            L"<p class='error-path'>Rebuild the front end UI and/or check your paths. " + L"</p>"
            L"</body></html>";

        m_webview->NavigateToString(html404.c_str());
        //spdlog::error("Failed to find page: {} Error: {}", narrow_dest, error);
        return;
    }

    if (!(attrs & FILE_ATTRIBUTE_NORMAL) && (attrs & FILE_ATTRIBUTE_DIRECTORY))
    {
        std::string narrow_dest(destination.begin(), destination.end());
        //spdlog::error("Path is a directory, not a file: {}", narrow_dest);
        return;
    }

    m_webview->Navigate(destination.c_str());
}

WebView2Manager& WebView2Manager::GetInstance() 
{
    //SPDLOG_TRACE("Entering");

    static WebView2Manager instance;
    return instance;
}

HRESULT WebView2Manager::Initialize(HWND hWnd)
{
    SPDLOG_TRACE("Entering");

    m_uiThreadId = GetCurrentThreadId();
    m_hWnd = hWnd;

    const ComPtr<ICoreWebView2EnvironmentOptions> options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
    options->put_AdditionalBrowserArguments(L"--disable-web-security --allow-file-access-from-files --allow-file-access --hide-scrollbars");

    return CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, options.Get(),
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [hWnd, this](HRESULT, ICoreWebView2Environment* env) -> HRESULT
            {
                m_webviewEnvironment = env;
                return CreateWebViewController(hWnd, env);
            }).Get());
}

HRESULT WebView2Manager::CreateWebViewController(HWND hWnd, ICoreWebView2Environment* env)
{
    SPDLOG_TRACE("Entering");

    auto p = m_webviewEnvironment;

    return env->CreateCoreWebView2Controller(hWnd,
        Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [hWnd, this](HRESULT, ICoreWebView2Controller* controller) -> HRESULT
            {
                if (controller != nullptr)
                {
                    m_webviewController = controller;
                    m_webviewController->get_CoreWebView2(m_webview.GetAddressOf());
                    // make background black
                    ICoreWebView2Controller2* controller2;
                    if (SUCCEEDED(controller->QueryInterface(&controller2))) {
	                    constexpr COREWEBVIEW2_COLOR color = { 255, 0, 0, 0 }; 
                        controller2->put_DefaultBackgroundColor(color);
                        controller2->Release();
                    }
                    return ConfigureWebView(hWnd);
                }
                return E_FAIL;
            }).Get());
}

HRESULT WebView2Manager::ConfigureWebView(const HWND hWnd)
{
    //SPDLOG_TRACE("Entering");

    ConfigureWebViewSettings();

    RECT bounds;
    GetClientRect(hWnd, &bounds);
    constexpr int RESIZE_BORDER = 1;

    // Inset the bounds by the resize border width
    bounds.left += RESIZE_BORDER;
    bounds.top += RESIZE_BORDER;
    bounds.right -= RESIZE_BORDER;
    bounds.bottom -= RESIZE_BORDER;
    m_webviewController->put_Bounds(bounds);

    SetupNavigationHandlers();
    SetupMessageHandlers();

    // Always start at home
    const std::wstring destination = L"index.html";
    NavigateToPage(destination);

    return S_OK;
}

void WebView2Manager::ConfigureWebViewSettings()
{
    //SPDLOG_TRACE("Entering");

    wil::com_ptr<ICoreWebView2Settings> settings;
    m_webview->get_Settings(&settings);
    settings->put_IsScriptEnabled(true);
    // Note: This should be disabled in production since we don't want JS Alerts popping up all the time
    settings->put_AreDefaultScriptDialogsEnabled(true);
    settings->put_IsWebMessageEnabled(true);
    settings->put_AreDevToolsEnabled(true);
    settings->put_IsStatusBarEnabled(false);
    settings->put_AreHostObjectsAllowed(true);  // Required for native interop
    settings->put_IsZoomControlEnabled(true);  // Might want to disable this in the future. For now I'll leave it to handle different monitor resolutions
    auto nativeCtrls = Microsoft::WRL::Make<NativeWindowControls>(m_hWnd);

    // Create VARIANT to hold the COM object
    VARIANT var = {};
    var.vt = VT_DISPATCH;
    HRESULT hr = nativeCtrls.Get()->QueryInterface(IID_IDispatch, (void**)&var.pdispVal);
    if (FAILED(hr)) {
        // Handle error
        return;
    }
    // Add it as a host object
    m_webview->AddHostObjectToScript(L"native", &var);
    ComPtr<ApiBridge> apiBridge = Make<ApiBridge>(GetInstance().GetWebView());

    // Wrap the COM object in a VARIANT
    VARIANT variant;
    VariantInit(&variant);
    variant.vt = VT_DISPATCH; // Set the type to IDispatch
    variant.pdispVal = apiBridge.Get(); // Assign the COM object
    m_webview->AddHostObjectToScript(L"bridge", &variant);
}

void WebView2Manager::SetupNavigationHandlers() const
{
    //SPDLOG_TRACE("Entering");

    EventRegistrationToken token;

    m_webview->add_NavigationCompleted(
        Callback<ICoreWebView2NavigationCompletedEventHandler>(
            [](ICoreWebView2*, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                BOOL success;
                args->get_IsSuccess(&success);
                if (!success) {
                    COREWEBVIEW2_WEB_ERROR_STATUS webErrorStatus;
                    args->get_WebErrorStatus(&webErrorStatus);
                    //spdlog::error("Navigation failed with error status: {}", (int)webErrorStatus);
                }
                return S_OK;
            }).Get(),
                &token);

    // Handle process failures
    m_webview->add_ProcessFailed(
        Callback<ICoreWebView2ProcessFailedEventHandler>(
            [this](ICoreWebView2*, ICoreWebView2ProcessFailedEventArgs* args) -> HRESULT
            {
                COREWEBVIEW2_PROCESS_FAILED_KIND kind;
                args->get_ProcessFailedKind(&kind);
                // TODO: Capture the process failure here and restart the webview
                // 
                // Handle process failure based on kind
                // You might want to reload the webview or notify the user
                return S_OK;
            }).Get(),
                nullptr);

}

std::string WideToUtf8(const wchar_t* wide) 
{
    //SPDLOG_TRACE("Entering");

    if (!wide) return "";
    const int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
    std::string utf8(size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8.data(), size, nullptr, nullptr);
    return utf8;
}

void WebView2Manager::SetupMessageHandlers() const
{
    //SPDLOG_TRACE("Entering");

    EventRegistrationToken token;
    m_webview->add_WebMessageReceived(
        Callback<ICoreWebView2WebMessageReceivedEventHandler>(
            [this](ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                wil::unique_cotaskmem_string message;
                args->TryGetWebMessageAsString(&message);
                spdlog::debug("Message from React app: {0}", WideToUtf8(message.get()));

                CommunicationManager::Instance().HandleWebMessage(args);
                try {
                    auto jsonData = json::parse(WideToUtf8(message.get()));

                    // Create an event from the web message
                    HeartControl::UIEvent event{
                        jsonData.contains("type") ? jsonData["type"].get<std::string>() : "web-message",
                        "WebView",
                        message.get(),  // Store the entire message as payload
                        std::chrono::system_clock::now()
                    };

                    // Queue the event - use the reference to avoid copying
                    auto result = WindowApp::TriggerEvent(std::move(event));

                    //auto& eventQueue = WindowApp::GetInstance()->GetEventQueue();
                    //eventQueue.enqueue(std::move(event));

                }
                catch (const std::exception& e) {
                    spdlog::error("Error processing web message: {}", e.what());
                }
                
                return S_OK;
            }).Get(),
                &token);
}

// Example usages:
// webview2Manager.ExecuteScript(L"alert('Hello from C++!');");
// Or run more complex JavaScript:
// webview2Manager.ExecuteScript(L"document.body.style.backgroundColor = 'red';");
// You can even evaluate expressions and get results:
// webview2Manager.ExecuteScript(L"1 + 1"); // Will output "Script result: 2" to debug output
// Or interact with your React app:
// webview2Manager.ExecuteScript(L"window.myReactFunction && window.myReactFunction('data from C++');");
HRESULT WebView2Manager::ExecuteScript(const std::wstring& script) const
{
    SPDLOG_TRACE("Entering");

    if (GetCurrentThreadId() == m_uiThreadId)
    {
        if (!m_webview)
        {
            return E_POINTER;
        }

        return m_webview->ExecuteScript(script.c_str(),
            Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
                [](const HRESULT error, const LPCWSTR result) -> HRESULT {
                    if (FAILED(error))
                    {
                        spdlog::error("Failed to execute script");

                        return error;
                    }
                    if (result != nullptr)
                    {
                        std::wstring resultStr(L"Script result: ");
                        resultStr += result;
                        spdlog::info(L"Script result : {0}", resultStr);
                    }
                    return S_OK;
                }).Get());
    }
    // Post to UI thread
    PostMessage(m_hWnd, WM_EXECUTE_SCRIPT_WEBVIEW, reinterpret_cast<WPARAM>(new std::wstring(script)), 0);
    return S_OK;
}

HRESULT WebView2Manager::PostMessageToWebView(const std::wstring& message) const
{
    //SPDLOG_TRACE("Entering");

    // Check if we're already on the UI thread
    if (GetCurrentThreadId() == m_uiThreadId)  
    {
        if (!m_webview)
        {
            return E_POINTER;
        }
        try
        {
            return m_webview->PostWebMessageAsString(message.c_str());
        }
        catch (const std::exception& e)
        {
            spdlog::error("Error posting message: {}", std::string(e.what()));
            return E_FAIL;
        }
    }
    // Post to UI thread and wait for completion
    PostMessage(m_hWnd, WM_POST_MSG_TO_WEBVIEW, reinterpret_cast<WPARAM>(new std::wstring(message)), 0);
    return S_OK;  // Note: This becomes async when called from another thread
}

void WebView2Manager::Close()
{
    SPDLOG_TRACE("Entering");

    //if (m_webview)
    //{
    //    m_webview->Close();
    //    m_webview = nullptr;
    //}
    //m_webviewEnvironment = nullptr;

}

// Close the WebView and de-initialize related state. This doesn't close the app window.
bool WebView2Manager::CloseWebView(bool cleanupUserDataFolder)
{
    SPDLOG_TRACE("Entering");

    if (m_webviewController)
    {
        m_webviewController->Close();
        m_webviewController = nullptr;
        m_webview = nullptr;
    }

   m_webviewEnvironment = nullptr;

    // reset profile name
    return true;
}

HRESULT WebView2Manager::Reload() const
{
    SPDLOG_TRACE("Entering");

    //spdlog::info("WebView2Manager::Reload()\n");
    if (!m_webview) 
    {
        return E_POINTER;
    }

    if (GetCurrentThreadId() != m_uiThreadId) 
    {
        return E_FAIL;
    }

    try {
        // Clear browser cache
        ComPtr<ICoreWebView2_3> webview2_3;
        HRESULT hr = m_webview.As(&webview2_3);
        if (SUCCEEDED(hr)) {
            hr = webview2_3->CallDevToolsProtocolMethod(
                L"Network.clearBrowserCache",
                L"{}",
                nullptr);
            if (FAILED(hr)) 
            {
                return hr;
            }
        }
        return m_webview->Reload();
    }
    catch (const winrt::hresult_error& ex) {
        return ex.code();
    }
}



