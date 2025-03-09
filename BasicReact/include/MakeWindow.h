#pragma once
#include <Windows.h>
#include "../AppMessageIDs.h"
#include "../WebViewManager.h"
#include "plog/Log.h"
#include "UIEvent.h"
#include <chrono>
#include <tchar.h>
#include "../resource.h" // Added for resource identifiers

namespace CPPGUI
{
	class MakeWindow
	{
		HINSTANCE hInstance = 0;
		int nCmdShow = 0;
		WebViewManager* wvMgr = nullptr;
		static MakeWindow* s_instance; // Static pointer to the current instance

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
		
		// Static window procedure that will forward calls to the instance method
		static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
		// Instance window procedure that can access member variables
		LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};

};