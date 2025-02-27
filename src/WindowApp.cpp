#include "WindowApp.h"
#include <iostream>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <chrono>
#include <thread>
#include <random>
#include <vector>
#include "SystemData.h"
#include "UsbListener.h"
#include "USBDevice.h"
#include "SystemUtils.h"
#include "UIMapper.h"
#include "DemoData.h"
#include <tchar.h>

TCHAR WindowApp::szWindowClass[] = L"DesktopApp";
WindowApp* WindowApp::sInstance = nullptr;

constexpr int RESIZE_BORDER = 1;

// Add this function somewhere accessible
void processAutoManualControlEvent(const HeartControl::UIEvent& event) {
	spdlog::info("Called processAutoManualControlEvent");
	try {
		std::string payload = event.payload;
		auto data = nlohmann::json::parse(payload);

		std::string newMode = data["state"];  // Will be 'auto' or 'manual'
		const bool isEnabled = (newMode == "auto");  // true if auto, false if manual
		spdlog::info("Operation mode switched to:{0}  (value: {1})",
			newMode, (isEnabled ? "true" : "false"));
	}
	catch (const std::exception& e) {
		spdlog::error("Error processing mode switch: {0}", e.what());
	}
}

void TestSubscribe()
{
	SPDLOG_TRACE("Entering");
	auto& mEvtMgr = WindowApp::GetInstance()->GetEventManager();

	// Register it and get the callbackId
	auto cbID = mEvtMgr.subscribe("auto-manual-control", &processAutoManualControlEvent);

}
std::string squarefoo(const HeartControl::UIEvent& evt)
{
	spdlog::info("called squarefoo");
	return SystemUtils::FormatTimeStamp(evt.timestamp);
}
void startProcess() {}
double setTemperature(double temp) { return temp = 1; }

HeartControl::EventQueue& WindowApp::GetEventQueue()
{
	return mEventQueue;
}

WindowApp* WindowApp::GetInstance()
{
	return sInstance;
}


HRESULT WindowApp::TriggerEvent(const HeartControl::UIEvent& evt)
{
	auto& eventQueue = GetInstance()->GetEventQueue();
	eventQueue.enqueue(static_cast<HeartControl::UIEvent>(std::move(evt)));
	return S_OK;
}

