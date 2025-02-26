#pragma once
#include <windows.h>
#include <dbt.h>
#include <setupapi.h>
#include <initguid.h>
#include <usbiodef.h>
#include <cfgmgr32.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>

#pragma comment(lib, "setupapi.lib")

class UsbDevice {
public:
    struct DeviceInfo {
        std::wstring devicePath;
        std::wstring friendlyName;
        std::wstring description;
        std::wstring driveLetter;
        std::wstring vendorId;
        std::wstring productId;
        std::wstring serialNumber;
        DEVINST deviceInstance = 0;
    };

    using DeviceChangeCallback = std::function<void(const DeviceInfo&, bool)>;

    explicit UsbDevice(const std::wstring& devicePath) : m_devicePath(devicePath) {
        Initialize();
    }

    ~UsbDevice() {
        if (m_deviceHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_deviceHandle);
        }
    }

    // Get device properties
    bool Initialize() {
        m_deviceHandle = CreateFileW(m_devicePath.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr);

        if (m_deviceHandle == INVALID_HANDLE_VALUE) {
            return false;
        }

        return RefreshDeviceInfo();
    }

    bool RefreshDeviceInfo() {
        HDEVINFO deviceInfo = SetupDiGetClassDevs(
            &GUID_DEVINTERFACE_USB_DEVICE,
            nullptr,
            nullptr,
            DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

        if (deviceInfo == INVALID_HANDLE_VALUE) {
            return false;
        }

        SP_DEVINFO_DATA deviceInfoData = { sizeof(SP_DEVINFO_DATA) };
        SP_DEVICE_INTERFACE_DATA interfaceData = { sizeof(SP_DEVICE_INTERFACE_DATA) };

        for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfo, nullptr,
            &GUID_DEVINTERFACE_USB_DEVICE, i, &interfaceData); i++) {

            DWORD requiredSize = 0;
            SetupDiGetDeviceInterfaceDetailW(deviceInfo, &interfaceData, nullptr, 0, &requiredSize, &deviceInfoData);

            auto detailData = std::make_unique<char[]>(requiredSize);
            auto pDetailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_W>(detailData.get());
            pDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

            if (SetupDiGetDeviceInterfaceDetailW(deviceInfo, &interfaceData,
                pDetailData, requiredSize, &requiredSize, &deviceInfoData)) {

                if (std::wstring(pDetailData->DevicePath) == m_devicePath) {
                    m_deviceInfo.devicePath = m_devicePath;
                    m_deviceInfo.deviceInstance = deviceInfoData.DevInst;
                    GetDeviceProperties(deviceInfo, deviceInfoData);
                    GetDriveLetter();
                    ParseDevicePath();
                    break;
                }
            }
        }

        SetupDiDestroyDeviceInfoList(deviceInfo);
        return true;
    }

    const DeviceInfo& GetInfo() const {
        return m_deviceInfo;
    }

    // Static methods for device enumeration and notification
    static std::vector<DeviceInfo> EnumerateDevices() {
        std::vector<DeviceInfo> devices;
        HDEVINFO deviceInfo = SetupDiGetClassDevs(
            &GUID_DEVINTERFACE_USB_DEVICE,
            nullptr,
            nullptr,
            DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

        if (deviceInfo == INVALID_HANDLE_VALUE) {
            return devices;
        }

        SP_DEVICE_INTERFACE_DATA interfaceData = { sizeof(SP_DEVICE_INTERFACE_DATA) };
        SP_DEVINFO_DATA deviceInfoData = { sizeof(SP_DEVINFO_DATA) };

        for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfo, nullptr,
            &GUID_DEVINTERFACE_USB_DEVICE, i, &interfaceData); i++) {

            DWORD requiredSize = 0;
            SetupDiGetDeviceInterfaceDetailW(deviceInfo, &interfaceData, nullptr, 0, &requiredSize, &deviceInfoData);

            auto detailData = std::make_unique<char[]>(requiredSize);
            auto pDetailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_W>(detailData.get());
            pDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

            if (SetupDiGetDeviceInterfaceDetailW(deviceInfo, &interfaceData,
                pDetailData, requiredSize, &requiredSize, &deviceInfoData)) {

                UsbDevice device(pDetailData->DevicePath);
                devices.push_back(device.GetInfo());
            }
        }

        SetupDiDestroyDeviceInfoList(deviceInfo);
        return devices;
    }

    static HDEVNOTIFY RegisterDeviceNotification(HWND hwnd, const DeviceChangeCallback& callback) {
        DEV_BROADCAST_DEVICEINTERFACE filter = { 0 };
        filter.dbcc_size = sizeof(filter);
        filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        filter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;

        // Store callback
        s_deviceChangeCallback = callback;

        return ::RegisterDeviceNotification(
            hwnd,
            &filter,
            DEVICE_NOTIFY_WINDOW_HANDLE
        );
    }

    // Window procedure helper for handling device notifications
    static LRESULT HandleDeviceChange(WPARAM wParam, LPARAM lParam) {
        if (s_deviceChangeCallback) {
            if (wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE) {
                auto pDev = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
                if (pDev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                    auto pDevInf = reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(pDev);
                    UsbDevice device(pDevInf->dbcc_name);
                    s_deviceChangeCallback(device.GetInfo(), wParam == DBT_DEVICEARRIVAL);
                }
            }
        }
        return 0;
    }

