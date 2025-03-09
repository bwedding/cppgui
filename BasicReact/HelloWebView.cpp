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
#include "MakeWindow.h"

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

std::unique_ptr<WebViewManager> StartWebView(HINSTANCE hInstance)
{
	auto wvptr = std::make_unique<WebViewManager>(hWnd, hInstance);
	wvptr->Initialize();  
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

	hWnd = window.CreateMainWindow();
	if (!hWnd) 
		return -1;

	g_webViewManager = StartWebView(hInstance);
	
	// Set the WebViewManager in the MakeWindow instance
	window.SetWebViewManager(g_webViewManager.get());

	// Add subscriptions here // 
	g_webViewManager->Subscribe("auto-manual-control", [](const CPPGUI::UIEvent& evt) {
		return squarefoo(evt);
	});

	TestDatabaseAccess(g_webViewManager->GetWebView());

	auto result = window.RunMessageLoop();

	return result;
}

