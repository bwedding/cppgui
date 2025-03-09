#include "MakeWindow.h"

namespace CPPGUI
{
	static TCHAR szWindowClass[] = _T("DesktopApp");
	static TCHAR szTitle[] = _T("WebView sample");

	// Initialize the static instance pointer
	MakeWindow* MakeWindow::s_instance = nullptr;

	// Static window procedure that forwards to the instance method
	LRESULT CALLBACK MakeWindow::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		// Forward to the instance method if we have an instance
		if (s_instance) {
			return s_instance->WndProc(hWnd, message, wParam, lParam);
		}
		// Fall back to default processing if no instance is available
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	// Instance window procedure that can access member variables
	LRESULT MakeWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			///////////  Custom Messages
		case WM_USER_EVENT:
		{
			int eventId = static_cast<int>(lParam);
			LOGD << "WM_USER_EVENT received with ID: {}" << eventId;
			if (wvMgr && wvMgr->GetNativeControls())
			{
				auto event = wvMgr->GetNativeControls()->GetEventManager()->retrieveEvent(eventId);
				if (!event.type.empty())
				{
					wvMgr->GetNativeControls()->GetEventDispatcher()->dispatch(event);
					LOGD << "Dispatched event: {}" << event.type;
				}
				else
				{
					LOGW << "Retrieved empty event for ID: " << eventId;
				}
			}
			break;
		}

		case WM_USER_DISPATCH:
		{
			CPPGUI::UIEvent testEvent{
						"auto-manual-control",
						"User interface",
						"",
						std::chrono::system_clock::time_point{}
			};
			if (wvMgr && wvMgr->GetNativeControls()) {
				wvMgr->GetNativeControls()->GetEventDispatcher()->dispatch(testEvent);
			}
			break;
		}

		///////////  End Custom Messages

		case WM_TIMER:
			if (wParam == TIMER_ID_SUBSCRIPTION_RETRY && wvMgr) {
				// Process any pending subscriptions
				wvMgr->ProcessPendingSubscriptions();
			}
			break;

		case WM_SIZE:
			if (wvMgr)
			{
				RECT bounds;
				GetClientRect(hWnd, &bounds);
				wvMgr->Resize(bounds);
			};
			break;

		case WM_SETTINGCHANGE:
			if (lParam && lstrcmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) {
				// System theme has changed, update our theme if in System mode
				if (m_themeMode == ThemeMode::System) {
					UpdateTheme(hWnd);
				}
			}
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_DWMCOLORIZATIONCOLORCHANGED:
			// DWM colorization color has changed, update our theme
			if (m_themeMode == ThemeMode::System) {
				UpdateTheme(hWnd);
			}
			return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}

		return 0;
	}

	void MakeWindow::SetWebViewManager(WebViewManager* wv)
	{
		if (wv)
			wvMgr = wv;
		else
			LOGE << "Passed me a null pointer WebViewManager!";
	}

	HWND MakeWindow::CreateMainWindow(int width, int height, const TCHAR* title, HICON icon) 
	{
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = StaticWndProc; // Use our static window procedure
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = icon ? icon : LoadIcon(hInstance, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = icon ? icon : LoadIcon(wcex.hInstance, IDI_APPLICATION);

		if (!RegisterClassEx(&wcex)) {
			MessageBox(NULL,
				_T("Call to RegisterClassEx failed!"),
				_T("Windows Desktop Guided Tour"),
				NULL);
			return nullptr;
		}

		HWND hWnd = CreateWindow(
			szWindowClass,
			title ? title : szTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			width, height,
			NULL,
			NULL,
			hInstance,
			NULL
		);

		if (!hWnd) return nullptr;

		// Apply theme settings immediately after window creation
		if (m_isThemingEnabled) {
			// Update theme based on current settings
			UpdateTheme(hWnd);
		}

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);
		return hWnd;
	}

	bool MakeWindow::IsSystemInDarkMode() const
	{
		// Check if system is using dark mode
		BOOL isDarkMode = FALSE;
		DWORD darkModeValue = 0;
		DWORD bufferSize = sizeof(darkModeValue);
		
		// Try to get the system theme setting from registry
		// Apps/Windows dark mode value
		HKEY hKey;
		if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			if (RegQueryValueExW(hKey, L"AppsUseLightTheme", 0, NULL, reinterpret_cast<LPBYTE>(&darkModeValue), &bufferSize) == ERROR_SUCCESS) {
				isDarkMode = (darkModeValue == 0); // 0 means dark mode
			}
			RegCloseKey(hKey);
		}
		
		return isDarkMode == TRUE;
	}
	
	void MakeWindow::SetThemeMode(ThemeMode mode)
	{
		if (m_themeMode != mode) {
			m_themeMode = mode;
			
			// If we have a window, update its theme
			HWND hWnd = FindWindow(szWindowClass, NULL);
			if (hWnd) {
				UpdateTheme(hWnd);
			}
		}
	}
	
	void MakeWindow::UpdateTheme(HWND hWnd)
	{
		if (!m_isThemingEnabled) return;
		
		// Determine if we should be in dark mode
		bool shouldBeDarkMode = false;
		switch (m_themeMode) {
			case ThemeMode::System:
				shouldBeDarkMode = IsSystemInDarkMode();
				break;
			case ThemeMode::Light:
				shouldBeDarkMode = false;
				break;
			case ThemeMode::Dark:
				shouldBeDarkMode = true;
				break;
		}
		
		// Only update if the mode has changed
		if (m_isDarkMode != shouldBeDarkMode) {
			m_isDarkMode = shouldBeDarkMode;
			
			// Apply theme to window
			ApplyThemeToWindow(hWnd);
			
			// Apply theme to WebView if available
			if (wvMgr && wvMgr->GetWebView()) {
				// Apply dark mode to WebView by injecting appropriate CSS
				std::wstring script;
				if (m_isDarkMode) {
					script = L"document.documentElement.setAttribute('data-theme', 'dark');";
				} else {
					script = L"document.documentElement.setAttribute('data-theme', 'light');";
				}
				wvMgr->ExecuteScript(script);
			}
		}
	}
	
	void MakeWindow::ApplyThemeToWindow(HWND hWnd)
	{
		if (!m_isThemingEnabled || !hWnd) return;
		
		// Apply dark mode to window frame
		BOOL darkMode = m_isDarkMode ? TRUE : FALSE;
		
		// Set the window attribute for dark mode (Windows 10 1809 and later)
		DwmSetWindowAttribute(
			hWnd, 
			20, // DWMWA_USE_IMMERSIVE_DARK_MODE (20 for Windows 11/10 build 19041 and later, 19 for earlier Windows 10 builds)
			&darkMode, 
			sizeof(darkMode)
		);
		
		// Set title bar color (if in dark mode)
		if (m_isDarkMode) {
			// Dark gray color for title bar in dark mode
			COLORREF darkGray = RGB(32, 32, 32);
			DwmSetWindowAttribute(
				hWnd, 
				35, // DWMWA_CAPTION_COLOR (Windows 11 and later)
				&darkGray, 
				sizeof(darkGray)
			);
		} else {
			// Use default title bar color for light mode
			// This resets the title bar to system default
			DwmSetWindowAttribute(
				hWnd, 
				35, // DWMWA_CAPTION_COLOR
				nullptr, 
				0
			);
		}
		
		// Force a redraw of the window
		SetWindowPos(
			hWnd, 
			NULL, 
			0, 0, 0, 0, 
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
		);
		
		// Invalidate and redraw
		InvalidateRect(hWnd, NULL, TRUE);
	}
}