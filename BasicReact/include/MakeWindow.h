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

	// Enum for window states
	enum class WindowState {
		Normal,    // Normal window state
		Maximized, // Maximized window
		Minimized  // Minimized window
	};

	// Structure for main window configuration
	struct MainWindowConfiguration {
		// Window dimensions
		int width = 1200;
		int height = 900;
		
		// Window appearance
		const TCHAR* title = nullptr;
		HICON icon = nullptr;
		
		// Window style options
		bool topMost = false;
		bool toolWindow = false;
		bool layered = false;
		bool resizable = true;     // Whether window is resizable
		BYTE opacity = 255; // Only used if layered is true (0-255)
		int borderWidth = -1;  // -1 means use system default
		WindowState initialState = WindowState::Normal;  // Initial window state
		
		// Theme options
		bool themingEnabled = true;
		ThemeMode themeMode = ThemeMode::System;
		
		// Window colors
		COLORREF titleBarColor = CLR_INVALID;  // Use system default if CLR_INVALID
		COLORREF frameColor = CLR_INVALID;     // Use system default if CLR_INVALID
		COLORREF textColor = CLR_INVALID;      // Use system default if CLR_INVALID
		COLORREF captionButtonHoverColor = CLR_INVALID;  // Use system default if CLR_INVALID
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
		
		// Color customization methods
		void SetTitleBarColor(HWND hWnd, COLORREF color);
		void SetFrameColor(HWND hWnd, COLORREF color);
		void SetTextColor(HWND hWnd, COLORREF color);
		void SetCaptionButtonHoverColor(HWND hWnd, COLORREF color);
		
		// Border customization
		void SetBorderWidth(HWND hWnd, int width);
		
		// Window styling
		void SetTopMost(HWND hWnd, bool topMost);
		bool IsTopMost() const { return m_isTopMost; }
		
		// Resizable window control
		void SetResizable(HWND hWnd, bool resizable);
		bool IsResizable() const { return m_config.resizable; }
		
		// Window state control
		void SetWindowState(HWND hWnd, WindowState state);
		
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