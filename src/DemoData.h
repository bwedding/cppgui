#pragma once
#include "WindowApp.h"
#include <chrono>
#include <thread>
#include <random>
#include <vector>
#include "SystemData.h"

constexpr long DATA_SZ = 512000;

enum TrendDirection { UP, DOWN };

TrendDirection current_trend = UP;
std::chrono::time_point<std::chrono::steady_clock> trend_start_time = std::chrono::steady_clock::now();
std::chrono::minutes trend_duration(1); // Adjust trend duration as needed

// Helper function to get random adjustment factor (�20%)
double get_random_adjustment(TrendDirection direction) 
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<> dis(0.70, 1.30); // Adjust percentage range as needed

	double adjustment = dis(gen);
	if (direction == DOWN) {
		adjustment = 2.0 - adjustment; // Reverse the adjustment for downward trend
	}
	return adjustment;
}

// Helper function to adjust numeric value with randomization
double adjust_value(double value, TrendDirection direction) 
{
	return value * get_random_adjustment(direction);
}

// Helper function to adjust optional numeric value with randomization
std::optional<double> adjust_optional(const std::optional<double>& value, TrendDirection direction) 
{
	if (value.has_value()) {
		return adjust_value(*value, direction);
	}
	return value;
}

// Function to update trend direction based on elapsed time
void update_trend_direction() 
{
	auto now = std::chrono::steady_clock::now();
	if (now - trend_start_time >= trend_duration) 
	{
		// Reverse the trend direction
		current_trend = (current_trend == UP) ? DOWN : UP;
		trend_start_time = now; // Reset the trend start time
	}
}

HeartMonitor create_test_instance() 
{
	update_trend_direction(); // Update the trend direction before adjusting values
	HeartMonitor monitor;
	// Basic fields
	monitor.Timestamp = L"2025-01-16T10:55:58.9084242Z";
	monitor.SystemId = L"Test System 001";
	monitor.HeartRate = static_cast<int>(adjust_value(144, current_trend));
	monitor.OperationState = L"Auto";
	monitor.HeartStatus = L"Both Running";
	monitor.FlowLimitState = L"";
	monitor.FlowLimit = adjust_value(9.193, current_trend);
	monitor.AtmosPressure = adjust_value(0.0, current_trend);
	monitor.UseMedicalSensor = true;
	monitor.IVCSensorVal = L"-";
	monitor.LocalClock = L"11:55:58";
	monitor.Messages = { L"Test Message 1", L"Test Message 2" };

	// Status Data
	monitor.StatusData.ExtLeft = { L"OK", L"#FF2EAE00" };
	monitor.StatusData.ExtRight = { L"OK", L"#FF2EAE00" };
	monitor.StatusData.CANStatus = { L"Receiving", L"badge-success" };
	monitor.StatusData.BytesSent = { L"52", L"badge-info" };
	monitor.StatusData.BytesRecd = { L"0.08", L"badge-info" };
	monitor.StatusData.Strokes = { L"2,338", L"badge-info" };
	monitor.StatusData.IntLeft = { L"OK", L"#FF2EAE00" };
	monitor.StatusData.IntRight = { L"OK", L"#FF2EAE00" };
	monitor.StatusData.BusLoad = { L"5%", L"#FF2EAE00" };

	// Left Heart
	monitor.LeftHeart.StrokeVolume = static_cast<int>(adjust_value(48, current_trend));
	monitor.LeftHeart.PowerConsumption = { adjust_value(7.6, current_trend), adjust_value(8.6, current_trend), adjust_value(7.1, current_trend), L"Default" };
	monitor.LeftHeart.IntPressure = { adjust_value(14.5, current_trend), adjust_value(21.5, current_trend), adjust_value(11.5, current_trend), L"Yellow" };
	monitor.LeftHeart.MedicalPressure = { adjust_value(0, current_trend), adjust_value(0, current_trend), adjust_value(0, current_trend), L"Default" };
	monitor.LeftHeart.IntPressureMin = adjust_value(-4.5, current_trend);
	monitor.LeftHeart.IntPressureMax = adjust_value(24.2, current_trend);
	monitor.LeftHeart.CardiacOutput = { adjust_value(5, current_trend), adjust_value(5.7, current_trend), adjust_value(4.5, current_trend),  L"Default" };
	monitor.LeftHeart.ActualStrokeLen = adjust_value(20.4, current_trend);
	monitor.LeftHeart.TargetStrokeLen = adjust_value(21.8, current_trend);
	monitor.LeftHeart.SensorTemperature = adjust_value(21.99, current_trend);
	monitor.LeftHeart.ThermistorTemperature = adjust_value(21.99, current_trend);
	monitor.LeftHeart.CpuLoad = static_cast<int>(adjust_value(73, current_trend));
	monitor.LeftHeart.OutflowPressure = adjust_value(16.5, current_trend);

	// Right Heart
	monitor.RightHeart.StrokeVolume = static_cast<int>(adjust_value(40, current_trend));
	monitor.RightHeart.PowerConsumption = { adjust_value(1.8, current_trend), adjust_value(2.2, current_trend), adjust_value(1.6, current_trend), L"Default" };
	monitor.RightHeart.IntPressure = { adjust_value(9.5, current_trend), adjust_value(4.5, current_trend), adjust_value(17.5, current_trend), L"Default" };
	monitor.RightHeart.MedicalPressure = { adjust_value(0, current_trend), adjust_value(0, current_trend), adjust_value(0, current_trend), L"Default" };
	monitor.RightHeart.IntPressureMin = adjust_value(-7.5999999, current_trend);
	monitor.RightHeart.IntPressureMax = adjust_value(11.3999996, current_trend);
	monitor.RightHeart.CardiacOutput = { adjust_value(5.3, current_trend), adjust_value(4.6, current_trend), adjust_value(6.3, current_trend), L"Default" };
	monitor.RightHeart.ActualStrokeLen = adjust_value(16.4, current_trend);
	monitor.RightHeart.TargetStrokeLen = adjust_value(18.1, current_trend);
	monitor.RightHeart.SensorTemperature = adjust_value(21.99, current_trend);
	monitor.RightHeart.ThermistorTemperature = adjust_value(28.57, current_trend);
	monitor.RightHeart.CpuLoad = static_cast<int>(adjust_value(73, current_trend));
	monitor.RightHeart.OutflowPressure = adjust_value(21.5, current_trend);

	// Sensors
	monitor.AoPSensor = { adjust_value(90, current_trend), adjust_value(120, current_trend), adjust_value(80, current_trend), L"Yellow" };
	monitor.CVPSensor = { adjust_value(21, current_trend), adjust_value(25, current_trend), adjust_value(16, current_trend), L"Default" };
	monitor.PAPSensor = { adjust_value(15, current_trend), adjust_value(19, current_trend), adjust_value(13, current_trend), L"Default" };
	monitor.ArtPressSensor = { adjust_value(76, current_trend), adjust_value(115, current_trend), adjust_value(76, current_trend), L"Red" };

	return monitor;
}

