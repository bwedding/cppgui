#pragma once
#include <windows.h>
#include "WebView2.h"
#include "nlohmann/json.hpp"
#pragma comment(lib, "wbemuuid.lib")
#include <winrt/base.h>
#include "NativeWindowControls.h"

using namespace Microsoft::WRL;
using json = nlohmann::json;

// Forward declarations
struct ICoreWebView2;
struct ICoreWebView2Controller;
struct ICoreWebView2Environment;
class  NativeWindowControls;

class WebView2Manager
{
    DWORD m_uiThreadId;

public:

    std::wstring LocalFolder;
    WebView2Manager() : m_uiThreadId(0), LocalFolder(GetUIPath())
    {
    }

    // Static methods as there's only 1 Webview Manager
    static WebView2Manager& GetInstance();

    void CheckMemoryUsage() const;

    // Initialize and setup methods
    HRESULT Initialize(HWND hWnd);
    [[nodiscard]] HRESULT PostMessageToWebView(const std::wstring& message) const;
    [[nodiscard]] HRESULT ExecuteScript(const std::wstring& script) const;

    void NavigateToPage(const std::wstring& page);
    [[nodiscard]] HRESULT Reload() const;
    static void Close();
    bool CloseWebView(bool cleanupUserDataFolder);

    // Getters for WebView2 components
    [[nodiscard]] ICoreWebView2* GetWebView() const { return m_webview.Get(); }
    [[nodiscard]] ICoreWebView2Controller* GetController() const { return m_webviewController.Get(); }
    [[nodiscard]] ICoreWebView2Environment* GetEnvironment() const { return m_webviewEnvironment.get(); }

    using MemoryUsageCallback = std::function<void(SIZE_T currentUsage, SIZE_T threshold)>;

    // Method to set the callback
    void SetMemoryUsageHighLimit(const SIZE_T threshold)
    {
        // Let's set a reasonable minimum of 5Mb
        if (threshold > 5000000 && threshold << sizeof(long))
            m_memoryThreshold = threshold;
    };

    void SetMemoryUsageHighCallback(MemoryUsageCallback callback) {
        m_memoryUsageCallback = std::move(callback);
    }

    ~WebView2Manager() = default;
    WebView2Manager(const WebView2Manager&) = delete;
    WebView2Manager& operator=(const WebView2Manager&) = delete;
    WebView2Manager(WebView2Manager&&) = delete;
    WebView2Manager& operator=(WebView2Manager&&) = delete;
    void Set404Page(const std::wstring pg) { m_html404 = pg; };
private:
    HWND m_hWnd = nullptr;
    MemoryUsageCallback m_memoryUsageCallback;
    std::wstring m_html404;
    long m_memoryThreshold = 0x1FFFFFFFFL;  // 512Mb default

    void MemoryUsageHigh(SIZE_T currentUsage, SIZE_T threshold) const {
        if (m_memoryUsageCallback) {
            m_memoryUsageCallback(currentUsage, threshold);
        }
    }

    // Private methods
    void RecreateWebView();

    HRESULT CreateWebViewController(HWND hWnd, ICoreWebView2Environment* env);
    HRESULT ConfigureWebView(HWND hWnd);
    void ConfigureWebViewSettings();
    void SetupNavigationHandlers() const;
    void SetupMessageHandlers() const;
    static std::wstring GetUIPath();

    // Private members
    ComPtr<ICoreWebView2Controller> m_webviewController;
    ComPtr<ICoreWebView2> m_webview;
    ComPtr<NativeWindowControls> nativeControls;
    wil::com_ptr<ICoreWebView2Environment> m_webviewEnvironment;
};
