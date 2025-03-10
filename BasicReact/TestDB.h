#pragma once
#include "include/NativeDatabaseAccess.h"
#include "libs/sqlite3.h"


inline bool CreateTestDatabase(const std::wstring& dbPath) 
{
	sqlite3* db = nullptr;
	std::string path = std::string(dbPath.begin(), dbPath.end());

	if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
		OutputDebugStringA("Failed to create database");
		return false;
	}

	// Create a test table
	const char* createTableSQL =
		"CREATE TABLE IF NOT EXISTS test_users ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"name TEXT NOT NULL,"
		"email TEXT,"
		"age INTEGER);";

	char* errMsg = nullptr;
	if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
		OutputDebugStringA(errMsg);
		sqlite3_free(errMsg);
		sqlite3_close(db);
		return false;
	}

	// Insert some test data
	const char* insertDataSQL =
		"INSERT INTO test_users (name, email, age) VALUES "
		"('John Doe', 'john@example.com', 30),"
		"('Jane Smith', 'jane@example.com', 28),"
		"('Bob Johnson', 'bob@example.com', 45);";

	if (sqlite3_exec(db, insertDataSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) 
	{
		OutputDebugStringA(errMsg);
		sqlite3_free(errMsg);
		sqlite3_close(db);
		return false;
	}

	sqlite3_close(db);
	return true;
}

inline void TestDatabaseAccess(wil::com_ptr<ICoreWebView2> webview)
{
	// Create a test database or use an existing one
	std::wstring dbPath = L"C:\\temp\\test_database.db";
	CreateTestDatabase(dbPath);

	ComPtr<NativeDatabaseAccess> databaseAccess = Make<NativeDatabaseAccess>(webview.get());

	// For SQLite, connection string is just the file path with a prefix
	std::wstring connStr = L"Data Source=" + dbPath;

	// Test a simple query
	BSTR result = nullptr;
	HRESULT hr = databaseAccess->ExecuteQuery(SysAllocString(connStr.c_str()),
		SysAllocString(L"SELECT * FROM test_users;"),
		&result);

	if (SUCCEEDED(hr) && result) {
		// Output the JSON result
		OutputDebugStringW(result);
		SysFreeString(result);
	}
	else {
		OutputDebugStringW(L"Query failed");
	}
}