class DataSource
{
private:
	std::random_device rd;
	std::mt19937 gen;
	std::wstring messages[2];  // Double buffer
	bool currentBuffer{ false };
	std::uniform_int_distribution<> charDist{ 32, 126 };
	std::uniform_int_distribution<> rotateDist{ 1, DATA_SZ - 1 };

public:
	DataSource()
	{
		// Generate initial random strings for both buffers
		messages[0].resize(DATA_SZ);
		messages[1].resize(DATA_SZ);
		for (auto& c : messages[0])
		{
			c = static_cast<wchar_t>(charDist(gen));
		}
		messages[1] = messages[0];  // Copy to second buffer
	}

	const std::wstring& GenerateRandomMessage()
	{
		// Work on the inactive buffer
		bool nextBuffer = !currentBuffer;

		// Copy from current to next buffer
		messages[nextBuffer] = messages[currentBuffer];

		// Rotate the copy
		size_t rotateAt = rotateDist(gen);
		std::rotate(messages[nextBuffer].begin(),
			messages[nextBuffer].begin() + rotateAt,
			messages[nextBuffer].end());

		// Swap buffers
		currentBuffer = nextBuffer;

		return messages[currentBuffer];
	}
};

void SendData()
{
	SPDLOG_TRACE("Entering");

	DataSource speedData;
	auto& commManager = CommunicationManager::Instance();

	while (true)
	{
		// Turn on for high speed load testing. This is NOT JSON data but will be recieved and counted
		//std::wstring message = speedData.GenerateRandomMessage();
		//if (!commManager.SendMessageToFrontend(message)) {
		//	LOG(ERROR) << "Failed to send random data message";
		//	break;
		//}
		HeartMonitor test_monitor = create_test_instance();
		int variation = (rand() % 7) - 3;  // Gives range of -3 to +3
		test_monitor.HeartRate = 144 + variation;

		std::wstring json_output = test_monitor.to_json();

		if (!commManager.SendMessageToFrontend(json_output)) {
			spdlog::error("Failed to send random data message");
			break;
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(3));  // Used to run it really fast for testing
		std::this_thread::sleep_for(std::chrono::milliseconds(test_monitor.HeartRate / 60 * 100));
	}
}

void StartSendingData()
{
	SPDLOG_TRACE("Entering");

	// Launch in a separate thread
	std::thread tickerThread(SendData);
	tickerThread.detach();  // Let it run independently
}

class Timer {
public:
	// Constructor that takes a delay in milliseconds and a lambda to execute
	Timer(int delay_ms, std::function<void()> callback)
		: delay_ms_(delay_ms), callback_(callback) {
		// Start the timer thread
		thread_ = std::thread(&Timer::run, this);
	}

	// Destructor to ensure the thread is joined
	~Timer() {
		if (thread_.joinable()) {
			thread_.join();
		}
	}

private:
	int delay_ms_; // Delay in milliseconds
	std::function<void()> callback_; // Callback to execute
	std::thread thread_; // Thread to run the timer

	// Function that runs in the thread
	void run() {
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms_)); // Sleep for the delay
		callback_(); // Execute the callback
	}
};
