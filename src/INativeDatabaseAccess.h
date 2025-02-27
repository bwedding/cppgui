#pragma once
#pragma once
#include <windows.h>
#include <oaidl.h>

// Define the interface ID - you'll need to generate a unique GUID
// {66A5F6BB-0D3F-4E8B-8E4F-2D12B49D7A6A} - example GUID, generate your own
interface __declspec(uuid("66A5F6BB-0D3F-4E8B-8E4F-2D12B49D7A6A")) INativeDatabaseAccess : public IDispatch
{
    // Connection management
    STDMETHOD(OpenConnection)(BSTR connectionString, BOOL * success) PURE;
    STDMETHOD(CloseConnection)(BSTR connectionString, BOOL* success) PURE;
    STDMETHOD(GetConnectionStatus)(BSTR connectionString, BOOL* isConnected) PURE;

    // Query execution
    STDMETHOD(ExecuteQuery)(BSTR connectionString, BSTR query, BSTR* pVarResult) PURE;
    STDMETHOD(ExecuteNonQuery)(BSTR connectionString, BSTR query, LONG* rowsAffected) PURE;

    // Transaction management
    STDMETHOD(BeginTransaction)(BSTR connectionString, BOOL* success) PURE;
    STDMETHOD(CommitTransaction)(BSTR connectionString, BOOL* success) PURE;
    STDMETHOD(RollbackTransaction)(BSTR connectionString, BOOL* success) PURE;

    // Parameter binding (safer than string concatenation)
    STDMETHOD(CreateParameterizedQuery)(BSTR query, BSTR* queryId) PURE;
    STDMETHOD(AddParameter)(BSTR queryId, BSTR paramName, VARIANT paramValue) PURE;
    STDMETHOD(ExecuteParameterizedQuery)(BSTR connectionString, BSTR queryId, BSTR* pVarResult) PURE;
    STDMETHOD(ExecuteParameterizedNonQuery)(BSTR connectionString, BSTR queryId, LONG* rowsAffected) PURE;
};