// include/Squidl/core/UIEvent.h
#pragma once

#include "Squidl/SquidlConfig.h" // For SQUIDL_API
#include "Squidl/utils/Point.h"  // For Squidl::Utils::Point
#include <SDL.h>  // For SDL_Scancode, SDL_Keymod, SDL_BUTTON_LEFT etc.
#include <string> // For std::string

namespace Squidl::Core {

    /**
     * @brief Enum для типов событий UI.
     * @ingroup Core
     */
    enum class EventType {
        None = 0,
        MouseEvent,
        KeyboardEvent,
        TextInputEvent,
        ResizeEvent, // {{ edit_1 }} Added new event type
        // Добавьте другие типы событий по мере необходимости (например,
        // FocusEvent)
    };

    /**
     * @brief Базовая структура для всех событий UI.
     * @ingroup Core
     */
    struct SQUIDL_API UIEvent {
        EventType type = EventType::None;
        bool handled = false; // Флаг, указывающий, было ли событие обработано
                              // (позволяет остановить распространение)
    };

    /**
     * @brief Enum для типов событий мыши.
     * @ingroup Core
     */
    enum class MouseEventType {
        Moved,
        ButtonPressed,
        ButtonReleased,
        Wheel,
    };

    /**
     * @brief Структура события мыши.
     * @ingroup Core
     */
    struct SQUIDL_API MouseEvent : public UIEvent {
        MouseEventType mouseEventType;
        Squidl::Utils::Point position; // Позиция мыши
        Uint8 button; // Кнопка мыши (SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT и т.д.)
        int clicks;   // Количество кликов (для двойного клика)
        int wheelX, wheelY; // Смещение колеса мыши

        // Fixed: Explicitly call the Point constructor to avoid ambiguity
        MouseEvent(MouseEventType type, int x, int y, Uint8 btn = 0,
                   int clks = 0, int wx = 0, int wy = 0)
            : mouseEventType(type), position(x, y), button(btn), clicks(clks),
              wheelX(wx), wheelY(wy) {
            this->type = EventType::MouseEvent;
        }
    };

    /**
     * @brief Структура события клавиатуры.
     * @ingroup Core
     */
    struct SQUIDL_API KeyboardEvent : public UIEvent {
        SDL_Scancode scancode; // Скан-код клавиши
        SDL_Keymod keymod;     // Модификаторы (Shift, Ctrl, Alt)
        bool isPressed; // true, если клавиша нажата; false, если отпущена

        KeyboardEvent(SDL_Scancode sc, SDL_Keymod km, bool pressed)
            : scancode(sc), keymod(km), isPressed(pressed) {
            this->type = EventType::KeyboardEvent;
        }
    };

    /**
     * @brief Структура события текстового ввода.
     * @ingroup Core
     */
    struct SQUIDL_API TextInputEvent : public UIEvent {
        std::string text; // Введенный текст (UTF-8)

        TextInputEvent(const std::string &t) : text(t) {
            this->type = EventType::TextInputEvent;
        }
    };

    /**
     * @brief Структура события изменения размеров окна.
     * @ingroup Core
     */
    struct SQUIDL_API ResizeEvent : public UIEvent {
        int width;  // {{ edit_2 }} New width of the window
        int height; // {{ edit_3 }} New height of the window

        ResizeEvent(int w, int h)
            : width(w), height(h) {
            this->type = EventType::ResizeEvent;
        }
    };

} // namespace Squidl::Core
