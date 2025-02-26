#pragma once
#include <windows.h>
#include <objbase.h>

// Interface ID (use uuidgen to generate a new one)
// {2B442739-0920-4489-A943-B6AA7573D4D8}
interface __declspec(uuid("bf3778c3-a587-4ad3-afc3-e3bef12d301d")) INativeWindowControls : IDispatch
{
	virtual HRESULT STDMETHODCALLTYPE MinimizeWindow() = 0;
    virtual HRESULT STDMETHODCALLTYPE MaximizeWindow() = 0;
    virtual HRESULT STDMETHODCALLTYPE CloseWindow() = 0;
    virtual HRESULT STDMETHODCALLTYPE StartWindowDrag() = 0;
    virtual HRESULT STDMETHODCALLTYPE FileOpenDialog(BSTR* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE FileSaveDialog(BSTR* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE BrowseForFolder(BSTR* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE OpenFolderDialog(BSTR* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendClick(BSTR jsonData) = 0;
};