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

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);
		return hWnd;
	}
}