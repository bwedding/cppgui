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
#include "SystemUtils.h"

using namespace Microsoft::WRL;

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("WebView sample");

HINSTANCE hInst;
HWND hWnd = nullptr;
DWORD m_uiThreadId = 0;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Pointer to WebViewController
static wil::com_ptr<ICoreWebView2Controller> webviewController;

// Pointer to WebView window
static wil::com_ptr<ICoreWebView2> webview;
static CPPGUI::EventDispatcher mEventDispatcher;
static std::unique_ptr<CPPGUI::EventManager> mEventManager; // Changed to pointer
static CPPGUI::CallbackRegistry mCallbackRegistry;
static wil::com_ptr<NativeWindowControls> g_nativeControls; // Global instance of NativeWindowControls

std::string squarefoo(const CPPGUI::UIEvent& evt)
{
	LOGI << "CALLBACK EXECUTED: squarefoo called with event type: '{}'" << evt.type;
	LOGI << "Event details - source: '{}', data: '{}'" << evt.target << evt.payload;
	return SystemUtils::FormatTimeStamp(evt.timestamp);
}

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

	m_uiThreadId = GetCurrentThreadId();

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
	hWnd = CreateWindow(
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

	// Set the window handle in the callback registry
	mCallbackRegistry.setWindowHandle(hWnd);

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
			[](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

				// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
				env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT 
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
						g_nativeControls = Microsoft::WRL::Make<NativeWindowControls>(hWnd);
						VARIANT var = {};
						var.vt = VT_DISPATCH;
						HRESULT hr = g_nativeControls.get()->QueryInterface(IID_IDispatch, reinterpret_cast<void**>(&var.pdispVal));
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

						// Register for the auto-manual-control event
						CPPGUI::SubscribeParams params;
						params.eventType = "auto-manual-control";
						params.callback = squarefoo;

						// Register it and get the callbackId
						const int callbackId = mCallbackRegistry.registerSubscribeParams(&params);
						LOGI << "Registered callback for event type: 'auto-manual-control' with ID: " << callbackId;

						// Start the event queue processing with the event dispatcher
						//mEventQueue.startProcessing(*g_nativeControls->GetEventDispatcher());
						LOGI << "Started event queue processing";

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

HRESULT ExecuteScript(const std::wstring& script) 
{
	SPDLOG_TRACE("Entering");

	if (GetCurrentThreadId() == m_uiThreadId)
	{
		if (!webview)
		{
			return E_POINTER;
		}

		return webview->ExecuteScript(script.c_str(),
			Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
				[](const HRESULT error, const LPCWSTR result) -> HRESULT {
					if (FAILED(error))
					{
						LOGE << "Failed to execute script";

						return error;
					}
					if (result != nullptr)
					{
						std::wstring resultStr(L"Script result: ");
						resultStr += result;
						LOGI << L"Script result : {0}" << resultStr;
					}
					return S_OK;
				}).Get());
	}
	// Post to UI thread
	PostMessage(hWnd, WM_EXECUTE_SCRIPT_WEBVIEW, reinterpret_cast<WPARAM>(new std::wstring(script)), 0);
	return S_OK;
}

HRESULT PostMessageToWebView(const std::wstring& message)
{
	// Check if we're already on the UI thread
	if (GetCurrentThreadId() == m_uiThreadId)
	{
		if (!webview)
		{
			return E_POINTER;
		}
		try
		{
			return webview->PostWebMessageAsString(message.c_str());
		}
		catch (const std::exception& e)
		{
			LOGE << "Error posting message: {}" << std::string(e.what());
			return E_FAIL;
		}
	}
	// Post to UI thread and wait for completion
	PostMessage(hWnd, WM_POST_MSG_TO_WEBVIEW, reinterpret_cast<WPARAM>(new std::wstring(message)), 0);
	return S_OK;  // Note: This becomes async when called from another thread
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	///////////  Custom Messages
	case WM_POST_MSG_TO_WEBVIEW: // Used to send JSON strings to the webview
	{
		if (auto pmessage = reinterpret_cast<std::wstring*>(wParam))
		{
			// TODO Add error handling
			PostMessageToWebView(*pmessage);
			delete pmessage;
		}
		return 0;
	}
	case WM_EXECUTE_SCRIPT_WEBVIEW:
	{
		if (auto pscript = reinterpret_cast<std::wstring*>(wParam))
		{
			const HRESULT hr = ExecuteScript(*pscript);  // This will now be on UI thread
			delete pscript;
			return hr;
		}
		return 0;
	}

	case WM_USER_EVENT:
	{
		int eventId = static_cast<int>(lParam);
		LOGI << "WM_USER_EVENT received with ID: " << eventId;
		auto event = g_nativeControls->GetEventManager()->retrieveEvent(eventId);
		if (!event.type.empty()) {
			LOGI << "Retrieved event with type: '" << event.type << "', enqueueing";
			g_nativeControls->GetEventQueue()->enqueue(std::move(event));
		} else {
			LOGW << "Retrieved empty event for ID: " << eventId;
		}
		break;
	}

	case WM_USER_SUBSCRIBE: 
	{
		int callbackId = static_cast<int>(lParam);
		LOGD << "WM_USER_SUBSCRIBE received with ID: {}" << callbackId;
		if (auto params = mCallbackRegistry.retrieveSubscribeParams(callbackId))
		{
			// Check if g_nativeControls is initialized
			if (g_nativeControls) 
			{
				// Subscribe and store the result directly in the params
				auto eventdispatch = g_nativeControls->GetEventDispatcher();
				params->resultSubscriptionId = eventdispatch->subscribe(
					params->eventType, std::move(params->callback));  // Put the move back
				LOGI <<"Subscribing to event type: '{}'" << params->eventType;
			} 
			else 
			{
				LOGW << "g_nativeControls not initialized yet, cannot subscribe to event type: '" << params->eventType << "'";
				// Re-register the params and post the message again to try later
				int newCallbackId = mCallbackRegistry.registerSubscribeParams(params);
				// Post the message again with a delay to try again later
				PostMessage(hWnd, WM_USER_SUBSCRIBE, 0, newCallbackId);
			}
		}
		else 
		{
			LOGE << "Failed to retrieve subscription params for ID: {}" << callbackId;
		}

		return 0;
	}

	case WM_USER_UNSUBSCRIBE: 
	{
		int unsubscribeId = static_cast<int>(lParam);
		auto [eventType, subscriptionId] = mCallbackRegistry.retrieveUnsubscribe(unsubscribeId);
		if (!eventType.empty() && subscriptionId >= 0) 
		{
			g_nativeControls->GetEventDispatcher()->unsubscribe(eventType, subscriptionId);
		}
		break;
	}

	case WM_USER_DISPATCH:
	{
		CPPGUI::UIEvent testEvent{
					"auto-manual-control",
					"User interface",
					"",
					system_clock::time_point{}
		};
		g_nativeControls->GetEventDispatcher()->dispatch(testEvent);
		break;
	}

	///////////  End Custom Messages

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
