#pragma once
#include <windows.h>
#include <string>
#include <mutex>
#include "INativeWindowControls.h"
#include <wil/com.h>
#include <wrl/implements.h>
#include "nlohmann/json.hpp"
#include "UIEvent.h"
#include "EventManager.h"
#include "AppMessageIDs.h"
#include <plog/Log.h> 

using namespace Microsoft::WRL;
using json = nlohmann::json;

class NativeWindowControls final : public RuntimeClass<
		RuntimeClassFlags<ClassicCom>, INativeWindowControls, IDispatch>
{
    HWND hwnd;
    std::unique_ptr<CPPGUI::EventManager> mEventManager;
    CPPGUI::EventDispatcher mevtDispatcher;
    CPPGUI::EventQueue mEventQueue;
    std::mutex m_connectionMutex;

    wil::com_ptr<ITypeLib> m_typeLib;
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;

    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;

    STDMETHODIMP GetIDsOfNames(
        REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;
    STDMETHODIMP Invoke(
        DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
        VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;

public:
    explicit NativeWindowControls(const HWND window ) : hwnd(window)
    { 
        mEventManager = std::make_unique<CPPGUI::EventManager>(window, mevtDispatcher);
        // Start the event queue processing with the event dispatcher
        mEventQueue.startProcessing(mevtDispatcher);
        LOGD << "Entering"; 
    }

    STDMETHODIMP SendClick(BSTR jsonData) override;
    STDMETHODIMP SendForm(BSTR jsonData) override;

    // Window control methods
    STDMETHODIMP  MinimizeWindow() override;
    STDMETHODIMP  MaximizeWindow() override;
    STDMETHODIMP  CloseWindow() override;
    STDMETHODIMP  StartWindowDrag() override;
    STDMETHODIMP  FileOpenDialog(BSTR* pVarResult) override;
    STDMETHODIMP  FileSaveDialog(BSTR* pVarResult) override;
    STDMETHODIMP  BrowseForFolder(BSTR* pVarResult) override;
    STDMETHODIMP  OpenFolderDialog(BSTR* pVarResult) override;
    static void SetStringResult(VARIANT* pVarResult, const std::wstring& str);
    static std::string ReadFileContent(const std::wstring& wFilePath);

    // Getter methods for event-related components
    CPPGUI::EventManager* GetEventManager() const { return mEventManager.get(); }
    CPPGUI::EventDispatcher* GetEventDispatcher() { return &mevtDispatcher; }
    CPPGUI::EventQueue* GetEventQueue() { return &mEventQueue; }
};
