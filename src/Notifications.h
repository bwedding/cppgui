#pragma once
#include <Windows.h>
#include <iostream>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.UI.Notifications.h>
#include "spdlog.h"
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::Data::Xml::Dom;

class Notifications
{
	XmlDocument m_defaultXml;
    ToastNotification m_toast{ nullptr };
    // Store event token to properly unregister handlers
	winrt::event_token m_activatedToken;
	winrt::event_token m_dismissedToken;
    winrt::event_token m_failedToken;

    bool m_hasToast = false;

public:
	Notifications() = default;
    ~Notifications() {
        // Unregister handlers if toast exists
        if (m_hasToast) {
            try {
                if (m_activatedToken) {
                    m_toast.Activated(m_activatedToken);
                }
                if (m_dismissedToken) {
                    m_toast.Dismissed(m_dismissedToken);
                }
                if (m_failedToken) {
                    m_toast.Failed(m_failedToken);
                }
            }
            catch (...) {
                // Ignore errors during cleanup
            }
        }
    }
	void ShowModernToastNotification(const std::wstring& title, const std::wstring& message);
	void ShowImageToastNotification(const std::wstring& title, const std::wstring& message, const std::wstring& imagePath);
	void ShowRichToastNotification();
	void AddEventHandlers();

};

