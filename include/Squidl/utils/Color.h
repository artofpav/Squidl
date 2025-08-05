// include/Squidl/utils/Color.h
#pragma once

#include "Squidl/SquidlConfig.h" // For SQUIDL_API
#include <SDL.h>                 // For SDL_Color

namespace Squidl::Utils {

    /**
     * @brief Represents a color with RGBA components.
     * @ingroup Utils
     */
    struct SQUIDL_API Color {
        Uint8 r, g, b, a;

        // Constructors
        Color() : r(0), g(0), b(0), a(255) {}
        Color(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a = 255)
            : r(_r), g(_g), b(_b), a(_a) {}
        Color(const SDL_Color &sdlColor)
            : r(sdlColor.r), g(sdlColor.g), b(sdlColor.b), a(sdlColor.a) {}
        // Lighter and darker functions
        Color lighter(float amount) const {
            float new_r = static_cast<float>(r) +
                          (255.0f - static_cast<float>(r)) * amount;
            float new_g = static_cast<float>(g) +
                          (255.0f - static_cast<float>(g)) * amount;
            float new_b = static_cast<float>(b) +
                          (255.0f - static_cast<float>(b)) * amount;
            return Color(static_cast<Uint8>(std::fmin(255.0f, new_r)),
                         static_cast<Uint8>(std::fmin(255.0f, new_g)),
                         static_cast<Uint8>(std::fmin(255.0f, new_b)), a);
        }

        Color darker(float amount) const {
            float new_r = static_cast<float>(r) * (1.0f - amount);
            float new_g = static_cast<float>(g) * (1.0f - amount);
            float new_b = static_cast<float>(b) * (1.0f - amount);
            return Color(static_cast<Uint8>(std::fmax(0.0f, new_r)),
                         static_cast<Uint8>(std::fmax(0.0f, new_g)),
                         static_cast<Uint8>(std::fmax(0.0f, new_b)), a);
        }

        // Conversion operator to SDL_Color
        operator SDL_Color() const { return {r, g, b, a}; }

        // Predefined colors (static methods for convenience)
        static Color Red() { return {255, 0, 0, 255}; }
        static Color Green() { return {0, 255, 0, 255}; }
        static Color Blue() { return {0, 0, 255, 255}; }
        static Color Black() { return {0, 0, 0, 255}; }
        static Color White() { return {255, 255, 255, 255}; }
        static Color Gray() { return {128, 128, 128, 255}; }
        static Color Transparent() { return {0, 0, 0, 0}; }
    };

} // namespace Squidl::Utils
