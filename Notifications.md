Key Customization Options
Styling Text

Use hint-style with values like "title", "subtitle", "body", "captionSubtle", etc.
Use hint-align with values like "left", "center", "right"
Use hint-maxLines to control text wrapping

Background Colors
xmlCopy<binding template="ToastGeneric" backgroundColor="#FF0000">
Button Styles
xmlCopy<action activationType="foreground" content="View" arguments="action=view" 
        imageUri="file:///C:/path/to/button-icon.png" 
        hint-buttonStyle="success"/>
Button styles: "default", "success", "critical"
Progress Bars
xmlCopy<progress title="Downloading..." value="0.6" valueStringOverride="60%" status="Downloading..."/>
Using Images from WebView2
If you want to use images from your WebView2 content:

Either save the images to a local path first
Or use data URIs for small images:

xmlCopy<image src="data:image/png;base64,iVBORw0KGgoAAA..."/>
Handling Notification Events
To handle user interaction with notifications:
cppCopy// Set up event handler
toast.Activated([](const auto& sender, const auto& args) {
    // Handle activation
});

toast.Dismissed([](const auto& sender, const auto& args) {
    // Handle dismissal
});

toast.Failed([](const auto& sender, const auto& args) {
    // Handle failure
});