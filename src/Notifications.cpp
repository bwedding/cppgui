#include "notifications.h"

void Notifications::ShowModernToastNotification(const std::wstring& title, const std::wstring& message) 
{
    // Create toast XML
    XmlDocument toastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastText02);

    // Set text content
    XmlNodeList textElements = toastXml.GetElementsByTagName(L"text");
    auto titleNode = textElements.Item(0).as<IXmlNode>();
    titleNode.AppendChild(toastXml.CreateTextNode(title));
    auto messageNode = textElements.Item(1).as<IXmlNode>();
    messageNode.AppendChild(toastXml.CreateTextNode(message));

    // Create toast
    m_toast = ToastNotification(toastXml);
    m_hasToast = true;

    // Add event handlers
    AddEventHandlers();

    // Show the toast
    try {
        ToastNotificationManager::CreateToastNotifier(L"YourAppId").Show(m_toast);
    }
    catch (const winrt::hresult_error& ex) {
        std::wcout << L"Failed to show toast: " << std::hex << ex.code() << L" - "
            << ex.message().c_str() << std::endl;
    }
}

void Notifications::ShowImageToastNotification(const std::wstring& title, const std::wstring& message, const std::wstring& imagePath) {
    // Create toast XML with image
    XmlDocument toastXml = ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastImageAndText02);

    // Set text content
    XmlNodeList textElements = toastXml.GetElementsByTagName(L"text");
    auto titleNode = textElements.Item(0).as<IXmlNode>();
    titleNode.AppendChild(toastXml.CreateTextNode(title));
    auto messageNode = textElements.Item(1).as<IXmlNode>();
    messageNode.AppendChild(toastXml.CreateTextNode(message));

    // Set image
    XmlNodeList imageElements = toastXml.GetElementsByTagName(L"image");
    auto imageNode = imageElements.Item(0).as<IXmlNode>();
    auto srcAttribute = imageNode.Attributes().GetNamedItem(L"src");
    srcAttribute.NodeValue(winrt::box_value(imagePath));

    // Create toast and store it
    m_toast = ToastNotification(toastXml);

    // Add event handlers
    AddEventHandlers();

    // Show the toast
    try {
        ToastNotificationManager::CreateToastNotifier(L"YourAppId").Show(m_toast);
    }
    catch (const winrt::hresult_error& ex) {
        std::wcout << L"Failed to show toast: " << std::hex << ex.code() << L" - "
            << ex.message().c_str() << std::endl;
    }
}

void Notifications::ShowRichToastNotification() {
    auto toastXml = winrt::Windows::Data::Xml::Dom::XmlDocument();
    std::wstring xmlContent = LR"(<toast launch="YourAppId">
    <visual>
        <binding template="ToastGeneric">
            <text>Rich Toast Title</text>
            <text>This is a rich toast message</text>
            
        </binding>
    </visual>
    <audio src="ms-winsoundevent:Notification.Default"/>
</toast>)";
    toastXml.LoadXml(xmlContent);
    OutputDebugString(toastXml.GetXml().c_str());

    OutputDebugString(L"XML created successfully\n");

    // Create toast
    m_toast = ToastNotification(toastXml);
    m_hasToast = true;

    // Add event handlers
    AddEventHandlers();

    // Show the toast with extra error logging
    try {
        auto notifier = ToastNotificationManager::CreateToastNotifier(L"YourAppId");
        notifier.Show(m_toast);
        OutputDebugString(L"Toast shown successfully\n");
    }
    catch (const winrt::hresult_error& ex) {
        std::wcout << L"Failed to show toast: " << std::hex << ex.code() << L" - "
            << ex.message().c_str() << std::endl;
        OutputDebugString((L"Failed to show toast: " + winrt::to_hstring(ex.code()) + L" - "
            + winrt::to_hstring(ex.message().c_str())).c_str());
    }
}

void Notifications::AddEventHandlers() 
{
    // Handle activation (click)
    m_activatedToken = m_toast.Activated([this](const auto& sender, const auto& args) {
        OutputDebugString(L"Toast activated\n");
        spdlog::info("Toast activated");
        // You can get the arguments from args to determine which action was clicked
        // Handle the action here
        });

    // Handle dismissal
    m_dismissedToken = m_toast.Dismissed([this](const auto& sender, ToastDismissedEventArgs args) {
        spdlog::info("Toast dismissed");
        auto reason = args.Reason();
        switch (reason) {
        case ToastDismissalReason::ApplicationHidden:
            OutputDebugString(L"Toast dismissed: Application Hidden\n");
            break;
        case ToastDismissalReason::UserCanceled:
            OutputDebugString(L"Toast dismissed: User Canceled\n");
            break;
        case ToastDismissalReason::TimedOut:
            OutputDebugString(L"Toast dismissed: Timed Out\n");
            break;
        default:
            OutputDebugString(L"Toast dismissed: Unknown reason\n");
            break;
        }
        });

    // You can also add a Failed handler if needed
    m_toast.Failed([this](const auto& sender, const auto& args) {
        OutputDebugString(L"Toast failed to display\n");
        });
}
