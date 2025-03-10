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
}
