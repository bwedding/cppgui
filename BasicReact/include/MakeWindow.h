#pragma once
#include <Windows.h>
#include <dwmapi.h>    // For DwmSetWindowAttribute
#include <uxtheme.h>   // For theme-related functions
#include "../AppMessageIDs.h"
#include "../WebViewManager.h"
#include "plog/Log.h"
#include "UIEvent.h"
#include <chrono>
#include <tchar.h>
#include "../resource.h" // Added for resource identifiers

// Link to the necessary libraries
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "uxtheme.lib")

namespace CPPGUI
{
	// Enum for theme modes
	enum class ThemeMode {
		System,  // Follow system setting
		Light,   // Force light mode
		Dark     // Force dark mode
	};

	class MakeWindow
	{
		HINSTANCE hInstance = 0;
		int nCmdShow = 0;
		WebViewManager* wvMgr = nullptr;
		static MakeWindow* s_instance; // Static pointer to the current instance
		
		// Theme-related members
		ThemeMode m_themeMode = ThemeMode::System;
		bool m_isDarkMode = false;
		bool m_isThemingEnabled = true;

	public:
		MakeWindow(HINSTANCE hInst, int nCmd) :hInstance(hInst), nCmdShow(nCmd) {
			s_instance = this; // Store the instance for static method access
		}
		
		~MakeWindow() {
			if (s_instance == this) {
				s_instance = nullptr;
			}
		}

		HWND CreateMainWindow(int width = 1200, int height = 900, const TCHAR* title = nullptr, HICON icon = nullptr);
		void SetWebViewManager(WebViewManager* wv);
		WebViewManager* GetWebViewManager() const { return wvMgr; }
		int RunMessageLoop()
		{
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return (int)msg.wParam;
		}
		
		// Theme related methods
		void SetThemeMode(ThemeMode mode);
		ThemeMode GetThemeMode() const { return m_themeMode; }
		bool IsDarkMode() const { return m_isDarkMode; }
		void EnableTheming(bool enable) { m_isThemingEnabled = enable; }
		bool IsThemingEnabled() const { return m_isThemingEnabled; }
		
		// Apply theme to window and WebView
		void ApplyThemeToWindow(HWND hWnd);
		
		// Detect system dark mode status
		bool IsSystemInDarkMode() const;
		
		// Update theme based on current settings
		void UpdateTheme(HWND hWnd);
		
		// Static window procedure that will forward calls to the instance method
		static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
		// Instance window procedure that can access member variables
		LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};

};