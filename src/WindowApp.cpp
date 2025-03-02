#include "WindowApp.h"
#include <commctrl.h> // For window subclassing
#pragma comment(lib, "comctl32.lib")
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
BOOL g_isDragging = FALSE;
RECT g_lastWindowRect = { 0 };
BOOL g_isSnapped = FALSE;
DWORD g_lastSnapReleaseTime = 0;
RECT g_snappedRect = { 0 };
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
	case WM_CREATE:
	{
		// Enable snap layouts by setting the Windows 11 corner preference
		// This makes your window participate in the new snap layout UI
		DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
		DwmSetWindowAttribute(m_hWnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));

		// Set rounded corners for Windows 11 style
		int cornerPreference = DWMWCP_ROUND;
		DwmSetWindowAttribute(m_hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));

		return 0;
	}

	case WM_POST_MSG_TO_WEBVIEW: // Used to send JSON strings to the webview
	{
		if (auto pmessage = reinterpret_cast<std::wstring*>(wParam))
		{
			// TODO Add error handling
			WebView2Manager::GetInstance().PostMessageToWebView(*pmessage);
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

		// Define title bar area for dragging (you may need to adjust this)
		// This assumes you have a custom title bar that's TITLEBAR_HEIGHT pixels tall
		const int TITLEBAR_HEIGHT = 32; // Adjust to match your title bar height
		const bool inTitleBar = (pt.y >= rc.top && pt.y < rc.top + TITLEBAR_HEIGHT &&
			pt.x >= rc.left && pt.x < rc.right &&
			!left && !right && !top);

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

		// Return HTCAPTION for title bar to enable dragging and snap functionality
		if (inTitleBar)      return HTCAPTION;

		return HTCLIENT;
	}
	case WM_WINDOWPOSCHANGING:
	{
		WINDOWPOS* pwp = (WINDOWPOS*)lParam;

		// Skip if not a move
		if (pwp->flags & SWP_NOMOVE)
			break;

		// Get current time to implement cooldown periods
		DWORD currentTime = GetTickCount();

		// Get current monitor's work area
		HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(MONITORINFO) };
		GetMonitorInfo(hMonitor, &mi);
		RECT workArea = mi.rcWork;

		// Get current window rect and size
		RECT rcWindow;
		GetWindowRect(hWnd, &rcWindow);
		int width = rcWindow.right - rcWindow.left;
		int height = rcWindow.bottom - rcWindow.top;

		// Define snap parameters
		const int SNAP_DISTANCE = 16;        // Distance to trigger snapping
		const int DETACH_THRESHOLD = 15;     // Distance needed to escape snapping
		const DWORD SNAP_COOLDOWN = 500;     // Milliseconds to wait before snapping again

		// If window is currently snapped, check if we should release the snap
		if (g_isSnapped)
		{
			// Compare current position with snapped position
			int deltaX = abs(pwp->x - g_snappedRect.left);
			int deltaY = abs(pwp->y - g_snappedRect.top);

			// If moved enough, release the snap
			if (deltaX > DETACH_THRESHOLD || deltaY > DETACH_THRESHOLD)
			{
				g_isSnapped = FALSE;
				g_lastSnapReleaseTime = currentTime;
			}
			else
			{
				// Still snapped, maintain the snapped position
				pwp->x = g_snappedRect.left;
				pwp->y = g_snappedRect.top;
				return 0;
			}
		}

		// Don't snap again immediately after releasing
		if (currentTime - g_lastSnapReleaseTime < SNAP_COOLDOWN)
			break;

		// Check for snapping
		BOOL needSnap = FALSE;
		int newX = pwp->x;
		int newY = pwp->y;

		// Left edge
		if (abs(pwp->x - workArea.left) < SNAP_DISTANCE) {
			newX = workArea.left;
			needSnap = TRUE;
		}

		// Right edge
		if (abs((pwp->x + width) - workArea.right) < SNAP_DISTANCE) {
			newX = workArea.right - width;
			needSnap = TRUE;
		}

		// Top edge
		if (abs(pwp->y - workArea.top) < SNAP_DISTANCE) {
			newY = workArea.top;
			needSnap = TRUE;
		}

		// Bottom edge
		if (abs((pwp->y + height) - workArea.bottom) < SNAP_DISTANCE) {
			newY = workArea.bottom - height;
			needSnap = TRUE;
		}

		// Apply snap if needed
		if (needSnap) {
			pwp->x = newX;
			pwp->y = newY;

			// Record that we're now snapped and the snapped position
			g_isSnapped = TRUE;
			g_snappedRect.left = newX;
			g_snappedRect.top = newY;
			g_snappedRect.right = newX + width;
			g_snappedRect.bottom = newY + height;
		}
	}
	break;
	case WM_EXITSIZEMOVE:
		// Reset snapping state when drag operation ends
		g_isSnapped = FALSE;
		break;

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

