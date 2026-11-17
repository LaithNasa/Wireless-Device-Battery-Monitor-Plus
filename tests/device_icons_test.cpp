// Checks the device icon palette: every type's default must be selectable, and the
// palette must not contain duplicate code points. No registry access is exercised.
#include "../src/icons.h"

#include <cstdio>
#include <cstdlib>
#include <set>

namespace {
void Check(bool condition, const char* expression, int line) {
    if (!condition) {
        std::fprintf(stderr, "Check failed: %s (line %d)\n", expression, line);
        std::exit(EXIT_FAILURE);
    }
}
#define CHECK(condition) Check((condition), #condition, __LINE__)
}  // namespace

int main() {
    std::set<wchar_t> glyphs;
    for (int i = 0; i < kIconChoiceCount; i++) {
        CHECK(kIconChoices[i].glyph != 0);
        CHECK(kIconChoices[i].label != nullptr);
        CHECK(glyphs.insert(kIconChoices[i].glyph).second);  // no duplicates
    }

    // The defaults the app used before per-device icons existed must all be in the palette,
    // otherwise a freshly connected device would show a "Custom" icon it cannot re-select.
    const DeviceKind kinds[] = { DeviceKind::Other, DeviceKind::Mouse, DeviceKind::Headset,
                                 DeviceKind::Keyboard, DeviceKind::Gamepad, DeviceKind::Phone };
    for (DeviceKind k : kinds) {
        for (bool bluetooth : { false, true }) {
            wchar_t g = DefaultGlyph(k, bluetooth);
            CHECK(glyphs.count(g) == 1);
            CHECK(DeviceGlyph(L"test-device", k, bluetooth) == g);  // no override load => default
        }
    }

    std::printf("device_icons_test: OK (%d icons)\n", kIconChoiceCount);
    return 0;
}