LRESULT WindowApp::HandleMessage(const HWND hWnd, const UINT message, const WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_POST_MSG_TO_WEBVIEW: // Used to send JSON strings to the webview
	{
		if (auto pmessage = reinterpret_cast<std::wstring*>(wParam))
		{
			auto result = WebView2Manager::GetInstance().PostMessageToWebView(*pmessage);
			if (result != S_OK)
			{
				spdlog::error("Failed to post message to WebView2: {}", result);
			}
			delete pmessage;
		}
		return 0;
	}
	case WM_EXECUTE_SCRIPT_WEBVIEW:
	{
		if (auto pscript = reinterpret_cast<std::wstring*>(wParam))
		{
			const HRESULT hr = WebView2Manager::GetInstance().ExecuteScript(*pscript);  // This will now be on UI thread
			delete pscript;
			return hr;
		}
		return 0;
	}

	case WM_USER_EVENT:
	{
		HeartControl::UIEvent evt{
			"auto-manual-control",
			"User interface",
			"",
			system_clock::time_point{}
			};
		spdlog::info("Event from WebView2: '{}'", evt.type);

		int eventId = static_cast<int>(lParam);
		auto event = mEventManager->retrieveEvent(eventId);
		if (!event.type.empty()) {
			mEventQueue.enqueue(std::move(event));
		}
		break;
	}

	case WM_USER_SUBSCRIBE: {
		int callbackId = static_cast<int>(lParam);
		spdlog::debug("WM_USER_SUBSCRIBE received with ID: {}", callbackId);
		if (auto params = mCallbackRegistry.retrieveSubscribeParams(callbackId)) 
		{
			// Subscribe and store the result directly in the params
			params->resultSubscriptionId = mEventDispatcher.subscribe(
				params->eventType, std::move(params->callback));
			spdlog::info("Subscribing to event type: '{}'", params->eventType);
		}
		else {
			spdlog::error("Failed to retrieve subscription params for ID: {}", callbackId);
		}
		
		return 0;
	}

	case WM_USER_UNSUBSCRIBE: {
		int unsubscribeId = static_cast<int>(lParam);
		auto [eventType, subscriptionId] = mCallbackRegistry.retrieveUnsubscribe(unsubscribeId);
		if (!eventType.empty() && subscriptionId >= 0) {
			mEventDispatcher.unsubscribe(eventType, subscriptionId);
		}
		break;
	}
	
	case WM_USER_DISPATCH:
	{
		HeartControl::UIEvent testEvent{
					"auto-manual-control",
					"User interface",
					"",
					system_clock::time_point{}
		};
		mEventDispatcher.dispatch(testEvent);
		break;
	}	

	case WM_TIMER:
	{
		if (wParam == 1) {
			KillTimer(hWnd, 1);
			auto& webview = WebView2Manager::GetInstance();
			if (const auto controller = webview.GetController(); controller != nullptr)
			{
				RECT bounds;
				GetClientRect(hWnd, &bounds);
				bounds.left += RESIZE_BORDER;
				bounds.top += RESIZE_BORDER;
				bounds.right -= RESIZE_BORDER;
				bounds.bottom -= RESIZE_BORDER;
				controller->put_Bounds(bounds);
			}
		}
		break;
	}

	case WM_SIZE:  // This resizes the window contents
	{
		static UINT_PTR resizeTimerId = 0;
		if (resizeTimerId) {
			KillTimer(hWnd, resizeTimerId);
		}
		resizeTimerId = SetTimer(hWnd, 1, 5, nullptr); // ~60fps
		break;
	}
	case WM_NCHITTEST:  // This allows grabbing the border to resize
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		RECT rc;
		GetWindowRect(hWnd, &rc);

		// Don't allow resizing when window is maximized
		if (IsZoomed(hWnd))
			return HTCLIENT;

		// Check if cursor is on window edges using screen coordinates
		const bool left = pt.x < rc.left + RESIZE_BORDER;
		const bool right = pt.x >= rc.right - RESIZE_BORDER;
		const bool top = pt.y < rc.top + RESIZE_BORDER;
		const bool bottom = pt.y >= rc.bottom - RESIZE_BORDER;

		// Handle corners
		if (top && left)     return HTTOPLEFT;
		if (top && right)    return HTTOPRIGHT;
		if (bottom && left)  return HTBOTTOMLEFT;
		if (bottom && right) return HTBOTTOMRIGHT;

		// Handle edges
		if (left)            return HTLEFT;
		if (right)           return HTRIGHT;
		if (top)             return HTTOP;
		if (bottom)          return HTBOTTOM;

		return HTCLIENT;
	}
	case WM_PAINT:  // We don't paint the resize border because it's transparent
	{
		PAINTSTRUCT ps;
		const HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_ERASEBKGND:
	{
		auto hdc = reinterpret_cast<HDC>(wParam);
		RECT rect;
		GetClientRect(hWnd, &rect);
		SetBkColor(hdc, RGB(0, 0, 0));
		ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, nullptr, 0, nullptr);
		return TRUE;
	}
	case WM_NCCALCSIZE:
	{
		if (wParam == TRUE)
		{
			// Return 0 to preserve the full window area
			return 0;
		}
		break;
	}
	case WM_DESTROY:  // Shutdown
		PostQuitMessage(0);
		break;
	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK WindowApp::WndProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	WindowApp* pThis = nullptr;

	if (message == WM_NCCREATE)
	{
		const auto* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pThis = static_cast<WindowApp*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->m_hWnd = hWnd;  // Update the handle
		pThis->InitializeEventManager();
	}
	else
	{
		pThis = reinterpret_cast<WindowApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pThis)
	{
		return pThis->HandleMessage(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool WindowApp::CreateWindowClass(const HINSTANCE hInstance)
{
	SPDLOG_TRACE("Entering");

	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),						// cbSize
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		WndProc,						// lpfnWndProc
		0,										// cbClsExtra
		0,										// cbWndExtra
		hInstance,								// hInstance
		LoadIcon(hInstance, IDI_APPLICATION),	// hIcon
		LoadCursor(nullptr, IDC_ARROW),         // hCursor
		CreateSolidBrush(RGB(0, 0, 0)),			// hbrBackground
		nullptr,								// lpszMenuName
		szWindowClass,							// lpszClassName
		LoadIcon(hInstance, IDI_APPLICATION)	// hIconSm
	};

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(nullptr, _T("Call to RegisterClassEx failed!"), _T("Windows Desktop Guided Tour"), MB_OK);
		return false;
	}
	return true;
}

