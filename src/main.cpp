// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c
#include "main.h"
#include <iostream>
#include "WindowApp.h"
#include "WebView.h"
#include "spdlog.h"
#include "spdlog/async.h"
#include "sinks/daily_file_sink.h" // support for basic file logging
#include "sinks/msvc_sink.h"

void InitializeLog()
{
	spdlog::init_thread_pool(10000, 1); // queue with 10K items and 1 backing thread

	const auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/SHRAnimalMonitor.log", 23, 59);
	file_sink->set_level(spdlog::level::trace);
	const auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	msvc_sink->set_level(spdlog::level::trace);

	const spdlog::sinks_init_list sink_list = { file_sink, msvc_sink };

	// Create the async logger with multiple sinks
	const auto alllogger = std::make_shared<spdlog::async_logger>(
		"shared",
		sink_list.begin(),
		sink_list.end(),
		spdlog::thread_pool(),
		spdlog::async_overflow_policy::block
	);
	alllogger->set_level(spdlog::level::trace);
	spdlog::flush_every(std::chrono::seconds(5));
	set_default_logger(alllogger);
	SPDLOG_TRACE("Some trace message");
}

void processMessage(wil::unique_cotaskmem_string::pointer* message)
{
    return;
}

int CALLBACK WinMain(
	const HINSTANCE hInstance,
	const HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	const int nShowCmd
)
{
    InitializeLog();

	WindowApp app;
	SPDLOG_TRACE("Entering");
	app.Run(hInstance, nShowCmd);
	SPDLOG_TRACE("Shutting Down Logger");
	spdlog::shutdown();
    return 0;
}