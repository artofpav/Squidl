// include/Squidl/utils/Point.h
#pragma once

#include "Squidl/SquidlConfig.h" // Для SQUIDL_API
#include <SDL.h>                 // Для SDL_Point
#include <cmath>                 // Для sqrt, pow

namespace Squidl::Utils {
    /**
     * @brief Представляет двумерную точку или вектор с целочисленными
     * координатами.
     * @ingroup Utils
     */
    struct SQUIDL_API Point {
        int x, y;

        // Конструкторы
        Point() : x(0), y(0) {}
        Point(int _x, int _y) : x(_x), y(_y) {}
        Point(const SDL_Point &p) : x(p.x), y(p.y) {}

        // Преобразование в SDL_Point
        operator SDL_Point() const { return {x, y}; }

        // Операторы
        Point operator+(const Point &other) const {
            return Point(x + other.x, y + other.y);
        }
        Point operator-(const Point &other) const {
            return Point(x - other.x, y - other.y);
        }
        Point operator*(int scalar) const {
            return Point(x * scalar, y * scalar);
        }
        Point operator/(int scalar) const {
            return Point(x / scalar, y / scalar);
        }

        void operator+=(const Point &other) {
            x += other.x;
            y += other.y;
        }
        void operator-=(const Point &other) {
            x -= other.x;
            y -= other.y;
        }
        void operator*=(int scalar) {
            x *= scalar;
            y *= scalar;
        }
        void operator/=(int scalar) {
            x /= scalar;
            y /= scalar;
        }

        bool operator==(const Point &other) const {
            return x == other.x && y == other.y;
        }
        bool operator!=(const Point &other) const { return !(*this == other); }

        // Дополнительные методы
        float magnitude() const {
            return std::sqrt(static_cast<float>(x * x + y * y));
        }
        float distance(const Point &other) const {
            return std::sqrt(std::pow(static_cast<float>(x - other.x), 2) +
                             std::pow(static_cast<float>(y - other.y), 2));
        }
    };
} // namespace Squidl::Utils