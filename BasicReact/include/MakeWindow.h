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

	// Structure for main window configuration
	struct MainWindowConfiguration {
		// Window dimensions
		int width = 1200;
		int height = 900;
		
		// Window title and icon
		const TCHAR* title = nullptr;
		HICON icon = nullptr;
		
		// Window style options
		bool topMost = false;
		bool toolWindow = false;
		bool layered = false;
		BYTE opacity = 255; // Only used if layered is true (0-255)
		
		// Theme options
		ThemeMode themeMode = ThemeMode::System;
		bool themingEnabled = true;
		
		// Colors (all initialized to use system defaults)
		COLORREF titleBarColor = CLR_INVALID;        // Title bar background color
		COLORREF frameColor = CLR_INVALID;           // Window frame color
		COLORREF textColor = CLR_INVALID;            // Title text color
		COLORREF captionButtonHoverColor = CLR_INVALID;  // Caption button hover color
	};

	class MakeWindow
	{
		HINSTANCE hInstance = 0;
		int nCmdShow = 0;
		WebViewManager* wvMgr = nullptr;
		static MakeWindow* s_instance; // Static pointer to the current instance
		
		// Store window configuration
		MainWindowConfiguration m_config;
		
		// Theme-related members
		bool m_isDarkMode = false;
		
		// Window style members
		bool m_isTopMost = false;

	public:
		MakeWindow(HINSTANCE hInst, int nCmd) :hInstance(hInst), nCmdShow(nCmd) {
			s_instance = this; // Store the instance for static method access
		}
		
		// Constructor that takes a configuration struct
		MakeWindow(HINSTANCE hInst, int nCmd, const MainWindowConfiguration& config) 
			: hInstance(hInst), nCmdShow(nCmd), m_config(config) {
			s_instance = this; // Store the instance for static method access
		}
		
		~MakeWindow() {
			if (s_instance == this) {
				s_instance = nullptr;
			}
		}

		// Configuration getter and setter
		void SetConfiguration(const MainWindowConfiguration& config) { m_config = config; }
		MainWindowConfiguration& GetConfiguration() { return m_config; }
		const MainWindowConfiguration& GetConfiguration() const { return m_config; }

		HWND CreateMainWindow();
		
		// Legacy overload for backward compatibility
		HWND CreateMainWindow(int width, int height, const TCHAR* title = nullptr, HICON icon = nullptr, bool topMost = false) {
			m_config.width = width;
			m_config.height = height;
			m_config.title = title;
			m_config.icon = icon;
			m_config.topMost = topMost;
			return CreateMainWindow();
		}
		
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
		
		// Theme and color methods
		void SetThemeMode(ThemeMode mode);
		ThemeMode GetThemeMode() const { return m_config.themeMode; }
		bool IsDarkMode() const { return m_isDarkMode; }
		void EnableTheming(bool enable) { m_config.themingEnabled = enable; }
		bool IsThemingEnabled() const { return m_config.themingEnabled; }
		
		// Apply theme to window and WebView
		void ApplyThemeToWindow(HWND hWnd);
		
		// Window coloring methods
		void SetTitleBarColor(HWND hWnd, COLORREF color);
		void SetFrameColor(HWND hWnd, COLORREF color);
		void SetTextColor(HWND hWnd, COLORREF color);
		void SetCaptionButtonHoverColor(HWND hWnd, COLORREF color);
		
		// Detect system dark mode status
		bool IsSystemInDarkMode() const;
		
		// Update theme based on current settings
		void UpdateTheme(HWND hWnd);
		
		// Window style methods
		void SetTopMost(HWND hWnd, bool topMost);
		bool IsTopMost() const { return m_isTopMost; }
		
		// Static window procedure that will forward calls to the instance method
		static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
		// Instance window procedure that can access member variables
		LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};

};