void WindowApp::EnableSnapLayouts(HWND hwnd)
{
	// Check Windows version
	OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
	DWORDLONG dwlConditionMask = 0;
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, VER_GREATER_EQUAL);
	osvi.dwMajorVersion = 10;
	osvi.dwMinorVersion = 0;
	osvi.dwBuildNumber = 22000; // Windows 11

	if (VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, dwlConditionMask))
	{
		// Set window to participate in Snap Layouts
		BOOL value = TRUE;
		DwmSetWindowAttribute(hwnd, DWMWA_USE_HOSTBACKDROPBRUSH, &value, sizeof(value));

		// Set rounded corners for Windows 11
		int cornerPreference = DWMWCP_ROUND;
		DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));
	}
}

LRESULT CALLBACK CustomSnapSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_SYSCOMMAND:
		// Detect the start of window dragging
		if ((wParam & 0xFFF0) == SC_MOVE) {
			g_isDragging = TRUE;
			GetWindowRect(hWnd, &g_lastWindowRect);
		}
		break;

	case WM_EXITSIZEMOVE:
		// End of dragging operation
		g_isDragging = FALSE;
		break;

	case WM_DPICHANGED:
	{
		spdlog::info("WM_DPICHANGED received");
		// The RECT* passed in lParam contains the suggested new window size
		RECT* prcNewWindow = (RECT*)lParam;

		// Resize the window to the recommended size
		SetWindowPos(hWnd,
			NULL,
			prcNewWindow->left,
			prcNewWindow->top,
			prcNewWindow->right - prcNewWindow->left,
			prcNewWindow->bottom - prcNewWindow->top,
			SWP_NOZORDER | SWP_NOACTIVATE);

		// If you have a WebView2 controller, resize it to match the new window size
		auto ctrlr = WebView2Manager::GetInstance().GetController();
		if (ctrlr) {
			RECT bounds;
			GetClientRect(hWnd, &bounds);
			ctrlr->put_Bounds(bounds);

			// Notify WebView2 that the parent window position has changed
			ctrlr->NotifyParentWindowPositionChanged();
		}

		return 0;  // Return 0 to indicate the message was handled
	}

	case WM_WINDOWPOSCHANGED:
		
		// Window has been moved - check for snapping
		if (g_isDragging) {
			WINDOWPOS* pwp = (WINDOWPOS*)lParam;

			// Skip if not a move
			if (pwp->flags & SWP_NOMOVE)
				break;

			// Get current monitor's work area
			HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(MONITORINFO) };
			GetMonitorInfo(hMonitor, &mi);
			RECT workArea = mi.rcWork;

			// Get current window rect and size
			RECT rcWindow;
			GetWindowRect(hWnd, &rcWindow);
			int width = rcWindow.right - rcWindow.left;
			int height = rcWindow.bottom - rcWindow.top;

			// Check if we need to snap
			const int SNAP_DISTANCE = 10;
			BOOL needSnap = FALSE;
			int newX = rcWindow.left;
			int newY = rcWindow.top;

			// Left edge
			if (abs(rcWindow.left - workArea.left) < SNAP_DISTANCE) {
				newX = workArea.left;
				needSnap = TRUE;
			}

			// Right edge
			if (abs(rcWindow.right - workArea.right) < SNAP_DISTANCE) {
				newX = workArea.right - width;
				needSnap = TRUE;
			}

			// Top edge
			if (abs(rcWindow.top - workArea.top) < SNAP_DISTANCE) {
				newY = workArea.top;
				needSnap = TRUE;
			}

			// Bottom edge
			if (abs(rcWindow.bottom - workArea.bottom) < SNAP_DISTANCE) {
				newY = workArea.bottom - height;
				needSnap = TRUE;
			}

			// If the window needs to snap
			if (needSnap) {
				// Always apply the snap position, even if it seems the same
				// This ensures the window snaps precisely

				// Use PostMessage to avoid interfering with the current message processing
				SetWindowPos(hWnd, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			}

			// Save current position for next comparison
			g_lastWindowRect = rcWindow;
		}
		break;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Add this function to enable Windows 11 background effects
