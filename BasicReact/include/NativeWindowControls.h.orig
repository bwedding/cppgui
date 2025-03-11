#pragma once
#include <windows.h>
#include <string>
#include "INativeWindowControls.h"
#include <wil/com.h>
#include <wrl/implements.h>
#include "spdlog.h"
#include "nlohmann/json.hpp"

using namespace Microsoft::WRL;
using json = nlohmann::json;

class NativeWindowControls final : public RuntimeClass<
		RuntimeClassFlags<ClassicCom>, INativeWindowControls, IDispatch>
{
    HWND hwnd;
    std::mutex m_connectionMutex;

    wil::com_ptr<ITypeLib> m_typeLib;

public:
    explicit NativeWindowControls(const HWND window) : hwnd(window) { SPDLOG_TRACE("Entering"); }

    STDMETHODIMP SendClick(BSTR jsonData) override;

    STDMETHODIMP SendForm(BSTR jsonData) override;
 
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;

    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;

    STDMETHODIMP GetIDsOfNames(
        REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;
    STDMETHODIMP Invoke(
        DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
        VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;

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
};
