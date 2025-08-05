#include "Squidl/managers/UITheme.h"

namespace Squidl::Managers {
    UITheme UITheme::Light() {
        UITheme theme;
        theme.button = {
            {200, 200, 200, 255}, // normal
            {220, 220, 220, 255}, // hover
            {180, 180, 180, 255}, // pressed
            {255, 230, 80, 255},  // selected
            {120, 120, 120, 255}, // disabled
            {0, 0, 0, 255},       // border
            {0, 0, 0, 255}        // text
        };
        return theme;
    }

    UITheme UITheme::Dark() {
        UITheme theme;
        theme.button = {
            {100, 150, 100, 255}, // normal
            {130, 100, 130, 255}, // hover
            {160, 100, 100, 255}, // pressed
            {200, 180, 80, 255},  // selected
            {80, 80, 80, 255},    // disabled
            {255, 255, 255, 255}, // border
            {255, 255, 255, 255}  // text
        };
        return theme;
    }
}
