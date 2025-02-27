// For PostgreSQL
#include "NativeDatabaseAccess.h"
#include <mutex>
#include "spdlog.h"

// Helper to convert BSTR to std::string
std::string BSTRToString(BSTR bstr) {
    if (!bstr) return "";

    int length = SysStringLen(bstr);
    std::wstring wstr(bstr, length);

    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(bufferSize - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], bufferSize, nullptr, nullptr);

    return result;
}

// Helper to convert std::string to BSTR
BSTR StringToBSTR(const std::string& str) {
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(bufferSize - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], bufferSize);

    return SysAllocString(wstr.c_str());
}

// Connection pool management
class ConnectionPool {
private:
    static std::unordered_map<std::wstring, std::shared_ptr<void>> postgresConnections;
    static std::unordered_map<std::wstring, std::shared_ptr<void>> sqliteConnections;
    static std::mutex connectionMutex;

public:
    static void* GetConnection(const std::wstring& connectionString, DatabaseType dbType) {
        std::lock_guard<std::mutex> lock(connectionMutex);

        if (dbType == DatabaseType::PostgreSQL) {
            if (postgresConnections.find(connectionString) == postgresConnections.end()) {
                std::string connStr = BSTRToString(SysAllocString(connectionString.c_str()));
                PGconn* conn = PQconnectdb(connStr.c_str());

                if (PQstatus(conn) != CONNECTION_OK) {
                    PQfinish(conn);
                    return nullptr;
                }

                postgresConnections[connectionString] = std::shared_ptr<void>(conn, [](void* ptr) {
                    PQfinish(static_cast<PGconn*>(ptr));
                    });
            }
            return postgresConnections[connectionString].get();
        }
        else if (dbType == DatabaseType::SQLite) {
            if (sqliteConnections.find(connectionString) == sqliteConnections.end()) {
                sqlite3* db = nullptr;
                // Extract filename from connection string (simplified)
                std::wstring filename = connectionString;
                size_t filePos = connectionString.find(L"Data Source=");
                if (filePos != std::wstring::npos) {
                    filename = connectionString.substr(filePos + 12);
                    size_t endPos = filename.find(L';');
                    if (endPos != std::wstring::npos) {
                        filename = filename.substr(0, endPos);
                    }
                }

                std::string filenameUtf8 = BSTRToString(SysAllocString(filename.c_str()));
                if (sqlite3_open(filenameUtf8.c_str(), &db) != SQLITE_OK) {
                    sqlite3_close(db);
                    return nullptr;
                }

                sqliteConnections[connectionString] = std::shared_ptr<void>(db, [](void* ptr) {
                    sqlite3_close(static_cast<sqlite3*>(ptr));
                    });
            }
            return sqliteConnections[connectionString].get();
        }

        return nullptr;
    }

    static bool CloseConnection(const std::wstring& connectionString) {
        std::lock_guard<std::mutex> lock(connectionMutex);

        auto pgConn = postgresConnections.find(connectionString);
        if (pgConn != postgresConnections.end()) {
            postgresConnections.erase(pgConn);
            return true;
        }

        auto sqliteConn = sqliteConnections.find(connectionString);
        if (sqliteConn != sqliteConnections.end()) {
            sqliteConnections.erase(sqliteConn);
            return true;
        }

        return false;
    }
};

// Initialize static members
std::unordered_map<std::wstring, std::shared_ptr<void>> ConnectionPool::postgresConnections;
std::unordered_map<std::wstring, std::shared_ptr<void>> ConnectionPool::sqliteConnections;
std::mutex ConnectionPool::connectionMutex;

