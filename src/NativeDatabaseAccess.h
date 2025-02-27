#pragma once
#include <windows.h>
#include <string>
#include "DatabaseTypes.h"
#include "INativeDatabaseAccess.h"
#include <wil/com.h>
#include <wrl/implements.h>
#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"
#include <lmcons.h>
#include "WebView2.h"
#include <shobjidl_core.h>
#include <shlobj.h>
#include <comdef.h>
#include <pdh.h>
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "advapi32.lib")

// For PostgreSQL
#include <libpq-fe.h>

// For SQLite
#include <sqlite3.h>

#include <mutex>
#include <unordered_map>
#include <memory>
#include <vector>

using namespace Microsoft::WRL;
using json = nlohmann::json;

class NativeDatabaseAccess final : public RuntimeClass<RuntimeClassFlags<ClassicCom>, INativeDatabaseAccess, IDispatch>
{
private:
    wil::com_ptr<ITypeLib> m_typeLib;
    ICoreWebView2* m_webview;

    // Connection pool management
    std::unordered_map<std::wstring, std::shared_ptr<void>> m_postgresConnections;
    std::unordered_map<std::wstring, std::shared_ptr<void>> m_sqliteConnections;
    std::mutex m_connectionMutex;

    // Transaction state tracking
    std::unordered_map<std::wstring, bool> m_activeTransactions;

    // Parameterized query management
    struct QueryParameter {
        std::wstring name;
        VARIANT value;
    };

    struct ParameterizedQuery {
        std::wstring query;
        std::vector<QueryParameter> parameters;
    };

    std::unordered_map<std::wstring, ParameterizedQuery> m_parameterizedQueries;
    std::mutex m_queryMutex;

    DatabaseType GetDatabaseTypeFromConnectionString(const std::wstring& connectionString);
    
    void* GetConnection(const std::wstring& connectionString, DatabaseType dbType);
    std::string FormatParameterizedQuery(const ParameterizedQuery& query);
    static void SetStringResult(VARIANT* pVarResult, const std::wstring& str);
    static void SetStringResult(BSTR* pVarResult, const std::wstring& str);

    // IDispatch implementation
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;
    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;
    STDMETHODIMP GetIDsOfNames(
        REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;
    STDMETHODIMP Invoke(
        DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
        VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;

public:
    NativeDatabaseAccess(ICoreWebView2* webview) : m_webview(webview) {
        SPDLOG_TRACE("NativeDatabaseAccess created");
    }
    ~NativeDatabaseAccess() { SPDLOG_TRACE("NativeDatabaseAccess destroyed"); }

    // Connection management
    STDMETHODIMP OpenConnection(BSTR connectionString, BOOL* success) override;
    STDMETHODIMP CloseConnection(BSTR connectionString, BOOL* success) override;
    STDMETHODIMP GetConnectionStatus(BSTR connectionString, BOOL* isConnected) override;

    // Query execution
    STDMETHODIMP ExecuteQuery(BSTR connectionString, BSTR query, BSTR* pVarResult) override;
    STDMETHODIMP ExecuteNonQuery(BSTR connectionString, BSTR query, LONG* rowsAffected) override;

    // Transaction management
    STDMETHODIMP BeginTransaction(BSTR connectionString, BOOL* success) override;
    STDMETHODIMP CommitTransaction(BSTR connectionString, BOOL* success) override;
    STDMETHODIMP RollbackTransaction(BSTR connectionString, BOOL* success) override;

    // Parameter binding
    STDMETHODIMP CreateParameterizedQuery(BSTR query, BSTR* queryId) override;
    STDMETHODIMP AddParameter(BSTR queryId, BSTR paramName, VARIANT paramValue) override;
    STDMETHODIMP ExecuteParameterizedQuery(BSTR connectionString, BSTR queryId, BSTR* pVarResult) override;
    STDMETHODIMP ExecuteParameterizedNonQuery(BSTR connectionString, BSTR queryId, LONG* rowsAffected) override;
};

