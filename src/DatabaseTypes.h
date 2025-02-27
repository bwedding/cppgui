#pragma once
// Helper method to determine database type from connection string
enum class DatabaseType {
    PostgreSQL,
    SQLite,
    Unknown
};
