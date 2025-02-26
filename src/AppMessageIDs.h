#pragma once
#include <Windows.h>

enum AppMessages
{
    WM_POST_MSG_TO_WEBVIEW = WM_APP + 1,
    WM_EXECUTE_SCRIPT_WEBVIEW,
    WM_USER_EVENT,
    WM_USER_SUBSCRIBE,
    WM_USER_UNSUBSCRIBE,
    WM_USER_DISPATCH
};

