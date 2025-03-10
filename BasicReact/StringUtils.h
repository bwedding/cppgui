#pragma once
#include <string>
#include <random>

namespace StringUtils {
    // Helper function to generate random printable ASCII string
    inline std::wstring GenerateRandomString(size_t length) {
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
}