void EnableBackdropEffects(HWND hwnd)
{
	// Check if running on Windows 11
	OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
	DWORDLONG dwlConditionMask = 0;
	VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, VER_GREATER_EQUAL);
	osvi.dwBuildNumber = 22000; // Windows 11
	BOOL isWin11 = VerifyVersionInfo(&osvi, VER_BUILDNUMBER, dwlConditionMask);

	if (isWin11) {
		// For Windows 11, enable Mica effect
		// DWMWA_SYSTEMBACKDROP_TYPE = 38
		int backdropType = 2; // 2 = DWMSBT_MAINWINDOW (Mica)
		DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));

		// Enable rounded corners
		// DWMWA_WINDOW_CORNER_PREFERENCE = 33
		int cornerPreference = 2; // DWMWCP_ROUND
		DwmSetWindowAttribute(hwnd, 33, &cornerPreference, sizeof(cornerPreference));
	}
	else {
		// For Windows 10, try to enable Acrylic
		// This requires a bit more work
		//ACCENT_POLICY policy = { 0 };
		//policy.AccentState = 3; // ACCENT_ENABLE_BLURBEHIND

		//WINDOWCOMPOSITIONATTRIBDATA data = { 0 };
		//data.Attrib = WCA_ACCENT_POLICY;
		//data.pvData = &policy;
		//data.cbData = sizeof(policy);

		//// This requires SetWindowCompositionAttribute which isn't directly available
		//// You might need to dynamically load user32.dll and get the function pointer
		//typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
		//HMODULE hUser = GetModuleHandle(TEXT("user32.dll"));
		//if (hUser) {
		//	pSetWindowCompositionAttribute setWindowComposition =
		//		(pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
		//	if (setWindowComposition) {
		//		setWindowComposition(hwnd, &data);
		//	}
		//}
	}
}

// Subclass procedure to handle window snapping
LRESULT CALLBACK SnapWindowSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_MOVING:
	case WM_WINDOWPOSCHANGING:
	{
		auto pwp = (WINDOWPOS*)lParam;

		// Even if SWP_NOMOVE is set, we want to check if we're in a drag operation
		DWORD dwThreadID = GetCurrentThreadId();
		GUITHREADINFO gti = { sizeof(GUITHREADINFO) };
		if (!GetGUIThreadInfo(dwThreadID, &gti) || !(gti.flags & GUI_INMOVESIZE)) {
			// We're not in a move/size operation, so skip snapping
			break;
		}

		// We're in a move operation, so process snapping regardless of SWP_NOMOVE
		// Get current monitor's work area
		HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(MONITORINFO) };
		GetMonitorInfo(hMonitor, &mi);
		RECT workArea = mi.rcWork;

		// Get window rect and dimensions
		RECT rcWindow;
		GetWindowRect(hWnd, &rcWindow);
		int width = rcWindow.right - rcWindow.left;
		int height = rcWindow.bottom - rcWindow.top;

		// The current position might be in pwp even with SWP_NOMOVE
		// or we can use the current window position
		int x = (pwp->flags & SWP_NOMOVE) ? rcWindow.left : pwp->x;
		int y = (pwp->flags & SWP_NOMOVE) ? rcWindow.top : pwp->y;

		// Check for snapping to screen edges
		const int SNAP_DISTANCE = 16;
		bool snapped = false;

		// Left edge
		if (abs(x - workArea.left) < SNAP_DISTANCE) {
			pwp->x = workArea.left;
			// Clear the SWP_NOMOVE flag to allow position changes
			pwp->flags &= ~SWP_NOMOVE;
			snapped = true;
		}

		// Right edge
		if (abs((x + width) - workArea.right) < SNAP_DISTANCE) {
			pwp->x = workArea.right - width;
			pwp->flags &= ~SWP_NOMOVE;
			snapped = true;
		}

		// Top edge
		if (abs(y - workArea.top) < SNAP_DISTANCE) {
			pwp->y = workArea.top;
			pwp->flags &= ~SWP_NOMOVE;
			snapped = true;
		}

		// Bottom edge
		if (abs((y + height) - workArea.bottom) < SNAP_DISTANCE) {
			pwp->y = workArea.bottom - height;
			pwp->flags &= ~SWP_NOMOVE;
			snapped = true;
		}

		return snapped ? TRUE : 0;
	}

	// For Windows 11 snap layout support
	case WM_NCHITTEST:
	{
		// Let the default processing handle this message first
		LRESULT lResult = DefSubclassProc(hWnd, uMsg, wParam, lParam);

		// If it's not a client area, return the default result
		if (lResult != HTCLIENT)
			return lResult;

		// For Windows 11 snap layout support, we want to make the
		// top edge of the window behave like a title bar for snapping
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		RECT rc;
		GetWindowRect(hWnd, &rc);

		// Create a "virtual" title bar at the top of the window
		// This is important for Windows 11 snap layouts
		const int VIRTUAL_TITLEBAR_HEIGHT = 8; // Usually a thin strip is enough
		if (pt.y >= rc.top && pt.y < rc.top + VIRTUAL_TITLEBAR_HEIGHT)
			return HTCAPTION;

		return lResult;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}


