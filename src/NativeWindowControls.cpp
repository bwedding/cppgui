#include "NativeWindowControls.h"
#include "INativeWindowControls.h"
#include <shobjidl.h> // For modern file dialogs
#include <wrl/implements.h>
#include <wrl/client.h>
#include "spdlog.h"
#include "nlohmann/json.hpp"
#include "SystemUtils.h"
#include "WindowApp.h"

using namespace Microsoft::WRL;
using json = nlohmann::json;

const std::string jsonData = R"(
[
  {
    "_id": "67bd8b53bf59d57fa64beff7",
    "index": 0,
    "guid": "29fe39b0-09ad-48f8-9656-5d47fc38f5ec",
    "isActive": true,
    "balance": "$1,536.95",
    "picture": "http://placehold.it/32x32",
    "age": 30,
    "eyeColor": "green",
    "name": "Santiago Mccormick",
    "gender": "male",
    "company": "GRACKER",
    "email": "santiagomccormick@gracker.com",
    "phone": "+1 (832) 582-2776",
    "address": "766 Hegeman Avenue, Mansfield, Washington, 3542",
    "about": "Ea elit pariatur ut Lorem nisi dolor anim qui minim adipisicing eiusmod eiusmod labore. Velit et deserunt esse consectetur aliqua laborum ut duis consectetur ipsum. Deserunt occaecat quis voluptate ut do laborum magna fugiat nulla. Eu cillum excepteur labore sint exercitation est. Eu reprehenderit labore culpa nostrud nulla ut deserunt anim sint officia qui.\r\n",
    "registered": "2018-12-09T12:06:56 -01:00",
    "latitude": -60.682135,
    "longitude": 125.722754,
    "tags": [
      "consequat",
      "fugiat",
      "pariatur",
      "quis",
      "reprehenderit",
      "labore",
      "consequat"
    ],
    "friends": [
      {
        "id": 0,
        "name": "Lucille Sellers"
      },
      {
        "id": 1,
        "name": "Darlene Bender"
      },
      {
        "id": 2,
        "name": "Gertrude Nichols"
      }
    ],
    "greeting": "Hello, Santiago Mccormick! You have 2 unread messages.",
    "favoriteFruit": "apple"
  },
  {
    "_id": "67bd8b537fdce7afd40d7e47",
    "index": 1,
    "guid": "5524cff5-d71b-4f1a-9d27-1758493a3193",
    "isActive": true,
    "balance": "$1,653.53",
    "picture": "http://placehold.it/32x32",
    "age": 23,
    "eyeColor": "blue",
    "name": "Johnnie Tillman",
    "gender": "female",
    "company": "RODEOLOGY",
    "email": "johnnietillman@rodeology.com",
    "phone": "+1 (808) 499-2944",
    "address": "932 Seagate Avenue, Baker, Alaska, 7642",
    "about": "Voluptate ut mollit esse sint fugiat voluptate ipsum commodo laboris consequat consectetur velit enim consequat. Deserunt in est incididunt proident do eu voluptate id cupidatat voluptate excepteur cillum commodo. Nisi commodo aliqua excepteur ut ullamco sint adipisicing cillum exercitation nulla anim adipisicing. Sint eu velit consequat pariatur duis. Laboris reprehenderit aliquip sunt pariatur.\r\n",
    "registered": "2020-12-05T08:52:00 -01:00",
    "latitude": -10.826358,
    "longitude": -52.558178,
    "tags": [
      "et",
      "veniam",
      "voluptate",
      "id",
      "laboris",
      "ad",
      "est"
    ],
    "friends": [
      {
        "id": 0,
        "name": "Janelle Ewing"
      },
      {
        "id": 1,
        "name": "Stacie Rocha"
      },
      {
        "id": 2,
        "name": "Green Horn"
      }
    ],
    "greeting": "Hello, Johnnie Tillman! You have 3 unread messages.",
    "favoriteFruit": "banana"
  },
  {
    "_id": "67bd8b53bb158722a59ee6e9",
    "index": 2,
    "guid": "63daf5d1-1fe4-467c-a1cb-972753dcb259",
    "isActive": false,
    "balance": "$3,683.59",
    "picture": "http://placehold.it/32x32",
    "age": 39,
    "eyeColor": "brown",
    "name": "Estela Decker",
    "gender": "female",
    "company": "EXERTA",
    "email": "esteladecker@exerta.com",
    "phone": "+1 (815) 587-2682",
    "address": "581 Stuart Street, Hegins, South Carolina, 3638",
    "about": "Elit do labore esse nostrud voluptate. Cupidatat velit incididunt exercitation eu ad duis commodo esse sunt. Incididunt ullamco enim occaecat ullamco sint magna mollit duis velit aute ad nisi fugiat. Et dolor cillum excepteur commodo reprehenderit incididunt duis amet cupidatat ut incididunt do quis quis.\r\n",
    "registered": "2018-03-21T07:02:25 -01:00",
    "latitude": 9.45801,
    "longitude": -179.406957,
    "tags": [
      "ea",
      "nulla",
      "sunt",
      "ea",
      "consectetur",
      "consequat",
      "est"
    ],
    "friends": [
      {
        "id": 0,
        "name": "Montoya Emerson"
      },
      {
        "id": 1,
        "name": "Maureen Booker"
      },
      {
        "id": 2,
        "name": "Noelle Herring"
      }
    ],
    "greeting": "Hello, Estela Decker! You have 7 unread messages.",
    "favoriteFruit": "banana"
  },
  {
    "_id": "67bd8b533d14c16ee88408a2",
    "index": 3,
    "guid": "f7cf46be-bd17-4523-a420-e7df753a30cf",
    "isActive": true,
    "balance": "$2,787.86",
    "picture": "http://placehold.it/32x32",
    "age": 29,
    "eyeColor": "blue",
    "name": "Gracie Calderon",
    "gender": "female",
    "company": "PHOTOBIN",
    "email": "graciecalderon@photobin.com",
    "phone": "+1 (914) 492-3631",
    "address": "336 Lefferts Avenue, Hondah, Rhode Island, 8821",
    "about": "Ut nostrud aute anim incididunt minim incididunt aute adipisicing proident ex incididunt veniam incididunt. Duis sunt cupidatat dolor duis enim magna est commodo fugiat quis. Laboris in qui pariatur ea commodo proident consequat in excepteur velit incididunt laborum exercitation. Aliqua est dolor proident voluptate laboris aliqua dolore Lorem anim nisi proident. Ea deserunt sint ad veniam commodo nostrud commodo exercitation ullamco nisi nisi.\r\n",
    "registered": "2020-10-26T08:34:21 -01:00",
    "latitude": 34.014758,
    "longitude": 164.76642,
    "tags": [
      "reprehenderit",
      "adipisicing",
      "aliqua",
      "aute",
      "proident",
      "et",
      "laborum"
    ],
    "friends": [
      {
        "id": 0,
        "name": "Marisa Green"
      },
      {
        "id": 1,
        "name": "Kane Stein"
      },
      {
        "id": 2,
        "name": "Ross Justice"
      }
    ],
    "greeting": "Hello, Gracie Calderon! You have 8 unread messages.",
    "favoriteFruit": "strawberry"
  },
  {
    "_id": "67bd8b5320373aed8b7df6e7",
    "index": 4,
    "guid": "9889b09d-fdc4-41a1-94da-e13be6a8c68d",
    "isActive": false,
    "balance": "$3,669.03",
    "picture": "http://placehold.it/32x32",
    "age": 38,
    "eyeColor": "blue",
    "name": "Adriana Campbell",
    "gender": "female",
    "company": "ZENTHALL",
    "email": "adrianacampbell@zenthall.com",
    "phone": "+1 (941) 527-3026",
    "address": "704 Bills Place, Kenwood, New Mexico, 8277",
    "about": "Pariatur nisi sit esse occaecat duis nostrud exercitation occaecat aliquip tempor eiusmod. Cupidatat dolore ipsum ullamco voluptate sunt proident enim culpa veniam nisi id. Irure Lorem deserunt aliqua ipsum pariatur sit exercitation.\r\n",
    "registered": "2024-08-17T11:39:28 -02:00",
    "latitude": -85.817847,
    "longitude": 0.935917,
    "tags": [
      "quis",
      "fugiat",
      "do",
      "fugiat",
      "dolor",
      "ut",
      "aliquip"
    ],
    "friends": [
      {
        "id": 0,
        "name": "Alba Marks"
      },
      {
        "id": 1,
        "name": "Sonya Larson"
      },
      {
        "id": 2,
        "name": "Elisabeth Haney"
      }
    ],
    "greeting": "Hello, Adriana Campbell! You have 8 unread messages.",
    "favoriteFruit": "strawberry"
  },
  {
    "_id": "67bd8b53c042fbb251b4e064",
    "index": 5,
    "guid": "3cae9a90-c432-47f8-836e-f124aee0c63f",
    "isActive": false,
    "balance": "$2,582.34",
    "picture": "http://placehold.it/32x32",
    "age": 23,
    "eyeColor": "green",
    "name": "Mcgee Rogers",
    "gender": "male",
    "company": "XANIDE",
    "email": "mcgeerogers@xanide.com",
    "phone": "+1 (814) 597-2263",
    "address": "607 Homecrest Avenue, Holcombe, Kansas, 5273",
    "about": "Fugiat fugiat exercitation laboris non qui ea consequat aute nostrud laboris minim ipsum velit nulla. Esse ullamco aute velit quis proident do. Reprehenderit tempor velit deserunt irure ea ullamco aliqua. Ad laborum aliquip ipsum ad anim minim sunt officia.\r\n",
    "registered": "2024-08-22T04:56:28 -02:00",
    "latitude": 44.280707,
    "longitude": -32.541756,
    "tags": [
      "officia",
      "sit",
      "in",
      "do",
      "dolor",
      "commodo",
      "consequat"
    ],
    "friends": [
      {
        "id": 0,
        "name": "Sasha Harrell"
      },
      {
        "id": 1,
        "name": "Tyson Hodges"
      },
      {
        "id": 2,
        "name": "Carolyn Carey"
      }
    ],
    "greeting": "Hello, Mcgee Rogers! You have 3 unread messages.",
    "favoriteFruit": "apple"
  },
  {
    "_id": "67bd8b53400f956fc8c3926b",
    "index": 6,
    "guid": "c0b538de-1d68-46e9-a0f5-df81a763b7e6",
    "isActive": false,
    "balance": "$3,703.33",
    "picture": "http://placehold.it/32x32",
    "age": 24,
    "eyeColor": "brown",
    "name": "Boyer Bond",
    "gender": "male",
    "company": "ZILENCIO",
    "email": "boyerbond@zilencio.com",
    "phone": "+1 (909) 497-3315",
    "address": "463 Shale Street, Malott, Texas, 5902",
    "about": "Nostrud tempor pariatur aliqua occaecat voluptate consequat duis adipisicing aliquip ipsum exercitation. Eu labore aute ad aliqua ullamco non aliqua do nostrud nisi. Aliquip velit cupidatat incididunt nulla quis est irure irure occaecat. Nostrud id irure cillum laboris officia labore incididunt consectetur commodo id minim ea sunt. Sint enim velit veniam occaecat minim ullamco amet enim nulla. Voluptate et reprehenderit cupidatat in laboris ullamco. Esse commodo incididunt elit id et nostrud Lorem id laborum.\r\n",
    "registered": "2018-11-10T10:10:57 -01:00",
    "latitude": -19.672876,
    "longitude": 109.827223,
    "tags": [
      "anim",
      "nulla",
      "veniam",
      "est",
      "excepteur",
      "consequat",
      "tempor"
    ],
    "friends": [
      {
        "id": 0,
        "name": "Higgins Watson"
      },
      {
        "id": 1,
        "name": "Gentry Henry"
      },
      {
        "id": 2,
        "name": "Reynolds Acosta"
      }
    ],
    "greeting": "Hello, Boyer Bond! You have 7 unread messages.",
    "favoriteFruit": "apple"
  }
])";

