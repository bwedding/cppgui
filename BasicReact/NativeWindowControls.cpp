#include "NativeWindowControls.h"
#include "INativeWindowControls.h"
#include <shobjidl.h> // For modern file dialogs
#include <wrl/implements.h>
#include <wrl/client.h>
#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"
#include "SystemUtils.h"

using namespace Microsoft::WRL;
using json = nlohmann::json;

STDMETHODIMP NativeWindowControls::GetTypeInfoCount(UINT* pctinfo)
{
    *pctinfo = 1;
    return S_OK;
}

STDMETHODIMP NativeWindowControls::GetTypeInfo(const UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
     if (0 != iTInfo)
    {
         return TYPE_E_ELEMENTNOTFOUND;
    }
    if (!m_typeLib)
    {
         RETURN_IF_FAILED(LoadTypeLib(L"NativeWindowControls.tlb", &m_typeLib));
    }
     return m_typeLib->GetTypeInfoOfGuid(__uuidof(INativeWindowControls), ppTInfo);
}

STDMETHODIMP NativeWindowControls::GetIDsOfNames(
    REFIID riid, LPOLESTR* rgszNames, const UINT cNames, const LCID lcid, DISPID* rgDispId)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    auto typeinfo = typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
    return typeinfo;
}

STDMETHODIMP NativeWindowControls::Invoke(
    DISPID dispIdMember, REFIID riid, const LCID lcid, const WORD wFlags, DISPPARAMS* pDispParams,
    VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));

    if (pDispParams) 
    {
        LOGI << "Args count: {}" << pDispParams->cArgs;
        if (pDispParams->cArgs > 0) 
        {
            LOGI << "First arg type: {}" << pDispParams->rgvarg[0].vt;
        }
    }

    if (pDispParams && pDispParams->cArgs > 0)
    {
        if (pDispParams->rgvarg[0].vt != VT_BSTR)
        {
            LOGD <<"Exiting";
            return E_FAIL;
        }
    }

    TYPEATTR* pTypeAttr = nullptr;

    const auto result = typeInfo->Invoke(
        this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);

    if (result != S_OK)
    {
        spdlog::error("Exception in typeInfo->Invoke");
        //SPDLOG_TRACE("Exiting");
        return E_FAIL;
    }
    return S_OK;
}

// Window control methods
STDMETHODIMP NativeWindowControls::SendClick(const BSTR jsonData)
{

    if (!jsonData) return E_INVALIDARG;

    std::wstring wstr(jsonData);
    std::string str(wstr.begin(), wstr.end());
    // Parse the JSON data
    const auto json = nlohmann::json::parse(str);

    spdlog::trace(json.dump());
    // TODO add code to populate event with actual data
    CPPGUI::UIEvent evt{
        "auto-manual-control",
        "User interface",
        str,
        system_clock::time_point{}
    };

    const int eventId = mEventManager->registerEvent(std::move(evt));
    LOGI << "Registered event with ID: " << eventId << ", type: 'auto-manual-control'";

    PostMessage(hwnd, WM_USER_EVENT, 0, eventId);
    LOGI << "Posted WM_USER_EVENT message with event ID: " << eventId;

    return S_OK;     // Return success for the method call itself
}

STDMETHODIMP NativeWindowControls::SendForm(const BSTR jsonData)
{

    SPDLOG_TRACE("Entering");
    if (!jsonData) return E_INVALIDARG;

    std::wstring wstr(jsonData);
    std::string str(wstr.begin(), wstr.end());
    // Parse the JSON data
    const auto json = nlohmann::json::parse(str);

    spdlog::trace(json.dump());
    CPPGUI::UIEvent evt{
        "auto-manual-control",
        "User interface",
        str,
        system_clock::time_point{}
    };

    const int eventId = mEventManager->registerEvent(std::move(evt));
    LOGI << "Registered event with ID: " << eventId << ", type: 'auto-manual-control'";

    PostMessage(hwnd, WM_USER_EVENT, 0, eventId);
    LOGI << "Posted WM_USER_EVENT message with event ID: " << eventId;

    return S_OK;     // Return success for the method call itself
}

STDMETHODIMP  NativeWindowControls::MinimizeWindow()
{
    if (!IsWindow(hwnd))
        return E_FAIL;
    return ShowWindow(hwnd, SW_MINIMIZE) ? S_OK : E_FAIL;
}

STDMETHODIMP  NativeWindowControls::MaximizeWindow()
{
    if (!IsWindow(hwnd))
        return E_FAIL;

    // Check if window is already maximized
    WINDOWPLACEMENT wp = { sizeof(wp) };
    GetWindowPlacement(hwnd, &wp);

    // Toggle between maximized and restored state
    if (wp.showCmd == SW_MAXIMIZE)
    {
        return ShowWindow(hwnd, SW_RESTORE) ? S_OK : E_FAIL;
    }
    return ShowWindow(hwnd, SW_MAXIMIZE) ? S_OK : E_FAIL;
}

STDMETHODIMP  NativeWindowControls::CloseWindow() 
{
    if (!IsWindow(hwnd)) return E_FAIL;
    return PostMessage(hwnd, WM_CLOSE, 0, 0) ? S_OK : E_FAIL;
}

STDMETHODIMP  NativeWindowControls::StartWindowDrag()
{
    if (!IsWindow(hwnd))
        return E_FAIL;

    ReleaseCapture();
    PostMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
    return S_OK;
}

