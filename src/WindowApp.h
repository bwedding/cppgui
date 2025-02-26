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

class WindowApp
{
private:
	HWND dragHandle = nullptr;
	static TCHAR szWindowClass[];  // Declaration
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	static WindowApp* sInstance;

	HeartControl::EventQueue mEventQueue;
	HeartControl::EventDispatcher mEventDispatcher;
	HeartControl::CallbackRegistry mCallbackRegistry;
	std::unique_ptr<HeartControl::EventManager> mEventManager; // Changed to pointer

	LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static bool CreateWindowClass(HINSTANCE hInstance);
	HWND& CreateWindows(HINSTANCE hInstance);
public:
	WindowApp() : m_hWnd(nullptr), m_hInstance(nullptr) {}
	~WindowApp() = default;

	// Public accessor for the event manager
	// Initialize the event manager once we have a valid window handle
	void InitializeEventManager()
	{
		if (!mEventManager && m_hWnd != nullptr) {
			mEventQueue.startProcessing(mEventDispatcher);
			mEventManager = std::make_unique<HeartControl::EventManager>(m_hWnd, mEventDispatcher);
		}
	}

	// Public accessor for the event manager - return by reference
	[[nodiscard]] HeartControl::EventManager& GetEventManager() const
	{
		if (!mEventManager) {
			throw std::runtime_error("EventManager not initialized");
		}
		return *mEventManager;
	}

	HeartControl::EventQueue& GetEventQueue();
	static HRESULT TriggerEvent(const HeartControl::UIEvent& evt);
	// Static accessor if needed from static contexts
	static WindowApp* GetInstance();

	bool CreateViews(HINSTANCE& hInstance);
	int Run(HINSTANCE hInstance, int nShowCmd);
};