STDMETHODIMP NativeWindowControls::GetTypeInfoCount(UINT* pctinfo)
{
    *pctinfo = 1;
    return S_OK;
}

STDMETHODIMP NativeWindowControls::GetTypeInfo(const UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
    //SPDLOG_TRACE("Entering");
    if (0 != iTInfo)
    {
        //SPDLOG_TRACE("Exiting");
        return TYPE_E_ELEMENTNOTFOUND;
    }
    if (!m_typeLib)
    {
        //SPDLOG_TRACE("Exiting");
        RETURN_IF_FAILED(LoadTypeLib(L"NativeWindowControls.tlb", &m_typeLib));
    }
    //SPDLOG_TRACE("Exiting");
    return m_typeLib->GetTypeInfoOfGuid(__uuidof(INativeWindowControls), ppTInfo);
}

STDMETHODIMP NativeWindowControls::GetIDsOfNames(
    REFIID riid, LPOLESTR* rgszNames, const UINT cNames, const LCID lcid, DISPID* rgDispId)
{
    //SPDLOG_TRACE("Entering");
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    auto typeinfo = typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
    //SPDLOG_TRACE("Exiting");
    return typeinfo;
}

STDMETHODIMP NativeWindowControls::Invoke(
    DISPID dispIdMember, REFIID riid, const LCID lcid, const WORD wFlags, DISPPARAMS* pDispParams,
    VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{

    OutputDebugString(L"Invoke called with dispid: ");
    wchar_t buffer[16];
    _itow_s(dispIdMember, buffer, 10);
    OutputDebugString(buffer);
    OutputDebugString(L"\n");

    //SPDLOG_TRACE("Entering");
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));

    if (pDispParams) {
        spdlog::info("Args count: {}", pDispParams->cArgs);
        if (pDispParams->cArgs > 0) {
            spdlog::info("First arg type: {}", pDispParams->rgvarg[0].vt);
        }
    }

    if (pDispParams && pDispParams->cArgs > 0)
    {
        if (pDispParams->rgvarg[0].vt != VT_BSTR)
        {
            SPDLOG_TRACE("Exiting");
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
STDMETHODIMP  NativeWindowControls::MinimizeWindow()
{
    SPDLOG_TRACE("Entering");
    if (!IsWindow(hwnd))
        return E_FAIL;
    SPDLOG_TRACE("Exiting");
    return ShowWindow(hwnd, SW_MINIMIZE) ? S_OK : E_FAIL;
}

STDMETHODIMP  NativeWindowControls::MaximizeWindow()
{
    SPDLOG_TRACE("Entering");
    if (!IsWindow(hwnd))
        return E_FAIL;

    // Check if window is already maximized
    WINDOWPLACEMENT wp = { sizeof(wp) };
    GetWindowPlacement(hwnd, &wp);

    // Toggle between maximized and restored state
    if (wp.showCmd == SW_MAXIMIZE)
    {
        SPDLOG_TRACE("Exiting");
        return ShowWindow(hwnd, SW_RESTORE) ? S_OK : E_FAIL;
    }
    SPDLOG_TRACE("Exiting");
    return ShowWindow(hwnd, SW_MAXIMIZE) ? S_OK : E_FAIL;
}

STDMETHODIMP  NativeWindowControls::CloseWindow() 
{
    SPDLOG_TRACE("Entering");
    if (!IsWindow(hwnd)) return E_FAIL;
    SPDLOG_TRACE("Exiting");
    return PostMessage(hwnd, WM_CLOSE, 0, 0) ? S_OK : E_FAIL;
}

STDMETHODIMP  NativeWindowControls::StartWindowDrag()
{
    SPDLOG_TRACE("Entering");
    if (!IsWindow(hwnd))
        return E_FAIL;

    ReleaseCapture();
    PostMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
    SPDLOG_TRACE("Exiting");
    return S_OK;
}

STDMETHODIMP  NativeWindowControls::FileOpenDialog(BSTR* pVarResult)
{
    SPDLOG_TRACE("Entering");
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
    SPDLOG_TRACE("Exiting");
    return hr;
}

STDMETHODIMP  NativeWindowControls::FileSaveDialog(BSTR* pVarResult)
{
    SPDLOG_TRACE("Entering");
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

    if (SUCCEEDED(hr) && pVarResult) {
        *pVarResult = SysAllocString(selectedFile.c_str());
    }
    SPDLOG_TRACE("Exiting");
    return hr;
}

STDMETHODIMP  NativeWindowControls::BrowseForFolder(BSTR* pVarResult)
{
    SPDLOG_TRACE("Entering");
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
    SPDLOG_TRACE("Exiting");
    return hr;
}

// Alias for BrowseForFolder for consistency
STDMETHODIMP  NativeWindowControls::OpenFolderDialog(BSTR* pVarResult)
{
    SPDLOG_TRACE("Entering");
    return BrowseForFolder(pVarResult);
}

STDMETHODIMP NativeWindowControls::SendClick(const BSTR jsonData)
{

    SPDLOG_TRACE("Entering");
    if (!jsonData) return E_INVALIDARG;

    std::wstring wstr(jsonData);
    std::string str(wstr.begin(), wstr.end());
    // Parse the JSON data
    const auto json = nlohmann::json::parse(str);

    spdlog::trace(json.dump());
    HeartControl::UIEvent evt{
        "auto-manual-control",
        "User interface",
        str,
        system_clock::time_point{}
    };
    const WindowApp* mWinApp = WindowApp::GetInstance();

    auto &mEvtMgr = mWinApp->GetEventManager();

    const int eventId = mEvtMgr.registerEvent(std::move(evt));

    PostMessage(hwnd, WM_USER_EVENT, 0, eventId);

    SPDLOG_TRACE("Exiting");
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
    HeartControl::UIEvent evt{
        "auto-manual-control",
        "User interface",
        str,
        system_clock::time_point{}
    };
    const WindowApp* mWinApp = WindowApp::GetInstance();

    auto& mEvtMgr = mWinApp->GetEventManager();

    const int eventId = mEvtMgr.registerEvent(std::move(evt));

    PostMessage(hwnd, WM_USER_EVENT, 0, eventId);

    SPDLOG_TRACE("Exiting");
    return S_OK;     // Return success for the method call itself
}

// Helper function to convert PWSTR to VARIANT
void NativeWindowControls::SetStringResult(VARIANT* pVarResult, const std::wstring& str)
{
    if (pVarResult) {
        pVarResult->vt = VT_BSTR;
        pVarResult->bstrVal = SysAllocString(str.c_str());
    }
    SPDLOG_TRACE("Exiting");
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
