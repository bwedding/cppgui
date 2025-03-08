#pragma once
#include <windows.h>
#include <lmcons.h>
#include "WebView2.h"
#include <string>
#include "spdlog/spdlog.h"
#include <nlohmann/json.hpp>
#include <shobjidl_core.h>
#include <shlobj.h>
#include <comdef.h>
#include <pdh.h>
#include <wrl/implements.h>
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "advapi32.lib")

using namespace Microsoft::WRL;
using json = nlohmann::json;

struct EndpointConfig {
    std::wstring endpoint;
    std::function<std::string(const json&)> handler;
};


class ApiBridge final : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDispatch>
{
    std::vector<EndpointConfig> endpoints;
    ComPtr<ICoreWebView2> webview = nullptr;
public:
    explicit ApiBridge(const ComPtr<ICoreWebView2> wv) : webview(wv) {}

    // IUnknown methods
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override {
        if (riid == IID_IUnknown || riid == IID_IDispatch) {
            *ppvObject = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG __stdcall AddRef() override {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG __stdcall Release() override {
	    const ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

    // IDispatch methods
    HRESULT __stdcall GetTypeInfoCount(UINT* pctinfo) override {
        *pctinfo = 0;
        return S_OK;
    }

    HRESULT __stdcall GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override {
        *ppTInfo = nullptr;
        return E_NOTIMPL;
    }

    HRESULT __stdcall GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override {
        if (wcscmp(rgszNames[0], L"getData") == 0) {
            *rgDispId = 1;
            return S_OK;
        }
        return DISP_E_UNKNOWNNAME;
    }

    HRESULT __stdcall Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override {
        if (dispIdMember == 1) {
            if (pDispParams->cArgs != 1) {
                return DISP_E_BADPARAMCOUNT;
            }

            // Extract and parse the JSON string argument
            BSTR jsonString = pDispParams->rgvarg[0].bstrVal;
            std::wstring wJsonString(jsonString, SysStringLen(jsonString));
            std::string jsonStringStr(wJsonString.begin(), wJsonString.end());

            // Parse JSON string
            json requestPayload = json::parse(jsonStringStr);

            // Convert endpoint to wstring
            auto endpointStr = requestPayload["endpoint"].get<std::string>();
            std::wstring endpoint(endpointStr.begin(), endpointStr.end());

            if (auto it = std::ranges::find_if(endpoints,
                                               [&endpoint](const auto& config) {
	                                               return endpoint == config.endpoint;
                                               }); it != endpoints.end()) {
                std::string jsonResponse = it->handler(requestPayload);
                pVarResult->vt = VT_BSTR;
                pVarResult->bstrVal = SysAllocString(std::wstring(jsonResponse.begin(), jsonResponse.end()).c_str());
                return S_OK;
            }

            if (endpoint == L"/api/users") {
                auto userData = "Bruce Wedding";//model.getUsers();
                json jsonData = userData;
                std::string jsonResponse = jsonData.dump();

                // Allocate and set the response string
                pVarResult->vt = VT_BSTR;
                pVarResult->bstrVal = SysAllocString(std::wstring(jsonResponse.begin(), jsonResponse.end()).c_str());
                return S_OK;
            }

            //if (endpoint == L"/api/loggedInUserInfo") {
            //    json userInfo = GetLoggedInUserInfo();
            //    std::string jsonResponse = userInfo.dump();

            //    pVarResult->vt = VT_BSTR;
            //    pVarResult->bstrVal = SysAllocString(std::wstring(jsonResponse.begin(), jsonResponse.end()).c_str());
            //    return S_OK;
            //}

            if (endpoint == L"/api/submitForm") {
                auto postData = requestPayload["postData"];
                std::string username = postData["username"];
                std::string password = postData["password"];

                // Process the post data (e.g., authenticate user)
                std::string jsonResponse = "User authenticated";

                pVarResult->vt = VT_BSTR;
                pVarResult->bstrVal = SysAllocString(std::wstring(jsonResponse.begin(), jsonResponse.end()).c_str());
                return S_OK;
            }

            if (endpoint == L"/api/saveFileDialog") {
                std::wstring filePath = SaveFileDialog();
                auto jsonResponse = std::string(filePath.begin(), filePath.end());

                pVarResult->vt = VT_BSTR;
                pVarResult->bstrVal = SysAllocString(std::wstring(jsonResponse.begin(), jsonResponse.end()).c_str());
                return S_OK;
            }

            if (endpoint == L"/api/openFileDialog") {
                std::wstring filePath = OpenFileDialog();
                std::string fileContent = ReadFileContent(filePath);
                auto divName = requestPayload["divName"].get<std::string>();
                std::string script = "document.getElementById('" + divName + "').innerText = `" + fileContent + "`;";

                if (webview) {
                    webview->ExecuteScript(std::wstring(script.begin(), script.end()).c_str(), nullptr);
                }

                json response;
                response["filePath"] = std::string(filePath.begin(), filePath.end());
                response["fileContent"] = fileContent;

                std::string jsonResponse = response.dump();
                pVarResult->vt = VT_BSTR;
                pVarResult->bstrVal = SysAllocString(std::wstring(jsonResponse.begin(), jsonResponse.end()).c_str());
                return S_OK;
            }

            if (endpoint == L"/api/browseForFolder") 
            {
                std::wstring folderPath = BrowseForFolder();
                auto jsonResponse = std::string(folderPath.begin(), folderPath.end());

                pVarResult->vt = VT_BSTR;
                pVarResult->bstrVal = SysAllocString(std::wstring(jsonResponse.begin(), jsonResponse.end()).c_str());
                return S_OK;
            }

        	if (endpoint == L"/api/openFolderDialog") 
            {
	            std::wstring folderPath = OpenFolderDialog();
	            auto jsonResponse = std::string(folderPath.begin(), folderPath.end());

	            pVarResult->vt = VT_BSTR;
	            pVarResult->bstrVal = SysAllocString(std::wstring(jsonResponse.begin(), jsonResponse.end()).c_str());
	            return S_OK;
            }

            if (endpoint == L"/api/clipboard/copy") {
                std::string text = requestPayload["postData"]["text"];

                // Copy text to clipboard
                if (!OpenClipboard(nullptr)) {
                    pVarResult->vt = VT_BSTR;
                    pVarResult->bstrVal = SysAllocString(L"Failed to open clipboard");
                    return E_FAIL;
                }

                EmptyClipboard();
                HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, text.size() + 1);
                if (hGlob == nullptr) {
                    CloseClipboard();
                    pVarResult->vt = VT_BSTR;
                    pVarResult->bstrVal = SysAllocString(L"Failed to allocate global memory");
                    return E_OUTOFMEMORY;
                }

                strcpy_s(static_cast<char*>(hGlob), text.size() + 1, text.c_str());
                if (SetClipboardData(CF_TEXT, hGlob) == nullptr) {
                    GlobalFree(hGlob);
                    CloseClipboard();
                    pVarResult->vt = VT_BSTR;
                    pVarResult->bstrVal = SysAllocString(L"Failed to set clipboard data");
                    return E_FAIL;
                }

                CloseClipboard();
                pVarResult->vt = VT_BSTR;
                pVarResult->bstrVal = SysAllocString(L"Text copied to clipboard");
                return S_OK;
            }
            if (endpoint == L"/api/systemMetrics") {
                json metrics = GetSystemMetrics();
                std::string jsonResponse = metrics.dump();

                pVarResult->vt = VT_BSTR;
                pVarResult->bstrVal = SysAllocString(std::wstring(jsonResponse.begin(), jsonResponse.end()).c_str());
                return S_OK;
            }
        }
        return DISP_E_MEMBERNOTFOUND;
    }

private:
    ULONG m_refCount{ 1 };
    static std::wstring GetLoggedInUserName();
    static std::wstring GetUserDomain();
    static json GetLoggedInUserInfo();
    static bool AuthenticateUser(const std::wstring& username, const std::wstring& password, const std::wstring& domain);
    static double GetDiskSpaceUsage(const std::wstring& drive);
    double GetCPUTemperature() const;
    static double GetSystemTemperature();
    static double GetMemoryUsage();
    static double GetCpuUsage();
    static json GetSystemMetrics();
    static std::wstring BrowseForFolder();
    static std::string ReadFileContent(const std::wstring& filePath);
    static std::wstring OpenFolderDialog();
    static std::wstring SaveFileDialog();
    static std::wstring OpenFileDialog();

};


// Examples User's code adding the endpoint
/*
ApiBridge.AddEndpoint(L"/api/getusers", [](const json& requestPayload) -> std::string {
    // Example: Return a list of users
    json users = {
        {"users", {
            {{"id", 1}, {"name", "John Doe"}, {"email", "john@example.com"}},
            {{"id", 2}, {"name", "Jane Smith"}, {"email", "jane@example.com"}},
            {{"id", 3}, {"name", "Bob Wilson"}, {"email", "bob@example.com"}}
        }}
    };
    return users.dump();
    });
    yourClassInstance.AddEndpoint(L"/api/getusers", [](const json& requestPayload) -> std::string {
    // Check for query parameters
    std::string roleFilter;
    if (requestPayload.contains("queryParams") &&
        requestPayload["queryParams"].contains("role")) {
        roleFilter = requestPayload["queryParams"]["role"];
    }

    // Example: Filter users by role if specified
    json users = {{"users", json::array()}};
    auto& usersArray = users["users"];

    // Sample user data
    std::vector<json> allUsers = {
        {{"id", 1}, {"name", "John Doe"}, {"role", "admin"}},
        {{"id", 2}, {"name", "Jane Smith"}, {"role", "user"}},
        {{"id", 3}, {"name", "Bob Wilson"}, {"role", "admin"}}
    };

    // Apply filter if role is specified
    for (const auto& user : allUsers) {
        if (roleFilter.empty() || user["role"] == roleFilter) {
            usersArray.push_back(user);
        }
    }

    return users.dump();
});

// React side of calling API
const [users, setUsers] = useState([]);

  const fetchUsers = async () => {
    console.log('fetching Users:');
    const data = await apiClient.get('/api/users');
    console.log('Users:', data);
    setUsers(data);

    // Submit form data using post method
    const formData = {
      username: "testUser",
      password: "password123"
    };
    const response = await apiClient.post('/api/submitForm', formData);
    console.log(response);
  };

 */
