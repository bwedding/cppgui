// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <chrono>
#include <thread>
#include <random>
#include <vector>

#include <wrl.h>
#include "../packages/Microsoft.Windows.ImplementationLibrary.1.0.240803.1/include/wil/com.h"
#include "../packages/Microsoft.Web.WebView2.1.0.3065.39/build/native/include/WebView2.h"
#include "../packages/Microsoft.Web.WebView2.1.0.3065.39/build/native/include/WebView2EnvironmentOptions.h"
#include <plog/Log.h> 
#include "plog/Initializers/RollingFileInitializer.h"
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/DynamicAppender.h>
#include <plog/Appenders/DebugOutputAppender.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "MSVCDebugFormatter.h"
#include "AsyncRollingFileAppender.h"
#include "TestDB.h"
#include "EventManager.h"
#include "NativeWindowControls.h"

using namespace Microsoft::WRL;

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("WebView sample");

HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Pointer to WebViewController
static wil::com_ptr<ICoreWebView2Controller> webviewController;

// Pointer to WebView window
static wil::com_ptr<ICoreWebView2> webview;
static CPPGUI::EventQueue mEventQueue;
static CPPGUI::EventDispatcher mEventDispatcher;
static CPPGUI::CallbackRegistry mCallbackRegistry;
static std::unique_ptr<CPPGUI::EventManager> mEventManager; // Changed to pointer

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	static plog::DebugOutputAppender<plog::MSVCDebugFormatter<false>> debugOutputAppender;
	static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("MultiAppender.csv", 40000000, 7); 
	plog::init(plog::debug, &fileAppender).addAppender(&debugOutputAppender);


	LOGD << "Hello log!"; // short macro

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1200, 900,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	// <-- WebView2 sample code starts here -->
	// Step 2.5 - Add options to allow opening files and disableing scrollbars
	const ComPtr<ICoreWebView2EnvironmentOptions> options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
	options->put_AdditionalBrowserArguments(L"--allow-file-access-from-files --hide-scrollbars");
	// May need to disable web security (CORS) if loading external sites
	//options->put_AdditionalBrowserArguments(L"--disable-web-security --user-data-dir=c:\\temp\\edgewebdata --hide-scrollbars");

	// Step 3 - Create a single WebView within the parent window
	// Locate the browser and set up the environment for WebView
	CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, options.Get(),
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

				// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
				env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[hWnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT 
					{
						if (controller != nullptr) {
							webviewController = controller;
							webviewController->get_CoreWebView2(&webview);
						}

						// Add a few settings for the webview
						// The demo step is redundant since the values are the default settings
						wil::com_ptr<ICoreWebView2Settings> settings;
						webview->get_Settings(&settings);
						settings->put_IsScriptEnabled(TRUE);
						settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						settings->put_IsWebMessageEnabled(TRUE);
						settings->put_AreDefaultScriptDialogsEnabled(true);
						settings->put_AreDevToolsEnabled(true);             // TODO May want to disable for production.
						settings->put_AreDefaultContextMenusEnabled(true);  // TODO May want to disable for production.
						settings->put_IsBuiltInErrorPageEnabled(false);     // This is set to false to disable built in error page for navigation failure and render process failure.
						settings->put_IsStatusBarEnabled(false);
						settings->put_AreHostObjectsAllowed(true);  // Required for native interop
						settings->put_IsZoomControlEnabled(true);   // Might want to disable this in the future. For now I'll leave it to handle different monitor resolutions
						auto nativeCtrls = Microsoft::WRL::Make<NativeWindowControls>(hWnd);
						VARIANT var = {};
						var.vt = VT_DISPATCH;
						HRESULT hr = nativeCtrls.Get()->QueryInterface(IID_IDispatch, reinterpret_cast<void**>(&var.pdispVal));
						if (FAILED(hr))
						{
							SPDLOG_ERROR("Failed to get IDispatch interface for native controls. HRESULT: 0x{:08X}", static_cast<unsigned int>(hr));

							// Attempt to provide a more specific error message based on common HRESULT values
							if (hr == E_NOINTERFACE)
							{
								SPDLOG_ERROR("The object doesn't support the requested interface");
							}
							else if (hr == E_POINTER)
							{
								SPDLOG_ERROR("Invalid pointer");
							}
							// Can't run without this so I'm just going to blow up.
							throw std::runtime_error("Critical error: Failed to initialize native controls");
						}

						// Add our NativeWindowControls as a host object
						webview->AddHostObjectToScript(L"native", &var);
						VariantClear(&var);

						// Resize WebView to fit the bounds of the parent window
						RECT bounds;
						GetClientRect(hWnd, &bounds);
						webviewController->put_Bounds(bounds);

						// Schedule an async task to navigate to Bing
						webview->Navigate(L"file:///C:/Users/bruce/source/cppgui/Frontend/UI/dist/index.html");

						// <NavigationEvents>
						// Step 4 - Navigation events
						// register an ICoreWebView2NavigationStartingEventHandler to cancel any non-https navigation
						EventRegistrationToken token;
						webview->add_NavigationStarting(Callback<ICoreWebView2NavigationStartingEventHandler>(
							[](ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT 
							{
								wil::unique_cotaskmem_string uri;
								args->get_Uri(&uri);
								std::wstring source(uri.get());
								return S_OK;
							}).Get(), &token);
						// </NavigationEvents>

						// <Scripting>
						webview->AddScriptToExecuteOnDocumentCreated(L"Object.freeze(Object);", nullptr);
						// Schedule an async task to get the document URL
						webview->ExecuteScript(L"window.document.URL;", Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
							[](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT 
							{
								LPCWSTR URL = resultObjectAsJson;
								return S_OK;
							}).Get());
						// </Scripting>

						// <CommunicationHostWeb>
						// Set an event handler for the host to return received message back to the web content
						webview->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
							[](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT 
							{
								wil::unique_cotaskmem_string message;
								args->TryGetWebMessageAsString(&message);
								// processMessage(&message);
								webview->PostWebMessageAsString(message.get());
								return S_OK;
							}).Get(), &token);

						// Schedule an async task to add initialization script that
						// 1) Add an listener to print message from the host
						// 2) Post document URL to the host
						webview->AddScriptToExecuteOnDocumentCreated(
							L"window.chrome.webview.addEventListener(\'message\', event => alert(event.data));" \
							L"window.chrome.webview.postMessage(window.document.URL);",
							nullptr);
						// </CommunicationHostWeb>

						return S_OK;
					}).Get());
				return S_OK;
			}).Get());


	
	// <-- WebView2 sample code ends here -->

	TestDatabaseAccess(webview);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		if (webviewController != nullptr) 
		{
			RECT bounds;
			GetClientRect(hWnd, &bounds);
			webviewController->put_Bounds(bounds);
		};
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
