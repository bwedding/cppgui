# Window Configuration Guide

This document provides a comprehensive guide to configuring and customizing windows using the `MakeWindow` class and `MainWindowConfiguration` structure.

## Table of Contents

1. [Basic Configuration](#basic-configuration)
2. [Window Dimensions](#window-dimensions)
3. [Window Appearance](#window-appearance)
4. [Window Style Options](#window-style-options)
5. [Theme Options](#theme-options)
6. [Color Customization](#color-customization)
7. [Border Customization](#border-customization)
8. [Window State Control](#window-state-control)
9. [Complete Examples](#complete-examples)

## Basic Configuration

The `MainWindowConfiguration` structure provides a convenient way to configure all aspects of a window's appearance and behavior. There are multiple ways to use the configuration system:

### Method 1: Using the Configuration Constructor

```cpp
// Create a configuration
MainWindowConfiguration config;
config.width = 1200;
config.height = 800;
config.title = L"My Custom Window";
config.topMost = true;

// Create a window manager with pre-configured settings
MakeWindow makeWindow(hInstance, nCmdShow, config);

// Create the window (all settings applied automatically)
HWND hWnd = makeWindow.CreateMainWindow();
```

### Method 2: Modify Configuration After Construction

```cpp
MakeWindow makeWindow(hInstance, nCmdShow);
auto& config = makeWindow.GetConfiguration();

// Modify config directly
config.width = 1200;
config.height = 800;
config.title = L"My Custom Window";
config.topMost = true;

HWND hWnd = makeWindow.CreateMainWindow();
```

## Window Dimensions

Control the size of your window:

```cpp
config.width = 1024;    // Window width in pixels
config.height = 768;    // Window height in pixels
```

## Window Appearance

Set basic window appearance properties:

```cpp
config.title = L"Application Title";  // Window title text
config.icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));  // Window icon
```

## Window Style Options

Customize window style and behavior:

```cpp
// Basic window style options
config.topMost = true;      // Window stays on top of other windows
config.toolWindow = true;   // Creates a tool window with no taskbar entry
config.layered = true;      // Enables transparency/opacity features
config.opacity = 200;       // Sets window opacity (0-255, 255 is fully opaque)
config.resizable = false;   // Creates a fixed-size window

// Initial window state
config.initialState = WindowState::Normal;     // Normal window
// OR
config.initialState = WindowState::Maximized;  // Start maximized
// OR
config.initialState = WindowState::Minimized;  // Start minimized
```

## Theme Options

Control the window's theme:

```cpp
// Enable or disable theme support
config.themingEnabled = true;  // Enable Windows theming

// Set theme mode
config.themeMode = ThemeMode::System;  // Follow system theme (light/dark)
// OR
config.themeMode = ThemeMode::Light;   // Always use light theme
// OR
config.themeMode = ThemeMode::Dark;    // Always use dark theme
```

## Color Customization

Customize window colors:

```cpp
// Set colors (use CLR_INVALID to use system default)
config.titleBarColor = RGB(30, 30, 30);         // Dark title bar
config.frameColor = RGB(0, 120, 215);           // Blue frame
config.textColor = RGB(255, 255, 255);          // White text
config.captionButtonHoverColor = RGB(200, 0, 0); // Red hover color for buttons
```

> Note: `captionButtonHoverColor` is stored but may not be applied on all Windows versions.

## Border Customization

Adjust the window border:

```cpp
// Set border width (0-20 pixels, -1 for system default)
config.borderWidth = 0;   // No border
// OR
config.borderWidth = 2;   // Thin border
// OR
config.borderWidth = -1;  // System default
```

## Window State Control

Dynamically change window properties after creation:

```cpp
// Change window state
makeWindow.SetWindowState(hWnd, WindowState::Maximized);  // Maximize
makeWindow.SetWindowState(hWnd, WindowState::Normal);     // Restore to normal
makeWindow.SetWindowState(hWnd, WindowState::Minimized);  // Minimize

// Toggle resizable state
makeWindow.SetResizable(hWnd, true);   // Allow resizing
makeWindow.SetResizable(hWnd, false);  // Prevent resizing

// Toggle top-most state
makeWindow.SetTopMost(hWnd, true);   // Make top-most
makeWindow.SetTopMost(hWnd, false);  // Remove top-most

// Change colors
makeWindow.SetTitleBarColor(hWnd, RGB(200, 0, 0));  // Red title bar
makeWindow.SetFrameColor(hWnd, RGB(0, 0, 200));     // Blue frame
makeWindow.SetTextColor(hWnd, RGB(255, 255, 0));    // Yellow text

// Change border width
makeWindow.SetBorderWidth(hWnd, 0);  // Remove border
makeWindow.SetBorderWidth(hWnd, 4);  // Medium border
```

## Complete Examples

### Modern Dark Mode Application

```cpp
// Create a fully configured window with a single block of configuration
MainWindowConfiguration config;
config.width = 800;
config.height = 600;
config.title = L"Dark Mode App";
config.resizable = true;
config.initialState = WindowState::Normal;
config.borderWidth = 1;                 // Thin border
config.themeMode = ThemeMode::Dark;     // Force dark mode
config.titleBarColor = RGB(30, 30, 30); // Dark title bar
config.frameColor = RGB(40, 40, 40);    // Dark frame
config.textColor = RGB(240, 240, 240);  // Light text

MakeWindow makeWindow(hInstance, nCmdShow, config);
HWND hWnd = makeWindow.CreateMainWindow();
```

### Fixed-Size Utility Window

```cpp
MainWindowConfiguration config;
config.width = 400;
config.height = 300;
config.title = L"Utility Window";
config.resizable = false;         // Fixed size
config.topMost = true;            // Always on top
config.toolWindow = true;         // No taskbar entry
config.borderWidth = 0;           // No border
config.themeMode = ThemeMode::System;  // Follow system theme

MakeWindow makeWindow(hInstance, nCmdShow, config);
HWND hWnd = makeWindow.CreateMainWindow();
```

### Semi-Transparent Overlay

```cpp
MainWindowConfiguration config;
config.width = 600;
config.height = 400;
config.title = L"Transparent Overlay";
config.layered = true;            // Enable transparency
config.opacity = 180;             // ~70% opacity
config.topMost = true;            // Always on top
config.borderWidth = 0;           // No border
config.frameColor = RGB(0, 0, 0); // Black frame

MakeWindow makeWindow(hInstance, nCmdShow, config);
HWND hWnd = makeWindow.CreateMainWindow();
```

### Themed Application with System Integration

```cpp
MainWindowConfiguration config;
config.width = 1024;
config.height = 768;
config.title = L"Themed Application";
config.resizable = true;
config.initialState = WindowState::Maximized;  // Start maximized
config.themingEnabled = true;
config.themeMode = ThemeMode::System;          // Follow system theme

MakeWindow makeWindow(hInstance, nCmdShow, config);
HWND hWnd = makeWindow.CreateMainWindow();

// Later, respond to system theme changes automatically
// The window will update when the system theme changes between light/dark
```

### Custom Title Bar Integration

For applications using a custom title bar implementation:

```cpp
MainWindowConfiguration config;
config.width = 1024;
config.height = 768;
config.title = L"Custom Title Bar App";
config.resizable = true;

// Use no border for a clean look with custom title bar
config.borderWidth = 0;                

// Set frame color to match custom title bar background
config.frameColor = RGB(30, 30, 30);    // Match dark title bar background

MakeWindow makeWindow(hInstance, nCmdShow, config);
HWND hWnd = makeWindow.CreateMainWindow();

// In your WndProc, check resizable state before allowing maximize
if (makeWindow.IsResizable()) {
    // Enable maximize button in custom title bar
} else {
    // Disable maximize button in custom title bar
}