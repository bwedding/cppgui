// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>

#include <plog/Log.h> 
#include "plog/Initializers/RollingFileInitializer.h"
#include <plog/Appenders/DebugOutputAppender.h>
#include "MSVCDebugFormatter.h"
#include "AsyncRollingFileAppender.h"

#include "TestDB.h"
#include "NativeWindowControls.h"
#include "SystemUtils.h"
#include "WebViewManager.h"
#include "include/MakeWindow.h"
#include "resource.h" // Added for resource identifiers
#include "simdjson.h"

using namespace simdjson;
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
	//wvptr->SetLocalFolder(L"C:\\Users\\bruce\\source\\cppgui\\Frontend\\UI\\dist\\");
	wvptr->SetLocalFolder(L"D:\\source\\cppgui\\Frontend\\UI\\dist\\index.html");
    	
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

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	CPPGUI::MakeWindow window(hInstance, nCmdShow);

	InitializeLog();
	/*ondemand::parser parser;
	padded_string json = padded_string::load("twitter.json");
	ondemand::document tweets = parser.iterate(json);
	LOGD << uint64_t(tweets["search_metadata"]["count"]) << " results." << std::endl;*/

	// Make the window with defaults
	CPPGUI::MakeWindow makeWindow(hInstance, nCmdShow);
	// Get the configuation
	auto& config2 = makeWindow.GetConfiguration();
	// Modify the titlebar and frame to be all black with a gray text color
	config2.icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	config2.titleBarColor = RGB(0, 0, 0);
	config2.frameColor = RGB(0, 0, 0);
	config2.textColor = RGB(100, 100, 100);
	config2.resizable = false;
	hWnd = makeWindow.CreateMainWindow();
	if (!hWnd) 
		return -1;

	//g_webViewManager = StartWebView(hInstance, L"file:///C:/Users/bruce/source/cppgui/Frontend/UI/dist/index.html");
	g_webViewManager = StartWebView(hInstance, L"file:///D:/source/cppgui/Frontend/UI/dist/index.html");
	
	//g_webViewManager = StartWebView(hInstance, L"edge://gpu/");
	// g_webViewManager = StartWebView(hInstance, L"edge://memory/");  // won't load for permissions error
	//g_webViewManager = StartWebView(hInstance, L"edge://flags/");
	//g_webViewManager = StartWebView(hInstance, L"edge://media-internals/");
	//g_webViewManager = StartWebView(hInstance, L"edge://webrtc-internals/");
	//g_webViewManager = StartWebView(hInstance, L"edge://net-export/");  // Network logging of ALL activity
	//g_webViewManager = StartWebView(hInstance, L"edge://crash/");
	//g_webViewManager = StartWebView(hInstance, L"edge://process-internals/");
	//g_webViewManager = StartWebView(hInstance, L"edge://tracing/");
	//g_webViewManager = StartWebView(hInstance, L"edge://histograms/");
	//g_webViewManager = StartWebView(hInstance, L"edge://system/");

	
	// Set the WebViewManager in the MakeWindow instance
	window.SetWebViewManager(g_webViewManager.get());
	//window.SetThemeMode(CPPGUI::ThemeMode::Dark); // or ThemeMode::Light, ThemeMode::Dark, ThemeMode::System
	window.SetTopMost(hWnd, false);  // Turn off topmost window behavior


	// Add subscriptions here  
	g_webViewManager->Subscribe("auto-manual-control", [](const CPPGUI::UIEvent& evt) {
		return squarefoo(evt);
	});

	// Set up simplified navigation callback - much cleaner for frontend developers
	g_webViewManager->SetSimpleNavigationCallback([](const std::wstring& uri, bool isSuccess, const std::wstring& errorMessage) 
		{
			LOGI << "Navigation completed to: " << SystemUtils::WideToUtf8(uri);
			if (!isSuccess) 
			{
				LOGE << "Navigation failed: " << SystemUtils::WideToUtf8(errorMessage);
			// Here you could show a user-friendly error message in the UI
			// or take other appropriate actions based on the error
			}
			// Navigation succeeded! We're on the next page
	});

	// The detailed navigation callback is still available if needed for debugging or advanced error handling
	/*
	g_webViewManager->SetNavigationCompletedCallback([](const std::wstring& uri, bool isSuccess, COREWEBVIEW2_WEB_ERROR_STATUS errorStatus) {
		LOGI << "Navigation completed callback: " << SystemUtils::WideToUtf8(uri);
		if (!isSuccess) {
			LOGE << "Navigation failed with error status: " << errorStatus;
			// Handle different error statuses
			switch (errorStatus) {
			case COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED:
				LOGE << "Network disconnected";
				break;
			case COREWEBVIEW2_WEB_ERROR_STATUS_CONNECTION_ABORTED:
				LOGE << "Connection aborted";
				break;
			case COREWEBVIEW2_WEB_ERROR_STATUS_CONNECTION_RESET:
				LOGE << "Connection reset";
				break;
			case COREWEBVIEW2_WEB_ERROR_STATUS_SERVER_UNREACHABLE:
				LOGE << "Server unreachable";
				break;
			case COREWEBVIEW2_WEB_ERROR_STATUS_TIMEOUT:
				LOGE << "Connection timed out";
				break;
			case COREWEBVIEW2_WEB_ERROR_STATUS_ERROR_HTTP_INVALID_SERVER_RESPONSE:
				LOGE << "Invalid HTTP server response";
				break;
			case COREWEBVIEW2_WEB_ERROR_STATUS_HOST_NAME_NOT_RESOLVED:
				LOGE << "Host name not resolved";
				break;
			case COREWEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED:
				LOGE << "Operation canceled";
				break;
			case COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_EXPIRED:
				LOGE << "SSL certificate expired";
				break;
			case COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_IS_INVALID:
				LOGE << "SSL certificate is invalid";
				break;
			case COREWEBVIEW2_WEB_ERROR_STATUS_UNEXPECTED_ERROR:
				LOGE << "Unexpected error occurred";
				break;
			default:
				LOGE << "Other error status: " << errorStatus;
				break;
			}
		}
	});
	*/

	TestDatabaseAccess(g_webViewManager->GetWebView());

	auto result = window.RunMessageLoop();

	return result;
}
