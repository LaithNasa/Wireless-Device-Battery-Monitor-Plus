#include "icons.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <map>
#include <vector>

// Code points verified against the Segoe Fluent Icons list. All are in the E7-E9 range,
// which is shared with Segoe MDL2 Assets (the Windows 10 fallback font).
const IconChoice kIconChoices[] = {
    { 0xE962, L"Mouse" },
    { 0xE7F6, L"Headphones" },
    { 0xE95B, L"Headset with mic" },
    { 0xE7F5, L"Speakers" },
    { 0xE765, L"Keyboard" },
    { 0xE7FC, L"Game controller" },
    { 0xE967, L"Game console" },
    { 0xE8EA, L"Phone" },
    { 0xE70A, L"Tablet" },
    { 0xE977, L"PC" },
    { 0xE7F4, L"Monitor" },
    { 0xE772, L"Generic device" },
    { 0xE702, L"Bluetooth" },
    { 0xE701, L"Wi-Fi" },
    { 0xE88E, L"USB" },
};
const int kIconChoiceCount = (int)(sizeof(kIconChoices) / sizeof(kIconChoices[0]));

wchar_t DefaultGlyph(DeviceKind kind, bool bluetooth) {
    switch (kind) {
        case DeviceKind::Headset:  return 0xE7F6;
        case DeviceKind::Mouse:    return 0xE962;
        case DeviceKind::Keyboard: return 0xE765;
        case DeviceKind::Gamepad:  return 0xE7FC;
        case DeviceKind::Phone:    return 0xE8EA;
        default:                   return bluetooth ? 0xE702 : 0xE772;
    }
}

namespace {

const wchar_t* kSettingsKey = L"Software\\WirelessBattery";
const wchar_t* kIconsValue = L"DeviceIcons";  // REG_MULTI_SZ: key, hex glyph, key, hex glyph, ...

std::map<std::wstring, wchar_t> g_overrides;

bool ParseHex(const wchar_t* s, wchar_t& out) {
    if (!s || !*s) return false;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(s, &end, 16);
    if (end == s || *end || v > 0xFFFF) return false;
    out = (wchar_t)v;
    return true;
}

}  // namespace

void LoadDeviceIcons() {
    g_overrides.clear();
    DWORD size = 0;
    if (RegGetValueW(HKEY_CURRENT_USER, kSettingsKey, kIconsValue, RRF_RT_REG_MULTI_SZ, nullptr, nullptr, &size) != ERROR_SUCCESS || !size)
        return;
    std::vector<wchar_t> buf(size / sizeof(wchar_t) + 1, 0);
    if (RegGetValueW(HKEY_CURRENT_USER, kSettingsKey, kIconsValue, RRF_RT_REG_MULTI_SZ, nullptr, buf.data(), &size) != ERROR_SUCCESS)
        return;
    const wchar_t* p = buf.data();
    while (*p) {
        std::wstring key = p;
        p += wcslen(p) + 1;
        if (!*p) break;  // truncated pair
        wchar_t glyph;
        if (ParseHex(p, glyph)) g_overrides[key] = glyph;
        p += wcslen(p) + 1;
    }
}

void SaveDeviceIcons() {
    std::wstring multi;
    for (const auto& kv : g_overrides) {
        wchar_t hex[8];
        swprintf(hex, 8, L"%04X", (unsigned)kv.second);
        multi += kv.first;
        multi.push_back(L'\0');
        multi += hex;
        multi.push_back(L'\0');
    }
    multi.push_back(L'\0');
    RegSetKeyValueW(HKEY_CURRENT_USER, kSettingsKey, kIconsValue, REG_MULTI_SZ, multi.data(),
                    (DWORD)(multi.size() * sizeof(wchar_t)));
}

wchar_t DeviceGlyph(const std::wstring& key, DeviceKind kind, bool bluetooth) {
    auto it = g_overrides.find(key);
    return it != g_overrides.end() ? it->second : DefaultGlyph(kind, bluetooth);
}

bool HasDeviceIcon(const std::wstring& key) { return g_overrides.count(key) != 0; }

void SetDeviceIcon(const std::wstring& key, wchar_t glyph) {
    if (glyph)
        g_overrides[key] = glyph;
    else
        g_overrides.erase(key);
    SaveDeviceIcons();
}