// Apply the subclass procedure to your window
// Call this after your window is created
void WindowApp::EnableWindowSnapping(HWND hwnd)
{
	// Set up window subclassing
	//SetWindowSubclass(hwnd, SnapWindowSubclassProc, 1, 0);
	SetWindowSubclass(hwnd, CustomSnapSubclassProc, 1, 0);

	// Enable Windows 11 snap features if available
	BOOL isWin11 = FALSE;

	// Check if running on Windows 11 (build 22000 or higher)
	OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
	DWORDLONG dwlConditionMask = 0;
	VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, VER_GREATER_EQUAL);
	osvi.dwBuildNumber = 22000;
	if (VerifyVersionInfo(&osvi, VER_BUILDNUMBER, dwlConditionMask)) {
		isWin11 = TRUE;
	}

	if (isWin11) {
		// Enable rounded corners if on Windows 11
		int cornerPreference = 2; // DWMWCP_ROUND (2)
		DwmSetWindowAttribute(hwnd, 33, &cornerPreference, sizeof(cornerPreference));

		// Enable Windows 11 Snap Layouts
		BOOL value = TRUE;
		DwmSetWindowAttribute(hwnd, 35, &value, sizeof(value));  // DWMWA_USE_IMMERSIVE_DARK_MODE + 1
	}
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
		pThis->EnableWindowSnapping(hWnd);
		EnableBackdropEffects(hWnd);

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

	static HWND h_wnd = CreateWindowEx(
		WS_EX_NOREDIRECTIONBITMAP | WS_EX_APPWINDOW,
		szWindowClass, L"",
		WS_POPUP | WS_VISIBLE | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
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
	/*const MARGINS margins = { 0, 0, 0, 0 };
	DwmExtendFrameIntoClientArea(h_wnd, &margins);*/

	const DWM_SYSTEMBACKDROP_TYPE backdropType = DWMSBT_MAINWINDOW; // or DWMSBT_TRANSIENTWINDOW
	DwmSetWindowAttribute(h_wnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));

	// Extend frame into client area to allow Mica to show through
	MARGINS margins = { -1, -1, -1, -1 }; // Extend to entire client area
	DwmExtendFrameIntoClientArea(h_wnd, &margins);

	// Enable resizing
	style = GetWindowLong(h_wnd, GWL_STYLE);
	style |= WS_SIZEBOX;
	SetWindowLong(h_wnd, GWL_STYLE, style);

	//// Set transparent border and caption colors
	constexpr COLORREF transparentColor = 0x00000000;
	DwmSetWindowAttribute(h_wnd, DWMWA_BORDER_COLOR, &transparentColor, sizeof(transparentColor));

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

	// Monitor memory usage
	WebView2Manager::GetInstance().SetMemoryUsageHighCallback([](SIZE_T usage, SIZE_T threshold) 
	{
		SPDLOG_WARN("WebView memory usage high: {} bytes (threshold: {} bytes)", usage, threshold);
		// Try to reload the webview. We may want to do more here later.
		WebView2Manager::GetInstance().Reload();
	});

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



