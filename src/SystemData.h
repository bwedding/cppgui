#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Helper functions for string conversion
inline std::wstring to_wstring(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

inline std::string to_string(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

// Forward declarations
struct StatusIndicator {
    std::wstring Text;
    std::wstring Color;

    // Default constructor
    StatusIndicator() : Text(L""), Color(L"") {}

    // Parameterized constructor
    StatusIndicator(std::wstring text, std::wstring color)
        : Text(std::move(text)), Color(std::move(color)) {
    }

    void from_json(const json& j) {
        if (j.contains("Text")) Text = to_wstring(j.at("Text").get<std::string>());
        if (j.contains("Color")) Color = to_wstring(j.at("Color").get<std::string>());
    }

    json to_json() const {
        json j;
        j["Text"] = to_string(Text);
        j["Color"] = to_string(Color);
        return j;
    }
};

struct MeasurementValue {
    std::optional<double> MeanValue;
    std::optional<double> MaxValue;
    std::optional<double> MinValue;
    std::wstring BackColor;

    // Default constructor
    MeasurementValue()
        : MeanValue(std::nullopt)
        , MaxValue(std::nullopt)
        , MinValue(std::nullopt)
        , BackColor(L"Default") {
    }

    // Constructor for direct initialization
    MeasurementValue(
        double mean,
        std::optional<double> max = std::nullopt,
        const std::optional<double> min = std::nullopt,
        std::wstring backColor = L"Default")
        : MeanValue(mean)
        , MaxValue(max)
        , MinValue(min)
        , BackColor(std::move(backColor)) {
    }

    // Constructor that takes optional primary value
    MeasurementValue(
        std::optional<double> mean,
        std::optional<double> max = std::nullopt,
        std::optional<double> min = std::nullopt,
        std::wstring backColor = L"Default")
        : MeanValue(mean)
        , MaxValue(max)
        , MinValue(min)
        , BackColor(std::move(backColor)) {
    }

    static bool isValidMeasurement(double value) {
        // You can adjust these thresholds based on your sensor ranges
        return value > -5000 && value < 5000;
    }

    void from_json(const json& j) {
        if (j.contains("MeanValue")) {
            try {
                double value = j.at("MeanValue").get<double>();
                MeanValue = isValidMeasurement(value) ? std::optional<double>(value) : std::nullopt;
            }
            catch (...) {
                MeanValue = std::nullopt;
            }
        }

        if (j.contains("MaxValue") && !j.at("MaxValue").is_null()) {
            try {
                double value = j.at("MaxValue").get<double>();
                MaxValue = isValidMeasurement(value) ? std::optional<double>(value) : std::nullopt;
            }
            catch (...) {
                MaxValue = std::nullopt;
            }
        }

        if (j.contains("MinValue") && !j.at("MinValue").is_null()) {
            try {
                double value = j.at("MinValue").get<double>();
                MinValue = isValidMeasurement(value) ? std::optional<double>(value) : std::nullopt;
            }
            catch (...) {
                MinValue = std::nullopt;
            }
        }

        if (j.contains("BackColor")) {
            BackColor = to_wstring(j.at("BackColor").get<std::string>());
        }
    }

    json to_json() const {
        json j;
        j["MeanValue"] = MeanValue.has_value() ? json(MeanValue.value()) : json("-");
        j["MaxValue"] = MaxValue.has_value() ? json(MaxValue.value()) : json(nullptr);
        j["MinValue"] = MinValue.has_value() ? json(MinValue.value()) : json(nullptr);
        j["BackColor"] = json(to_string(BackColor));
        return j;
    }
};

struct StatusData {
    StatusIndicator ExtLeft;
    StatusIndicator ExtRight;
    StatusIndicator CANStatus;
    StatusIndicator BytesSent;
    StatusIndicator BytesRecd;
    StatusIndicator Strokes;
    StatusIndicator IntLeft;
    StatusIndicator IntRight;
    StatusIndicator BusLoad;

    // Default constructor initializes all members with their default constructors
    StatusData() = default;

    void from_json(const json& j) {
        if (j.contains("ExtLeft")) ExtLeft.from_json(j.at("ExtLeft"));
        if (j.contains("ExtRight")) ExtRight.from_json(j.at("ExtRight"));
        if (j.contains("CANStatus")) CANStatus.from_json(j.at("CANStatus"));
        if (j.contains("BytesSent")) BytesSent.from_json(j.at("BytesSent"));
        if (j.contains("BytesRecd")) BytesRecd.from_json(j.at("BytesRecd"));
        if (j.contains("Strokes")) Strokes.from_json(j.at("Strokes"));
        if (j.contains("IntLeft")) IntLeft.from_json(j.at("IntLeft"));
        if (j.contains("IntRight")) IntRight.from_json(j.at("IntRight"));
        if (j.contains("BusLoad")) BusLoad.from_json(j.at("BusLoad"));
    }

    json to_json() const {
        return {
            {"ExtLeft", ExtLeft.to_json()},
            {"ExtRight", ExtRight.to_json()},
            {"CANStatus", CANStatus.to_json()},
            {"BytesSent", BytesSent.to_json()},
            {"BytesRecd", BytesRecd.to_json()},
            {"Strokes", Strokes.to_json()},
            {"IntLeft", IntLeft.to_json()},
            {"IntRight", IntRight.to_json()},
            {"BusLoad", BusLoad.to_json()}
        };
    }
};

struct HeartData {
    int StrokeVolume;  // Changed from std::wstring
    MeasurementValue PowerConsumption;
    MeasurementValue IntPressure;
    MeasurementValue MedicalPressure;
    double IntPressureMin;
    double IntPressureMax;
    MeasurementValue CardiacOutput;
    double ActualStrokeLen;  // Changed from std::wstring
    double TargetStrokeLen;  // Changed from std::wstring
    double SensorTemperature;  // Changed from std::wstring
    double ThermistorTemperature;  // Changed from std::wstring
    int CpuLoad;  // Changed from std::wstring
    double OutflowPressure;

    // Default constructor
    HeartData()
        : StrokeVolume(0)
        , IntPressureMin(0.0)
        , IntPressureMax(0.0)
        , ActualStrokeLen(0.0)
        , TargetStrokeLen(0.0)
        , SensorTemperature(0.0)
        , ThermistorTemperature(0.0)
        , CpuLoad(0) 
        , OutflowPressure(0){
    }

    void from_json(const json& j) {
        if (j.contains("StrokeVolume")) j.at("StrokeVolume").get_to(StrokeVolume);
        if (j.contains("PowerConsumption")) PowerConsumption.from_json(j.at("PowerConsumption"));
        if (j.contains("IntPressure")) IntPressure.from_json(j.at("IntPressure"));
        if (j.contains("MedicalPressure")) MedicalPressure.from_json(j.at("MedicalPressure"));
        if (j.contains("IntPressureMin")) j.at("IntPressureMin").get_to(IntPressureMin);
        if (j.contains("IntPressureMax")) j.at("IntPressureMax").get_to(IntPressureMax);
        if (j.contains("CardiacOutput")) CardiacOutput.from_json(j.at("CardiacOutput"));
        if (j.contains("ActualStrokeLen")) j.at("ActualStrokeLen").get_to(ActualStrokeLen);
        if (j.contains("TargetStrokeLen")) j.at("TargetStrokeLen").get_to(TargetStrokeLen);
        if (j.contains("SensorTemperature")) j.at("SensorTemperature").get_to(SensorTemperature);
        if (j.contains("ThermistorTemperature")) j.at("ThermistorTemperature").get_to(ThermistorTemperature);
        if (j.contains("CpuLoad")) j.at("CpuLoad").get_to(CpuLoad);
        if (j.contains("OutflowPressure")) j.at("OutflowPressure").get_to(OutflowPressure);
    }

    json to_json() const {
        try {
            json j;

            j["StrokeVolume"] = StrokeVolume;
            j["ActualStrokeLen"] = ActualStrokeLen;
            j["TargetStrokeLen"] = TargetStrokeLen;
            j["SensorTemperature"] = SensorTemperature;
            j["ThermistorTemperature"] = ThermistorTemperature;
            j["CpuLoad"] = CpuLoad;
            j["OutflowPressure"] = OutflowPressure;
            j["IntPressureMin"] = IntPressureMin;
            j["IntPressureMax"] = IntPressureMax;

            try { j["PowerConsumption"] = PowerConsumption.to_json(); }
            catch (...) { j["PowerConsumption"] = nullptr; }

            try { j["IntPressure"] = IntPressure.to_json(); }
            catch (...) { j["IntPressure"] = nullptr; }

            try { j["MedicalPressure"] = MedicalPressure.to_json(); }
            catch (...) { j["MedicalPressure"] = nullptr; }

            try { j["CardiacOutput"] = CardiacOutput.to_json(); }
            catch (...) { j["CardiacOutput"] = nullptr; }

            return j;
        }
        catch (const std::exception& e) {
            return json::object();
        }
    }
};

class HeartMonitor {
public:
    std::wstring Timestamp;
    std::wstring SystemId;
    StatusData StatusData;
    HeartData LeftHeart;
    HeartData RightHeart;
    int HeartRate;                    // Changed from std::wstring
    std::wstring OperationState;
    std::wstring HeartStatus;
    std::wstring FlowLimitState;
    double FlowLimit;                 // Changed from std::wstring
    double AtmosPressure;            // Changed from std::wstring
    bool UseMedicalSensor;
    MeasurementValue AoPSensor;
    MeasurementValue CVPSensor;
    MeasurementValue PAPSensor;
    MeasurementValue ArtPressSensor;
    std::wstring IVCSensorVal;        // Keeping as string since it can be "-"
    std::wstring LocalClock;
    std::vector<std::wstring> Messages;

    // Default constructor
    HeartMonitor()
        : Timestamp(L"")
        , SystemId(L"")
        , HeartRate(0)
        , OperationState(L"")
        , HeartStatus(L"")
        , FlowLimitState(L"")
        , FlowLimit(0.0)
        , AtmosPressure(0.0)
        , UseMedicalSensor(false)
        , IVCSensorVal(L"")
        , LocalClock(L"")
        , Messages() {
    }

    // Deserialize from JSON
    static HeartMonitor from_json(const std::wstring& json_str) {
        HeartMonitor monitor;
        try {
            json j = json::parse(json_str);

            if (j.contains("Timestamp")) monitor.Timestamp = to_wstring(j.at("Timestamp").get<std::string>());
            if (j.contains("SystemId")) monitor.SystemId = to_wstring(j.at("SystemId").get<std::string>());
            if (j.contains("StatusData")) monitor.StatusData.from_json(j.at("StatusData"));
            if (j.contains("LeftHeart")) monitor.LeftHeart.from_json(j.at("LeftHeart"));
            if (j.contains("RightHeart")) monitor.RightHeart.from_json(j.at("RightHeart"));
            if (j.contains("HeartRate")) j.at("HeartRate").get_to(monitor.HeartRate);
            if (j.contains("OperationState")) monitor.OperationState = to_wstring(j.at("OperationState").get<std::string>());
            if (j.contains("HeartStatus")) monitor.HeartStatus = to_wstring(j.at("HeartStatus").get<std::string>());
            if (j.contains("FlowLimitState")) monitor.FlowLimitState = to_wstring(j.at("FlowLimitState").get<std::string>());
            if (j.contains("FlowLimit")) j.at("FlowLimit").get_to(monitor.FlowLimit);
            if (j.contains("AtmosPressure")) j.at("AtmosPressure").get_to(monitor.AtmosPressure);
        }
        catch (const json::exception& e) {
            throw std::runtime_error(std::string("JSON parsing error: ") + e.what());
        }
        return monitor;
    }

    // Serialize to JSON remains largely the same, but numeric values will be serialized as numbers
    std::wstring to_json() const {
        try {
            json j = {
                {"Timestamp", to_string(Timestamp)},
                {"SystemId", to_string(SystemId)},
                {"StatusData", StatusData.to_json()},
                {"LeftHeart", LeftHeart.to_json()},
                {"RightHeart", RightHeart.to_json()},
                {"HeartRate", HeartRate},
                {"OperationState", to_string(OperationState)},
                {"HeartStatus", to_string(HeartStatus)},
                {"FlowLimitState", to_string(FlowLimitState)},
                {"FlowLimit", FlowLimit},
                {"AtmosPressure", AtmosPressure},
                {"UseMedicalSensor", UseMedicalSensor},
                {"AoPSensor", AoPSensor.to_json()},
                {"CVPSensor", CVPSensor.to_json()},
                {"PAPSensor", PAPSensor.to_json()},
                {"ArtPressSensor", ArtPressSensor.to_json()},
                {"IVCSensorVal", to_string(IVCSensorVal)},
                {"LocalClock", to_string(LocalClock)}
            };

            // Convert the json object to a string and then to wstring
            std::string json_str = j.dump();
            return to_wstring(json_str);
        }
        catch (const std::exception& e) {
            throw std::runtime_error(std::string("JSON serialization error: ") + e.what());
        }
    }
};


