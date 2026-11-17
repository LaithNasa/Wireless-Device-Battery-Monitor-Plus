#pragma once
// Selectable device icons. The glyphs are Segoe Fluent Icons (with a Segoe MDL2 Assets
// fallback on Windows 10), so no image assets are needed. A per-device override is stored
// in the registry, keyed by DeviceStatus::key, and shared by the taskbar widget and the
// tray icons.
#include <string>
#include "devices.h"

struct IconChoice {
    wchar_t glyph;         // font code point (0 = "use the type's default")
    const wchar_t* label;  // shown in the menu
};

extern const IconChoice kIconChoices[];
extern const int kIconChoiceCount;

// What the app shows before the user picks anything for a device type.
wchar_t DefaultGlyph(DeviceKind kind, bool bluetooth);

// Per-device overrides, loaded from the registry at startup.
void LoadDeviceIcons();
void SaveDeviceIcons();
wchar_t DeviceGlyph(const std::wstring& key, DeviceKind kind, bool bluetooth);
bool HasDeviceIcon(const std::wstring& key);
// glyph == 0 clears the override (back to the type's default).
void SetDeviceIcon(const std::wstring& key, wchar_t glyph);