STDMETHODIMP NativeDatabaseAccess::ExecuteQuery(BSTR connectionString, BSTR query, BSTR* pVarResult) {
    SPDLOG_TRACE("ExecuteQuery called");

    if (!connectionString || !query || !pVarResult) {
        return E_INVALIDARG;
    }

    try {
        std::wstring connStr(connectionString);
        std::wstring queryStr(query);
        auto dbType = GetDatabaseTypeFromConnectionString(connStr);
        json resultJson = json::array();

        switch (dbType) {
        case DatabaseType::PostgreSQL: {
            PGconn* conn = static_cast<PGconn*>(ConnectionPool::GetConnection(connStr, dbType));
            if (!conn) {
                SetStringResult(pVarResult, L"{\"error\": \"Failed to connect to PostgreSQL database\"}");
                return S_OK;
            }

            std::string queryUtf8 = BSTRToString(query);
            PGresult* res = PQexec(conn, queryUtf8.c_str());

            if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
                std::string errorMsg = PQerrorMessage(conn);
                PQclear(res);
                SetStringResult(pVarResult, std::wstring(L"{\"error\": \"") +
                    std::wstring(errorMsg.begin(), errorMsg.end()) + L"\"}");
                return S_OK;
            }

            int rows = PQntuples(res);
            int cols = PQnfields(res);

            // Get column names
            std::vector<std::string> columnNames;
            for (int i = 0; i < cols; i++) {
                columnNames.push_back(PQfname(res, i));
            }

            // Get data
            for (int i = 0; i < rows; i++) {
                json rowJson;
                for (int j = 0; j < cols; j++) {
                    if (PQgetisnull(res, i, j)) {
                        rowJson[columnNames[j]] = nullptr;
                    }
                    else {
                        rowJson[columnNames[j]] = PQgetvalue(res, i, j);
                    }
                }
                resultJson.push_back(rowJson);
            }

            PQclear(res);
            break;
        }

        case DatabaseType::SQLite: {
            sqlite3* db = static_cast<sqlite3*>(ConnectionPool::GetConnection(connStr, dbType));
            if (!db) {
                SetStringResult(pVarResult, L"{\"error\": \"Failed to connect to SQLite database\"}");
                return S_OK;
            }

            std::string queryUtf8 = BSTRToString(query);
            sqlite3_stmt* stmt;

            if (sqlite3_prepare_v2(db, queryUtf8.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                std::string errorMsg = sqlite3_errmsg(db);
                SetStringResult(pVarResult, std::wstring(L"{\"error\": \"") +
                    std::wstring(errorMsg.begin(), errorMsg.end()) + L"\"}");
                return S_OK;
            }

            int colCount = sqlite3_column_count(stmt);
            std::vector<std::string> columnNames;

            // Get column names
            for (int i = 0; i < colCount; i++) {
                columnNames.push_back(sqlite3_column_name(stmt, i));
            }

            // Get data
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                json rowJson;
                for (int i = 0; i < colCount; i++) {
                    int colType = sqlite3_column_type(stmt, i);

                    switch (colType) {
                    case SQLITE_INTEGER:
                        rowJson[columnNames[i]] = sqlite3_column_int64(stmt, i);
                        break;
                    case SQLITE_FLOAT:
                        rowJson[columnNames[i]] = sqlite3_column_double(stmt, i);
                        break;
                    case SQLITE_TEXT:
                        rowJson[columnNames[i]] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                        break;
                    case SQLITE_BLOB:
                        // Handle BLOB as base64 or handle specially as needed
                        rowJson[columnNames[i]] = "[BLOB data]";
                        break;
                    case SQLITE_NULL:
                        rowJson[columnNames[i]] = nullptr;
                        break;
                    }
                }
                resultJson.push_back(rowJson);
            }

            sqlite3_finalize(stmt);
            break;
        }

        default:
            SetStringResult(pVarResult, L"{\"error\": \"Unknown database type\"}");
            return S_OK;
        }

        std::string resultStr = resultJson.dump();
        SetStringResult(pVarResult, std::wstring(resultStr.begin(), resultStr.end()));

    }
    catch (const std::exception& e) {
        std::string errorMsg = e.what();
        SetStringResult(pVarResult, std::wstring(L"{\"error\": \"") +
            std::wstring(errorMsg.begin(), errorMsg.end()) + L"\"}");
    }

    return S_OK;
}
// Implementation of database methods
DatabaseType NativeDatabaseAccess::GetDatabaseTypeFromConnectionString(const std::wstring& connectionString) {
    std::wstring lowerConnStr = connectionString;
    std::transform(lowerConnStr.begin(), lowerConnStr.end(), lowerConnStr.begin(),
        [](wchar_t c) { return std::tolower(c); });

    // Check for SQLite connection string patterns
    if (lowerConnStr.find(L"data source=") != std::wstring::npos ||
        lowerConnStr.find(L"filename=") != std::wstring::npos ||
        lowerConnStr.find(L".db") != std::wstring::npos ||
        lowerConnStr.find(L".sqlite") != std::wstring::npos ||
        lowerConnStr.find(L"sqlite") != std::wstring::npos) {
        return DatabaseType::SQLite;
    }

    // Check for PostgreSQL connection string patterns
    if (lowerConnStr.find(L"host=") != std::wstring::npos ||
        lowerConnStr.find(L"port=") != std::wstring::npos ||
        lowerConnStr.find(L"dbname=") != std::wstring::npos ||
        lowerConnStr.find(L"user=") != std::wstring::npos ||
        lowerConnStr.find(L"password=") != std::wstring::npos ||
        lowerConnStr.find(L"postgresql") != std::wstring::npos ||
        lowerConnStr.find(L"postgres") != std::wstring::npos) {
        return DatabaseType::PostgreSQL;
    }

    // If we can't determine the type, return Unknown
    return DatabaseType::Unknown;
}
STDMETHODIMP NativeDatabaseAccess::OpenConnection(BSTR connectionString, BOOL* success) {
    SPDLOG_TRACE("OpenConnection called");

    if (!connectionString || !success) {
        return E_INVALIDARG;
    }

    *success = FALSE;

    try {
        std::wstring connStr(connectionString);
        DatabaseType dbType = GetDatabaseTypeFromConnectionString(connStr);

        SPDLOG_INFO("Opening connection for database type: {}",
            dbType == DatabaseType::SQLite ? "SQLite" :
            dbType == DatabaseType::PostgreSQL ? "PostgreSQL" : "Unknown");

        void* conn = nullptr;

        switch (dbType) {
        case DatabaseType::SQLite: {
            // Extract filename from connection string
            std::wstring filename = connStr;
            size_t filePos = connStr.find(L"Data Source=");
            if (filePos != std::wstring::npos) {
                filename = connStr.substr(filePos + 12);
                size_t endPos = filename.find(L';');
                if (endPos != std::wstring::npos) {
                    filename = filename.substr(0, endPos);
                }
            }

            // Convert to UTF-8
            std::string filenameUtf8(filename.begin(), filename.end());
            sqlite3* db = nullptr;

            SPDLOG_INFO("Opening SQLite database: {}", filenameUtf8);

            if (sqlite3_open(filenameUtf8.c_str(), &db) != SQLITE_OK) {
                std::string errorMsg = sqlite3_errmsg(db);
                SPDLOG_ERROR("Failed to open SQLite database: {}", errorMsg);
                sqlite3_close(db);
                return S_OK; // Return success but with *success = FALSE
            }

            // Store in connection pool
            std::lock_guard<std::mutex> lock(m_connectionMutex);
            m_sqliteConnections[connStr] = std::shared_ptr<void>(db, [](void* ptr) {
                sqlite3_close(static_cast<sqlite3*>(ptr));
                });

            *success = TRUE;
            break;
        }

        case DatabaseType::PostgreSQL: {
            // Convert to UTF-8
            std::string connStrUtf8(connStr.begin(), connStr.end());

            SPDLOG_INFO("Opening PostgreSQL connection");

            PGconn* pgConn = PQconnectdb(connStrUtf8.c_str());

            if (PQstatus(pgConn) != CONNECTION_OK) {
                std::string errorMsg = PQerrorMessage(pgConn);
                SPDLOG_ERROR("Failed to connect to PostgreSQL: {}", errorMsg);
                PQfinish(pgConn);
                return S_OK; // Return success but with *success = FALSE
            }

            // Store in connection pool
            std::lock_guard<std::mutex> lock(m_connectionMutex);
            m_postgresConnections[connStr] = std::shared_ptr<void>(pgConn, [](void* ptr) {
                PQfinish(static_cast<PGconn*>(ptr));
                });

            *success = TRUE;
            break;
        }

        default:
            SPDLOG_ERROR("Unknown database type");
            return S_OK; // Return success but with *success = FALSE
        }

    }
    catch (const std::exception& e) {
        SPDLOG_ERROR("Exception in OpenConnection: {}", e.what());
        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP NativeDatabaseAccess::CloseConnection(BSTR connectionString, BOOL* success) {
    SPDLOG_TRACE("CloseConnection called");

    if (!connectionString || !success) {
        return E_INVALIDARG;
    }

    *success = FALSE;

    try {
        std::wstring connStr(connectionString);

        // Check if there's an active transaction and roll it back if needed
        {
            std::lock_guard<std::mutex> lock(m_connectionMutex);
            auto txnIt = m_activeTransactions.find(connStr);
            if (txnIt != m_activeTransactions.end() && txnIt->second) {
                // There's an active transaction, roll it back
                BOOL txnSuccess = FALSE;
                RollbackTransaction(connectionString, &txnSuccess);
                SPDLOG_INFO("Auto-rolled back transaction during connection close: {}",
                    txnSuccess ? "success" : "failed");
            }
        }

        // Close the connection
        std::lock_guard<std::mutex> lock(m_connectionMutex);

        // Try SQLite connections first
        auto sqliteIt = m_sqliteConnections.find(connStr);
        if (sqliteIt != m_sqliteConnections.end()) {
            m_sqliteConnections.erase(sqliteIt);
            *success = TRUE;
            SPDLOG_INFO("Closed SQLite connection");
            return S_OK;
        }

        // Try PostgreSQL connections next
        auto pgIt = m_postgresConnections.find(connStr);
        if (pgIt != m_postgresConnections.end()) {
            m_postgresConnections.erase(pgIt);
            *success = TRUE;
            SPDLOG_INFO("Closed PostgreSQL connection");
            return S_OK;
        }

        SPDLOG_WARN("No active connection found to close");

    }
    catch (const std::exception& e) {
        SPDLOG_ERROR("Exception in CloseConnection: {}", e.what());
        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP NativeDatabaseAccess::BeginTransaction(BSTR connectionString, BOOL* success) {
    SPDLOG_TRACE("BeginTransaction called");

    if (!connectionString || !success) {
        return E_INVALIDARG;
    }

    *success = FALSE;

    try {
        std::wstring connStr(connectionString);
        DatabaseType dbType = GetDatabaseTypeFromConnectionString(connStr);

        // Check if there's already an active transaction for this connection
        {
            std::lock_guard<std::mutex> lock(m_connectionMutex);
            auto txnIt = m_activeTransactions.find(connStr);
            if (txnIt != m_activeTransactions.end() && txnIt->second) {
                SPDLOG_WARN("Transaction already in progress for this connection");
                return S_OK; // Return success but with *success = FALSE
            }
        }

        switch (dbType) {
        case DatabaseType::SQLite: {
            // Get the SQLite connection
            sqlite3* db = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_connectionMutex);
                auto connIt = m_sqliteConnections.find(connStr);
                if (connIt == m_sqliteConnections.end()) {
                    SPDLOG_ERROR("No open SQLite connection found");
                    return S_OK; // Return success but with *success = FALSE
                }
                db = static_cast<sqlite3*>(connIt->second.get());
            }

            // Begin transaction
            char* errMsg = nullptr;
            if (sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, &errMsg) != SQLITE_OK) {
                std::string errorMsg = errMsg;
                sqlite3_free(errMsg);
                SPDLOG_ERROR("Failed to begin SQLite transaction: {}", errorMsg);
                return S_OK; // Return success but with *success = FALSE
            }

            // Mark transaction as active
            {
                std::lock_guard<std::mutex> lock(m_connectionMutex);
                m_activeTransactions[connStr] = true;
            }

            *success = TRUE;
            SPDLOG_INFO("SQLite transaction started");
            break;
        }

        case DatabaseType::PostgreSQL: {
            // Get the PostgreSQL connection
            PGconn* pgConn = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_connectionMutex);
                auto connIt = m_postgresConnections.find(connStr);
                if (connIt == m_postgresConnections.end()) {
                    SPDLOG_ERROR("No open PostgreSQL connection found");
                    return S_OK; // Return success but with *success = FALSE
                }
                pgConn = static_cast<PGconn*>(connIt->second.get());
            }

            // Begin transaction
            PGresult* res = PQexec(pgConn, "BEGIN");
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                std::string errorMsg = PQerrorMessage(pgConn);
                PQclear(res);
                SPDLOG_ERROR("Failed to begin PostgreSQL transaction: {}", errorMsg);
                return S_OK; // Return success but with *success = FALSE
            }

            PQclear(res);

            // Mark transaction as active
            {
                std::lock_guard<std::mutex> lock(m_connectionMutex);
                m_activeTransactions[connStr] = true;
            }

            *success = TRUE;
            SPDLOG_INFO("PostgreSQL transaction started");
            break;
        }

        default:
            SPDLOG_ERROR("Unknown database type");
            return S_OK; // Return success but with *success = FALSE
        }

    }
    catch (const std::exception& e) {
        SPDLOG_ERROR("Exception in BeginTransaction: {}", e.what());
        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP NativeDatabaseAccess::CommitTransaction(BSTR connectionString, BOOL * success) {
    SPDLOG_TRACE("CommitTransaction called");

    if (!connectionString || !success) {
        return E_INVALIDARG;
    }

    *success = FALSE;

    try {
        std::wstring connStr(connectionString);
        DatabaseType dbType = GetDatabaseTypeFromConnectionString(connStr);

        // Check if there's an active transaction for this connection
        bool hasActiveTxn = false;
        {
            std::lock_guard<std::mutex> lock(m_connectionMutex);
            auto txnIt = m_activeTransactions.find(connStr);
            hasActiveTxn = (txnIt != m_activeTransactions.end() && txnIt->second);
        }

        if (!hasActiveTxn) {
            SPDLOG_WARN("No active transaction to commit");
            return S_OK; // Return success but with *success = FALSE
        }

        switch (dbType) {
        case DatabaseType::SQLite: {
            // Get the SQLite connection
            sqlite3* db = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_connectionMutex);
                auto connIt = m_sqliteConnections.find(connStr);
                if (connIt == m_sqliteConnections.end()) {
                    SPDLOG_ERROR("No open SQLite connection found");
                    return S_OK; // Return success but with *success = FALSE
                }
                db = static_cast<sqlite3*>(connIt->second.get());
            }

            // Commit transaction
            char* errMsg = nullptr;
            if (sqlite3_exec(db, "COMMIT", nullptr, nullptr, &errMsg) != SQLITE_OK) {
                std::string errorMsg = errMsg;
                sqlite3_free(errMsg);
                SPDLOG_ERROR("Failed to commit SQLite transaction: {}", errorMsg);
                return S_OK; // Return success but with *success = FALSE
            }

            // Mark transaction as inactive
            {
                std::lock_guard<std::mutex> lock(m_connectionMutex);
                m_activeTransactions[connStr] = false;
            }

            *success = TRUE;
            SPDLOG_INFO("SQLite transaction committed");
            break;
        }

        case DatabaseType::PostgreSQL: {
            // Get the PostgreSQL connection
            PGconn* pgConn = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_connectionMutex);
                auto connIt = m_postgresConnections.find(connStr);
                if (connIt == m_postgresConnections.end()) {
                    SPDLOG_ERROR("No open PostgreSQL connection found");
                    return S_OK; // Return success but with *success = FALSE
                }
                pgConn = static_cast<PGconn*>(connIt->second.get());
            }

            // Commit transaction
            PGresult* res = PQexec(pgConn, "COMMIT");
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                std::string errorMsg = PQerrorMessage(pgConn);
                PQclear(res);
                SPDLOG_ERROR("Failed to commit PostgreSQL transaction: {}", errorMsg);
                return S_OK; // Return success but with *success = FALSE
            }

            PQclear(res);
            {
                std::lock_guard<std::mutex> lock(m_connectionMutex);
                m_activeTransactions[connStr] = false;
            }

            *success = TRUE;
            SPDLOG_INFO("PostgreSQL transaction committed");
            break;
        }

        default:
            SPDLOG_ERROR("Unknown database type");
            return S_OK; // Return success but with *success = FALSE
        }

    }
    catch (const std::exception& e) {
        SPDLOG_ERROR("Exception in CommitTransaction: {}", e.what());
        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP NativeDatabaseAccess::RollbackTransaction(BSTR connectionString, BOOL* success) {
    // Implementation here
    return S_OK;
}

STDMETHODIMP NativeDatabaseAccess::CreateParameterizedQuery(BSTR query, BSTR* queryId)
{ 
    return S_OK; 
}

STDMETHODIMP NativeDatabaseAccess::AddParameter(BSTR queryId, BSTR paramName, VARIANT paramValue)
{ 
    return S_OK; 
}

STDMETHODIMP NativeDatabaseAccess::ExecuteParameterizedQuery(BSTR connectionString, BSTR queryId, BSTR* pVarResult)
{ 
    return S_OK; 
}

STDMETHODIMP NativeDatabaseAccess::ExecuteParameterizedNonQuery(BSTR connectionString, BSTR queryId, LONG* rowsAffected)
{ 
    return S_OK; 
}

void NativeDatabaseAccess::SetStringResult(VARIANT* pVarResult, const std::wstring& str)
{
    if (pVarResult) {
        pVarResult->vt = VT_BSTR;
        pVarResult->bstrVal = SysAllocString(str.c_str());
    }
    SPDLOG_TRACE("Exiting");
}

void NativeDatabaseAccess::SetStringResult(BSTR* pVarResult, const std::wstring& str) 
{
    if (!pVarResult) 
    {
        return;
    }

    // Free any existing BSTR to prevent memory leaks
    if (*pVarResult) 
    {
        SysFreeString(*pVarResult);
        *pVarResult = nullptr;
    }

    // Allocate a new BSTR from the wstring
    *pVarResult = SysAllocStringLen(str.c_str(), static_cast<UINT>(str.length()));
}

STDMETHODIMP NativeDatabaseAccess::ExecuteNonQuery(BSTR connectionString, BSTR query, LONG* rowsAffected) {
    SPDLOG_TRACE("ExecuteNonQuery called");

    if (!connectionString || !query || !rowsAffected) {
        return E_INVALIDARG;
    }

    *rowsAffected = 0;

    try {
        std::wstring connStr(connectionString);
        std::wstring queryStr(query);
        DatabaseType dbType = GetDatabaseTypeFromConnectionString(connStr);

        switch (dbType) {
        case DatabaseType::PostgreSQL: {
            PGconn* conn = static_cast<PGconn*>(ConnectionPool::GetConnection(connStr, dbType));
            if (!conn) {
                return E_FAIL;
            }

            std::string queryUtf8 = BSTRToString(query);
            PGresult* res = PQexec(conn, queryUtf8.c_str());

            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                std::string errorMsg = PQerrorMessage(conn);
                PQclear(res);
                SPDLOG_ERROR("PostgreSQL error: {}", errorMsg);
                return E_FAIL;
            }

            std::string rowsStr = PQcmdTuples(res);
            if (!rowsStr.empty()) {
                *rowsAffected = std::stol(rowsStr);
            }

            PQclear(res);
            break;
        }

        case DatabaseType::SQLite: {
            sqlite3* db = static_cast<sqlite3*>(ConnectionPool::GetConnection(connStr, dbType));
            if (!db) {
                return E_FAIL;
            }

            std::string queryUtf8 = BSTRToString(query);
            char* errMsg = nullptr;

            if (sqlite3_exec(db, queryUtf8.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
                std::string errorMsg = errMsg;
                sqlite3_free(errMsg);
                SPDLOG_ERROR("SQLite error: {}", errorMsg);
                return E_FAIL;
            }

            *rowsAffected = sqlite3_changes(db);
            break;
        }

        default:
            return E_FAIL;
        }

    }
    catch (const std::exception& e) {
        SPDLOG_ERROR("Exception in ExecuteNonQuery: {}", e.what());
        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP NativeDatabaseAccess::GetConnectionStatus(BSTR connectionString, BOOL* isConnected) {
    SPDLOG_TRACE("GetConnectionStatus called");

    if (!connectionString || !isConnected) {
        return E_INVALIDARG;
    }

    *isConnected = FALSE;

    try {
        std::wstring connStr(connectionString);
        DatabaseType dbType = GetDatabaseTypeFromConnectionString(connStr);

        void* conn = ConnectionPool::GetConnection(connStr, dbType);
        if (!conn) {
            return S_OK;  // Not connected, but not an error
        }

        switch (dbType) {
        case DatabaseType::PostgreSQL: {
            PGconn* pgConn = static_cast<PGconn*>(conn);
            *isConnected = (PQstatus(pgConn) == CONNECTION_OK) ? TRUE : FALSE;
            break;
        }

        case DatabaseType::SQLite: {
            sqlite3* db = static_cast<sqlite3*>(conn);
            // For SQLite, if we have a connection handle, we're considered connected
            *isConnected = TRUE;
            break;
        }

        default:
            *isConnected = FALSE;
            break;
        }

    }
    catch (const std::exception& e) {
        SPDLOG_ERROR("Exception in GetConnectionStatus: {}", e.what());
        *isConnected = FALSE;
    }

    return S_OK;
}
STDMETHODIMP NativeDatabaseAccess::GetTypeInfoCount(UINT* pctinfo) {
    if (!pctinfo) {
        return E_INVALIDARG;
    }
    *pctinfo = 1;
    return S_OK;
}

STDMETHODIMP NativeDatabaseAccess::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) {
    if (!ppTInfo) {
        return E_INVALIDARG;
    }
    *ppTInfo = nullptr;

    if (iTInfo != 0) {
        return DISP_E_BADINDEX;
    }

    // Load the type library if not already loaded
    if (!m_typeLib) {
        OLECHAR szModulePath[MAX_PATH] = {};
        GetModuleFileName(nullptr, szModulePath, MAX_PATH);

        // Extract path without filename
        OLECHAR* pLastSlash = wcsrchr(szModulePath, L'\\');
        if (pLastSlash) {
            *(pLastSlash + 1) = L'\0';
        }

        // Append .tlb file name
        wcscat_s(szModulePath, L"DatabaseInterfaces.tlb");

        // Load the type library
        HRESULT hr = LoadTypeLib(szModulePath, &m_typeLib);
        if (FAILED(hr)) {
            return hr;
        }
    }

    // Get the type info for the interface
    return m_typeLib->GetTypeInfoOfGuid(__uuidof(INativeDatabaseAccess), ppTInfo);
}

STDMETHODIMP NativeDatabaseAccess::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) {
    // We need type info to map names to IDs
    ITypeInfo* pTypeInfo = nullptr;
    HRESULT hr = GetTypeInfo(0, lcid, &pTypeInfo);
    if (FAILED(hr)) {
        return hr;
    }

    hr = pTypeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
    pTypeInfo->Release();
    return hr;
}

STDMETHODIMP NativeDatabaseAccess::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) {
    // We need type info for invocation
    ITypeInfo* pTypeInfo = nullptr;
    HRESULT hr = GetTypeInfo(0, lcid, &pTypeInfo);
    if (FAILED(hr)) {
        return hr;
    }

    hr = pTypeInfo->Invoke(static_cast<INativeDatabaseAccess*>(this), dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
    pTypeInfo->Release();
    return hr;
}