HWND& WindowApp::CreateWindows(const HINSTANCE hInstance)
{
	SPDLOG_TRACE("Entering");

	static HWND h_wnd = CreateWindow(
		szWindowClass, L"",
		WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 1600, 1100,
		nullptr, nullptr,
		hInstance, this
	);
	DWORD style = GetWindowLong(h_wnd, GWL_STYLE);
	style |= WS_SIZEBOX;
	SetWindowLong(h_wnd, GWL_STYLE, style);

	// Enable transparency
	LONG_PTR exStyle = GetWindowLongPtr(h_wnd, GWL_EXSTYLE);
	exStyle |= WS_EX_LAYERED;
	SetWindowLongPtr(h_wnd, GWL_EXSTYLE, exStyle);
	SetLayeredWindowAttributes(h_wnd, 0, 255, LWA_ALPHA);

	// Set zero margins
	const MARGINS margins = { 0, 0, 0, 0 };
	DwmExtendFrameIntoClientArea(h_wnd, &margins);

	const DWM_SYSTEMBACKDROP_TYPE backdropType = DWMSBT_MAINWINDOW; // or DWMSBT_TRANSIENTWINDOW
	DwmSetWindowAttribute(h_wnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));

	// Enable resizing
	style = GetWindowLong(h_wnd, GWL_STYLE);
	style |= WS_SIZEBOX;
	SetWindowLong(h_wnd, GWL_STYLE, style);

	// Disable default frame rendering
	constexpr DWMNCRENDERINGPOLICY ncrp = DWMNCRP_DISABLED;
	DwmSetWindowAttribute(h_wnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));

	// Set transparent border and caption colors
	constexpr COLORREF transparentColor = 0x00000000;
	DwmSetWindowAttribute(h_wnd, DWMWA_BORDER_COLOR, &transparentColor, sizeof(transparentColor));
	DwmSetWindowAttribute(h_wnd, DWMWA_CAPTION_COLOR, &transparentColor, sizeof(transparentColor));

	// Enable rounded corners
	constexpr DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND; // or try DWMWCP_ROUNDSMALL
	DwmSetWindowAttribute(h_wnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

	return h_wnd;
}

bool WindowApp::CreateViews(HINSTANCE& hInstance)
{
	SPDLOG_TRACE("Entering");
	if (!szWindowClass || !szWindowClass[0]) {
		return false;
	}

	if (!CreateWindowClass(m_hInstance)) {
		MessageBox(nullptr, _T("Error creating window class!"), _T("SRH Animal Monitor"), MB_OK);
		return false;
	}

	m_hWnd = CreateWindows(m_hInstance);
	if (!m_hWnd) {
		MessageBox(nullptr, _T("Call to CreateWindow failed!"), _T("SRH Animal Monitor"), MB_OK);
		return false;
	}
	SPDLOG_TRACE("Exit Trace");
	return true;
}

int WindowApp::Run(const HINSTANCE hInstance, const int nShowCmd)
{
	SPDLOG_TRACE("Entering");

	m_hInstance = hInstance;
	sInstance = this;
	
	if (!CreateViews(m_hInstance))
		return -1;

	ShowWindow(m_hWnd, nShowCmd);
	UpdateWindow(m_hWnd);

	auto& webview = WebView2Manager::GetInstance();
	if (const HRESULT hr = webview.Initialize(m_hWnd); FAILED(hr))
	{
		MessageBox(nullptr, _T("Failed to initialize WebView2 environment"), _T("SRH Animal Monitor"), MB_OK);
		return 1;
	}


	// Prepare the params
	HeartControl::SubscribeParams params;
	params.eventType = "auto-manual-control";
	params.callback = squarefoo;

	// Register it and get the callbackId
	const int callbackId = mCallbackRegistry.registerSubscribeParams(&params);

	// Send the message and wait for it to complete
	SendMessage(m_hWnd, WM_USER_SUBSCRIBE, 0, callbackId);

	// Now params.resultSubscriptionId contains the subscription ID
	int subscriptionId = params.resultSubscriptionId;

	// Usage example:
	UIMapper uiMapper(GetEventManager());

	//// Map a button to a simple function
	uiMapper.mapButton("start", []
	{
		startProcess();
		});

	//// Map a slider to a function that takes a value
	uiMapper.mapSlider("temperature", [](const double temp) {
		setTemperature(temp);
		});

	auto& commManager = CommunicationManager::Instance();
	Timer timer(3000, [&commManager]
	{

			StartSendingData();
			TestSubscribe();
	});

	MSG msg = {};

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) 
		{
			if (msg.message == WM_QUIT) 
			{
				break;
			}
			TranslateMessage(&msg);
			try 
			{
				DispatchMessage(&msg);
			}
			catch (...) 
			{
				OutputDebugString(L"Exception in DispatchMessage\n");
				spdlog::critical("Exception in DispatchMessage");
			}
		}
		else
			Sleep(0);  // Yield when no messages
	}

	return static_cast<int>(msg.wParam);
}