private:
    void GetDeviceProperties(HDEVINFO deviceInfo, SP_DEVINFO_DATA& deviceInfoData) {
        DWORD dataType;
        DWORD size = 0;

        // Get friendly name
        SetupDiGetDeviceRegistryPropertyW(deviceInfo, &deviceInfoData,
            SPDRP_FRIENDLYNAME, &dataType, nullptr, 0, &size);
        if (size > 0) {
            std::vector<WCHAR> buffer(size / sizeof(WCHAR));
            if (SetupDiGetDeviceRegistryPropertyW(deviceInfo, &deviceInfoData,
                SPDRP_FRIENDLYNAME, &dataType,
                reinterpret_cast<PBYTE>(buffer.data()), size, nullptr)) {
                m_deviceInfo.friendlyName = buffer.data();
            }
        }

        // Get description
        size = 0;
        SetupDiGetDeviceRegistryPropertyW(deviceInfo, &deviceInfoData,
            SPDRP_DEVICEDESC, &dataType, nullptr, 0, &size);
        if (size > 0) {
            std::vector<WCHAR> buffer(size / sizeof(WCHAR));
            if (SetupDiGetDeviceRegistryPropertyW(deviceInfo, &deviceInfoData,
                SPDRP_DEVICEDESC, &dataType,
                reinterpret_cast<PBYTE>(buffer.data()), size, nullptr)) {
                m_deviceInfo.description = buffer.data();
            }
        }
    }

    void GetDriveLetter() {
        WCHAR volumeName[MAX_PATH];
        HANDLE volumeHandle = FindFirstVolumeW(volumeName, ARRAYSIZE(volumeName));
        if (volumeHandle == INVALID_HANDLE_VALUE) {
            return;
        }

        do {
            DWORD charCount = MAX_PATH + 1;
            WCHAR deviceName[MAX_PATH + 1];
            if (QueryDosDeviceW(&volumeName[4], deviceName, charCount)) {
                if (m_devicePath.find(deviceName) != std::wstring::npos) {
                    WCHAR driveLetter[3] = { 0 };
                    if (GetVolumePathNamesForVolumeNameW(volumeName, driveLetter, 3, &charCount) && charCount > 0) {
                        m_deviceInfo.driveLetter = driveLetter;
                        break;
                    }
                }
            }
        } while (FindNextVolumeW(volumeHandle, volumeName, ARRAYSIZE(volumeName)));

        FindVolumeClose(volumeHandle);
    }

    void ParseDevicePath() {
        // Parse VID, PID, and Serial from device path
        // Format: \\?\USB#VID_xxxx&PID_xxxx#serial#{guid}
        size_t vidPos = m_devicePath.find(L"VID_");
        size_t pidPos = m_devicePath.find(L"PID_");
        if (vidPos != std::wstring::npos && pidPos != std::wstring::npos) {
            m_deviceInfo.vendorId = m_devicePath.substr(vidPos + 4, 4);
            m_deviceInfo.productId = m_devicePath.substr(pidPos + 4, 4);

            size_t serialStart = pidPos + 9;
            size_t serialEnd = m_devicePath.find(L'#', serialStart);
            if (serialEnd != std::wstring::npos) {
                m_deviceInfo.serialNumber = m_devicePath.substr(serialStart, serialEnd - serialStart);
            }
        }
    }

    std::wstring m_devicePath;
    HANDLE m_deviceHandle = INVALID_HANDLE_VALUE;
    DeviceInfo m_deviceInfo;
    static inline DeviceChangeCallback s_deviceChangeCallback;
};
