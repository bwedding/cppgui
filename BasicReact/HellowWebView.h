#pragma once
#include <windows.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <chrono>
#include <iostream>
#include <iomanip>

#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"
#include <plog/Appenders/DebugOutputAppender.h>
#include "MSVCDebugFormatter.h"
#include "AsyncRollingFileAppender.h"

#include "TestDB.h"
#include "NativeWindowControls.h"
#include "SystemUtils.h"
#include "WebViewManager.h"
#include "MakeWindow.h"
#include "../resource.h" // Added for resource identifiers
#include "DataSender.h"
#include "DataSenderManager.h"
#include "WebView2DataStreamer.h"
#include "AppMessageIDs.h" // Include for message IDs

extern std::unique_ptr<WebViewManager> g_webViewManager;
