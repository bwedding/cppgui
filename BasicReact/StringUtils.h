#pragma once
#include <string>
#include <random>
#include <Windows.h> // Include Windows.h for MultiByteToWideChar and WideCharToMultiByte

namespace StringUtils {
    // Helper function to generate random printable ASCII string
    inline std::wstring GenerateRandomString(size_t length) 
    {
        const wchar_t charset[] = 
            L"0123456789"
            L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            L"abcdefghijklmnopqrstuvwxyz"
            L"!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?";
        
        const size_t charsetSize = sizeof(charset) / sizeof(charset[0]) - 1; // -1 for null terminator
        
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distribution(0, charsetSize - 1);
        
        std::wstring randomString;
        randomString.reserve(length);
        
        for (size_t i = 0; i < length; ++i) {
            randomString += charset[distribution(generator)];
        }
        
        return randomString;
    }
    
    // Helper function for safe conversion from wstring to string
    inline std::string WstringToString(const std::wstring& wstr) {
        // Use only ASCII characters to avoid encoding issues
        std::string result;
        result.reserve(wstr.size());
        
        for (wchar_t wc : wstr) {
            // Only include ASCII characters (0-127)
            if (wc <= 127) {
                result += static_cast<char>(wc);
            } else {
                result += '?'; // Replace non-ASCII with placeholder
            }
        }
        
        return result;
    }
    
    // Helper function to generate random ASCII-only string
    inline std::string GenerateRandomAsciiString(size_t length) {
        const char charset[] = 
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?";
        
        const size_t charsetSize = sizeof(charset) - 1; // -1 for null terminator
        
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distribution(0, charsetSize - 1);
        
        std::string randomString;
        randomString.reserve(length);
        
        for (size_t i = 0; i < length; ++i) {
            randomString += charset[distribution(generator)];
        }
        
        return randomString;
    }
    
    // Convert UTF-8 string to wide string
    inline std::wstring Utf8ToWide(const std::string& utf8Str) {
        if (utf8Str.empty()) {
            return std::wstring();
        }
        
        // Get the required buffer size
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), 
                                             static_cast<int>(utf8Str.size()), nullptr, 0);
        
        // Allocate the wide string
        std::wstring wideStr(size_needed, 0);
        
        // Convert the string
        MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), 
                           static_cast<int>(utf8Str.size()), &wideStr[0], size_needed);
        
        return wideStr;
    }
    
    // Convert wide string to UTF-8 string
    inline std::string WideToUtf8(const std::wstring& wideStr) {
        if (wideStr.empty()) {
            return std::string();
        }
        
        // Get the required buffer size
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), 
                                             static_cast<int>(wideStr.size()), nullptr, 0, 
                                             nullptr, nullptr);
        
        // Allocate the UTF-8 string
        std::string utf8Str(size_needed, 0);
        
        // Convert the string
        WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), 
                           static_cast<int>(wideStr.size()), &utf8Str[0], size_needed, 
                           nullptr, nullptr);
        
        return utf8Str;
    }
}
