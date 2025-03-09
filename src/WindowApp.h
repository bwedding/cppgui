#pragma once
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c
#include <windows.h>
#include <windowsx.h>
#include <memory>
#include "WebView.h"
#include "EventQueue.h"
#include "CommunicationManager.h"
#include "CallbackRegistry.h"
#include "EventManager.h"
#include "WebView2DataStreamer.h"
#include <commctrl.h>

class WindowApp
{
private:
	HWND dragHandle = nullptr;
	static TCHAR szWindowClass[];  // Declaration
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	static WindowApp* sInstance;

	CPPGUI::EventQueue mEventQueue;
	CPPGUI::EventDispatcher mEventDispatcher;
	CPPGUI::CallbackRegistry mCallbackRegistry;
	std::unique_ptr<CPPGUI::EventManager> mEventManager; // Changed to pointer
	std::unique_ptr<WebView2DataStreamer> m_dataStreamer = nullptr;

	LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static bool CreateWindowClass(HINSTANCE hInstance);
	HWND& CreateWindows(HINSTANCE hInstance);
	void EnableSnapLayouts(HWND hwnd);
	void EnableWindowSnapping(HWND hwnd);
	void CreateEnvironmentOnUIThread(HWND hWnd);

	// Static function as a trampoline to call the member function
	static LRESULT CALLBACK CustomSnapSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) 
	{
		WindowApp* pThis = reinterpret_cast<WindowApp*>(dwRefData);
		if (pThis) {
			return pThis->mCustomSnapSubclassProc(hWnd, message, wParam, lParam, 1);
		}
		return DefSubclassProc(hWnd, message, wParam, lParam);
	}

public:
	WindowApp() : m_hWnd(nullptr), m_hInstance(nullptr) {}
	~WindowApp()
	{
		auto ctr = webView2Manager.GetController();
		auto env = webView2Manager.GetEnvironment();
		auto wv = webView2Manager.GetWebView();
		wv = nullptr;
		env = nullptr;

		if (ctr != nullptr)
		{
			// First detach from parent window if applicable
			ctr->put_ParentWindow(nullptr);

			// Release any event tokens/handlers you registered

			// Explicitly release the reference before ComPtr's destructor runs
			auto tempPtr = ctr;
			ctr = nullptr;

			// Optional: You could also try setting the raw pointer to null
			// if you have access to the underlying raw pointer
		}
	}
	WebView2Manager webView2Manager;
	CommunicationManager communicationManager;

	// Public accessor for the event manager
	// Initialize the event manager once we have a valid window handle
	void InitializeEventManager()
	{
		if (!mEventManager && m_hWnd != nullptr) {
			mEventQueue.startProcessing(mEventDispatcher);
			mEventManager = std::make_unique<CPPGUI::EventManager>(m_hWnd, mEventDispatcher);
		}
	}

	// Public accessor for the event manager - return by reference
	[[nodiscard]] CPPGUI::EventManager& GetEventManager() const
	{
		if (!mEventManager) {
			throw std::runtime_error("EventManager not initialized");
		}
		return *mEventManager;
	}

	CPPGUI::EventQueue& GetEventQueue();
	static HRESULT TriggerEvent(const CPPGUI::UIEvent& evt);
	// Static accessor if needed from static contexts
	static WindowApp* GetInstance();

	bool CreateViews(HINSTANCE& hInstance);
	int Run(HINSTANCE hInstance, int nShowCmd);
	LRESULT mCustomSnapSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
		LPARAM lParam, UINT_PTR uIdSubclass);
};
