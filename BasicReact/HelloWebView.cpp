// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <chrono>
#include <iostream>
#include <iomanip>

#include <plog/Log.h> 
#include "plog/Initializers/RollingFileInitializer.h"
#include <plog/Appenders/DebugOutputAppender.h>
#include "MSVCDebugFormatter.h"
#include "AsyncRollingFileAppender.h"

#include "TestDB.h"
#include "NativeWindowControls.h"
#include "SystemUtils.h"
#include "WebViewManager.h"
#include "MakeWindow.h"
#include "../resource.h" // Added for resource identifiers
#include "DataSender.h"

using namespace Microsoft::WRL;

HINSTANCE hInst;
HWND hWnd = nullptr;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

std::unique_ptr<WebViewManager> g_webViewManager = nullptr;

std::string squarefoo(const CPPGUI::UIEvent& evt)
{
	LOGI << "CALLBACK EXECUTED: squarefoo called with event type: " << evt.type;
	LOGI << "Event details - source: " << evt.target << " data: " << evt.payload;
	return SystemUtils::FormatTimeStamp(evt.timestamp);
}

std::unique_ptr<WebViewManager> StartWebView(HINSTANCE hInstance, const std::wstring& navigationURL)
{
	auto wvptr = std::make_unique<WebViewManager>(hWnd, hInstance);
	
	// Sets the initial URL that the WebView will navigate to when initialized
	wvptr->SetNavigationURL(navigationURL);
	
	// Sets the local folder for file-based navigation with the NavigateToPage method
	// This is the base directory for all page navigations
	wvptr->SetLocalFolder(L"C:\\Users\\bruce\\source\\cppgui\\Frontend\\UI\\dist\\");
	
	// Optional: Set a custom 404 page (you can use %PAGE% as a placeholder for the requested page)
	// wvptr->SetCustom404Page(L"<html><body><h1>Custom 404</h1><p>Could not find: %PAGE%</p></body></html>");
	
	// Enables developer tools (F12) for debugging and inspecting web content
	wvptr->SetDevToolsEnabled(true);
	
	// Enables right-click context menus (copy, paste, inspect, etc.)
	wvptr->SetDefaultContextMenusEnabled(true);
	
	// Disables the built-in error pages (e.g., for 404, connection errors)
	// When false, you can handle errors yourself through navigation events
	wvptr->SetBuiltInErrorPageEnabled(false);
	
	// Shows the status bar at the bottom of the WebView when hovering over links
	wvptr->SetStatusBarEnabled(true);
	
	// Enables zoom controls (Ctrl+/- and Ctrl+mouse wheel)
	wvptr->SetZoomControlEnabled(true);
	
	wvptr->Initialize();  
	
	// Example of how to navigate to a page relative to the local folder:
	// wvptr->NavigateToPage(L"/index.html");
	
	return wvptr;
}

void InitializeLog()
{
	static plog::DebugOutputAppender<plog::MSVCDebugFormatter<false>> debugOutputAppender;
  																	// 1Mb files, keep up to 7
	static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("MultiAppender.txt", 1000000, 7);
	plog::init(plog::debug, &fileAppender).addAppender(&debugOutputAppender);
	LOGD << "Launching Hello WebView!";
}

extern void GlazeTest();

// Timer ID for retrying WebView pointer acquisition
#define TIMER_RETRY_WEBVIEW 1002

// Global flag to track if we've started the sender thread
bool g_senderStarted = false;

