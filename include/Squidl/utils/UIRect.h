// include/Squidl/utils/UIRect.h
#pragma once

#include "Point.h"               // For Squidl::Utils::Point
#include "Squidl/SquidlConfig.h" // For SQUIDL_API
#include <SDL.h>                 // For SDL_Rect

namespace Squidl::Utils {

    /**
     * @brief Represents a rectangle with integer coordinates and dimensions.
     * @ingroup Utils
     */
    struct SQUIDL_API UIRect {
        int x, y, w, h;

        // Constructors
        UIRect() : x(0), y(0), w(0), h(0) {}
        UIRect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
        UIRect(const SDL_Rect &sdlRect)
            : x(sdlRect.x), y(sdlRect.y), w(sdlRect.w), h(sdlRect.h) {}

        // Conversion operator to SDL_Rect
        operator SDL_Rect() const { return {x, y, w, h}; }

        // Check if a point is inside the rectangle
        bool contains(int pointX, int pointY) const {
            return (pointX >= x && pointX < (x + w) && pointY >= y &&
                    pointY < (y + h));
        }

        bool contains(Point p) const { return contains(p.x, p.y); }

        // Get top-left point
        Point getPosition() const { return {x, y}; }

        // Get center point
        Point getCenter() const { return {x + w / 2, y + h / 2}; }
    };

} // namespace Squidl::Utils
