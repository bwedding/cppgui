#pragma once

#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