// Timer procedure for retrying WebView pointer acquisition
VOID CALLBACK WebViewRetryTimerProc(HWND hwnd, UINT msg, UINT_PTR timerId, DWORD time) {
	LOGI << "Timer fired, retrying to get WebView pointer";
	auto webview = g_webViewManager->GetWebView();
	if (webview) {
		LOGI << "WebView pointer is now valid on retry, starting sender thread";
		std::thread stringDataSenderThread(SendStringData, hwnd);
		stringDataSenderThread.detach();
		KillTimer(hwnd, timerId);
	} else {
		static int retryCount = 0;
		retryCount++;
		
		if (retryCount >= 10) {  // Give up after 10 retries (10 seconds)
			LOGE << "Giving up after 10 retries. WebView pointer is still null!";
			KillTimer(hwnd, timerId);
		} else {
			LOGE << "WebView pointer is still null on retry " << retryCount << "/10!";
		}
	}
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	InitializeLog();

	GlazeTest();

	// Make the window with defaults
	CPPGUI::MakeWindow makeWindow(hInstance, nCmdShow);
	// Get the configuation
	auto& config2 = makeWindow.GetConfiguration();
	// Modify the titlebar and frame to be all black with a gray text color
	config2.icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	//config2.titleBarColor = RGB(0, 0, 0);
	//config2.frameColor = RGB(0, 0, 0);
	//config2.textColor = RGB(100, 100, 100);
	hWnd = makeWindow.CreateMainWindow();
	if (!hWnd) 
		return -1;

	g_webViewManager = StartWebView(hInstance, L"file:///C:/Users/bruce/source/cppgui/Frontend/UI/dist/index.html");
	//g_webViewManager = StartWebView(hInstance, L"edge://gpu/");

	
	// Set the WebViewManager in the MakeWindow instance
	makeWindow.SetWebViewManager(g_webViewManager.get());
	makeWindow.SetThemeMode(CPPGUI::ThemeMode::Dark); // or ThemeMode::Light, ThemeMode::Dark, ThemeMode::System
	makeWindow.SetTopMost(hWnd, false);  // Turn off topmost window behavior


	// Add subscriptions here  
	g_webViewManager->Subscribe("auto-manual-control", [](const CPPGUI::UIEvent& evt) {
		return squarefoo(evt);
	});

	// Set up detailed navigation callback - more technical details
	g_webViewManager->SetNavigationCompletedCallback([](const std::wstring& uri, bool isSuccess, COREWEBVIEW2_WEB_ERROR_STATUS errorStatus) {
		if (isSuccess) {
			LOGI << "Navigation completed to: " << SystemUtils::WideToUtf8(uri) << " Success: true";
		}
		else {
			LOGW << "Navigation failed with status: " << errorStatus;
		}
	});

	// Set up simplified navigation callback - much cleaner for frontend developers
	g_webViewManager->SetSimpleNavigationCallback([hwndMain = hWnd](const std::wstring& uri, bool isSuccess, const std::wstring& errorMessage) {
		if (isSuccess) {
			LOGI << "Navigation successful to: " << SystemUtils::WideToUtf8(uri);
			
			// Start the string data sender thread after successful navigation
			if (!g_senderStarted) {
				g_senderStarted = true;
				LOGI << "Starting string data sender thread after successful navigation";
				
				// Initialize the data sender with the main window handle
				LOGI << "Initializing data sender with window handle: " << hwndMain;
				InitializeDataSender(hwndMain);
				
				// Start the string data sender thread with the window handle
				std::thread stringDataSenderThread(SendStringData, hwndMain);
				stringDataSenderThread.detach();
				
				// Optionally start the JSON data sender thread as well
				// std::thread jsonDataSenderThread(SendJSONData, hwndMain);
				// jsonDataSenderThread.detach();
			}
		}
		else {
			LOGE << "Navigation failed: " << SystemUtils::WideToUtf8(errorMessage);
		}
	});

	TestDatabaseAccess(g_webViewManager->GetWebView());

	auto result = makeWindow.RunMessageLoop();

	// Set the global flag to stop the sender thread
	g_keepSending = false;

	return result;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_SIZE:
		if (g_webViewManager) {
			RECT bounds;
			GetClientRect(hWnd, &bounds);
			g_webViewManager->Resize(bounds);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PROCESS_WEBVIEW_MESSAGE:
		// Process messages queued by the data sender threads
		return ProcessWebViewMessage(hWnd, wParam, lParam);
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}