STDMETHODIMP  NativeWindowControls::FileOpenDialog(BSTR* pVarResult)
{
    IFileOpenDialog* pFileOpen;
    std::wstring selectedFile;

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
        IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if (SUCCEEDED(hr))
    {
        hr = pFileOpen->Show(hwnd);

        if (SUCCEEDED(hr))
        {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR filePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
                if (SUCCEEDED(hr) && pVarResult)
                {
                    // Get file info
                    const HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ,
                                                    nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

                    if (hFile != INVALID_HANDLE_VALUE)
                    {
                        // Get file size
                        LARGE_INTEGER fileSize;
                        if (!GetFileSizeEx(hFile, &fileSize)) {
                            CloseHandle(hFile);
                            *pVarResult = SysAllocString(L"Failed to get file size");
                            return E_FAIL;
                        }

                        // Read first few bytes to detect file type
                        unsigned char buffer[1024];
                        DWORD bytesRead;
                        if (!ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, nullptr)) {
                            CloseHandle(hFile);
                            *pVarResult = SysAllocString(L"Failed to read file");
                            return E_FAIL;
                        }

                        // Create JSON response
                        nlohmann::json response;
                        std::wstring wFilePath(filePath);
                        response["filePath"] = std::string(wFilePath.begin(), wFilePath.end());
                        response["size"] = fileSize.QuadPart;

                        // Simple file type detection
                        bool isTextFile = true;
                        for (DWORD i = 0; i < bytesRead && i < 1024; i++) {
                            if (buffer[i] == 0 || (buffer[i] < 32 && buffer[i] != '\n' && buffer[i] != '\r' && buffer[i] != '\t')) {
                                isTextFile = false;
                                break;
                            }
                        }

                        if (isTextFile && fileSize.QuadPart < 1024 * 1024)
                        {
                            // Only load content for text files under 1MB
                            // Read text content
                            std::string content = ReadFileContent(wFilePath);
                            response["content"] = content;
                            response["type"] = "text";
                        }
                        else
                        {
                            response["type"] = "binary";
                            response["preview"] = "Binary file - content not displayed";
                        }

                        CloseHandle(hFile);
                        // Convert to BSTR and set result
                        std::string jsonResponse = response.dump();
                        *pVarResult = SysAllocString(std::wstring(jsonResponse.begin(), jsonResponse.end()).c_str());
                        return S_OK;
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
    }
    return hr;
}

STDMETHODIMP  NativeWindowControls::FileSaveDialog(BSTR* pVarResult)
{
    IFileSaveDialog* pFileSave;
    std::wstring selectedFile;

    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL,
        IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

    if (SUCCEEDED(hr))
    {
        hr = pFileSave->Show(hwnd);

        if (SUCCEEDED(hr))
        {
            IShellItem* pItem;
            hr = pFileSave->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR filePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
                if (SUCCEEDED(hr))
                {
                    selectedFile = filePath;
                    CoTaskMemFree(filePath);
                }
                pItem->Release();
            }
        }
        pFileSave->Release();
    }

    if (SUCCEEDED(hr) && pVarResult) 
    {
        *pVarResult = SysAllocString(selectedFile.c_str());
    }
    return hr;
}

STDMETHODIMP  NativeWindowControls::BrowseForFolder(BSTR* pVarResult)
{
    IFileOpenDialog* pFolderDialog;
    std::wstring selectedFolder;

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
        IID_IFileOpenDialog, reinterpret_cast<void**>(&pFolderDialog));

    if (SUCCEEDED(hr))
    {
        // Set the dialog to select folders
        FILEOPENDIALOGOPTIONS options;
        hr = pFolderDialog->GetOptions(&options);
        if (SUCCEEDED(hr))
        {
            hr = pFolderDialog->SetOptions(options | FOS_PICKFOLDERS);
            if (SUCCEEDED(hr))
            {
                hr = pFolderDialog->Show(hwnd);
                if (SUCCEEDED(hr))
                {
                    IShellItem* pItem;
                    hr = pFolderDialog->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        PWSTR folderPath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &folderPath);
                        if (SUCCEEDED(hr))
                        {
                            selectedFolder = folderPath;
                            CoTaskMemFree(folderPath);
                        }
                        pItem->Release();
                    }
                }
            }
        }
        pFolderDialog->Release();
    }
    if (SUCCEEDED(hr) && pVarResult) {
        *pVarResult = SysAllocString(selectedFolder.c_str());
    }
    return hr;
}

// Alias for BrowseForFolder for consistency
STDMETHODIMP  NativeWindowControls::OpenFolderDialog(BSTR* pVarResult)
{
    return BrowseForFolder(pVarResult);
}

// Helper function to convert PWSTR to VARIANT
void NativeWindowControls::SetStringResult(VARIANT* pVarResult, const std::wstring& str)
{
    if (pVarResult) {
        pVarResult->vt = VT_BSTR;
        pVarResult->bstrVal = SysAllocString(str.c_str());
    }
    LOGD << "Exiting";
}

std::string NativeWindowControls::ReadFileContent(const std::wstring& wFilePath) {
    // Open the file
    const HANDLE hFile = CreateFileW(
        wFilePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open file");
    }

    // Get file size
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to get file size");
    }

    // Ensure file size is not too large
    if (fileSize.QuadPart > (1LL << 31)) {  // 2GB limit
        CloseHandle(hFile);
        throw std::runtime_error("File is too large");
    }

    // Create string with required size
    std::string content;
    content.resize(static_cast<size_t>(fileSize.QuadPart));

    // Read file content
    DWORD bytesRead;
    if (!ReadFile(
        hFile,
        &content[0],
        static_cast<DWORD>(fileSize.QuadPart),
        &bytesRead,
        nullptr
    )) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to read file");
    }

    // Close file handle
    CloseHandle(hFile);

    // Verify all bytes were read
    if (bytesRead != fileSize.QuadPart) {
        throw std::runtime_error("Failed to read entire file");
    }

    return content;
}
