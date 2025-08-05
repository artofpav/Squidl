#pragma once
#include <SDL.h>

namespace Squidl::Managers {

    struct ButtonStyle {
        SDL_Color normalColor = {100, 100, 100, 255};
        SDL_Color hoverColor = {130, 130, 130, 255};
        SDL_Color pressedColor = {160, 100, 100, 255};
        SDL_Color selectedColor = {200, 180, 80, 255};
        SDL_Color disabledColor = {80, 80, 80, 255};
        SDL_Color borderColor = {0, 0, 0, 255};
        SDL_Color textColor = {255, 255, 255, 255};
    };

    struct UITheme {
        ButtonStyle button;

        static UITheme Light();
        static UITheme Dark();
    };
} // namespace Squidl::Managers