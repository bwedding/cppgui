classDiagram
    class WindowApp {
        -HWND dragHandle
        -HWND m_hWnd
        -HINSTANCE m_hInstance
        +Run(hInstance, nShowCmd) int
        +CreateViews(hInstance) bool
        +MinimizeWindow() HRESULT
        +MaximizeWindow() HRESULT
        +CloseWindow() HRESULT
        -HandleMessage(hWnd, message, wParam, lParam) LRESULT
        -CreateWindowClass(hInstance) bool
        -CreateWindows(hInstance) HWND&
    }

    class CommunicationManager {
        -HWND m_hwnd
        +Instance() CommunicationManager&
        +SendScriptToFrontend(script) bool
        +SendMessageToFrontend(message) bool
        +Initialize(hwnd) bool
        +SubscribeToEvent(eventName, schema, callback) EventToken
    }

    class StockTicker {
        -vector~StockData~ stocks
        +UpdateStockPrices() void
        +GenerateRandomMessage() wstring
    }

    class Timer {
        -int delay_ms_
        -function callback_
        -thread thread_
        +Timer(delay_ms, callback)
    }

    class StockData {
        +wstring symbol
        +double lastPrice
        +double changePercent
    }

    class WebView2Manager {
        -ICoreWebView2 coreWebView2
        -ICoreWebView2Controller coreWebView2Controller
        -ICoreWebView2Environment coreWebView2Environment
        -NativeWindowControls nativeWindowControls
    }

    WindowApp --> CommunicationManager : uses
    WindowApp --> StockTicker : creates
    StockTicker --> StockData : contains
    WindowApp --> Timer : uses
    CommunicationManager --> WebView2Manager : uses
    WebView2Manager --> ICoreWebView2 : manages
    WebView2Manager --> ICoreWebView2Controller : manages
    WebView2Manager --> ICoreWebView2Environment : manages
    WebView2Manager --> NativeWindowControls : manages