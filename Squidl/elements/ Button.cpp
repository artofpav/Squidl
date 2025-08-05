#include "Squidl/core/IRenderer.h"
#include "Squidl/core/UIAlignment.h"
#include "Squidl/core/UIContext.h"
#include "Squidl/elements/Backdrop.h"
#include "Squidl/elements/Button.h"
#include "Squidl/elements/Label.h"
#include "Squidl/utils/Color.h"
#include "Squidl/utils/Logger.h"
#include "Squidl/utils/UIRect.h"
#include <SDL_ttf.h>
#include <algorithm>
#include <sstream>

namespace Squidl::Elements {

    Button::Button(std::string labelText, int x, int y, int w, int h,
                   TTF_Font *font, bool isBorderless)
        : label(std::make_shared<Squidl::Elements::Label>(labelText, x, y, w, h,
                                                          font)) {
        setRect(Squidl::Utils::UIRect(x, y, w, h));
        borderless = isBorderless;
        setFont(font);
        label->setHorizontalAlignment(Squidl::Core::HorizontalAlign::Center);
        label->setVerticalAlignment(Squidl::Core::VerticalAlign::Center);
        setBackgroundColor(Squidl::Utils::Color(100, 100, 100, 255));
        setBorderColor(Squidl::Utils::Color(0, 0, 0, 255));
    }

    void Button::setLabelText(const std::string &text) {
        if (label)
            label->setText(text);
    }

    // Обработка событий в отдельном методе
    void Button::onEvent(Squidl::Core::UIEvent &event) {

        if (!enabled) { //TODO Remove to managed enable events
            return;
        }

        // Проверяем, является ли событие событием мыши
        if (event.type == Squidl::Core::EventType::MouseEvent) {
            auto &mouseEvent = static_cast<Squidl::Core::MouseEvent &>(event);
            Squidl::Utils::UIRect currentRect = getRect();
            bool mouseOver = currentRect.contains(mouseEvent.position.x,
                                                  mouseEvent.position.y);

            hovered = mouseOver;

            //TODO optimize code under
            if (mouseEvent.mouseEventType ==
                    Squidl::Core::MouseEventType::ButtonPressed &&
                mouseOver && mouseEvent.button == SDL_BUTTON_LEFT) {
                pressed = true;
                event.handled = true; // Останавливаем распространение
            }
            if (mouseEvent.mouseEventType ==
                    Squidl::Core::MouseEventType::ButtonReleased &&
                pressed && mouseOver && mouseEvent.button == SDL_BUTTON_LEFT) {
                pressed = false;
                SQUIDL_LOG_INFO << "Button clicked!";
                if (toggleMode) {
                    selected = !selected;
                }
                // Вызываем callback
                if (onClick) {
                    onClick();
                }
                event.handled = true; // Останавливаем распространение
            } else if (mouseEvent.mouseEventType ==
                           Squidl::Core::MouseEventType::ButtonReleased &&
                       pressed) {
                // Если мышь отпустили за пределами кнопки
                pressed = false;
            }
        }
    }

    bool Button::update(Squidl::Core::UIContext &ctx,
                        Squidl::Core::IRenderer &renderer) {

        // Логика нажатия и ховера теперь находится в onEvent
        if (!enabled) {
            pressed = false;
        }

        updateBackdrop(ctx, renderer);

        if (label) {
            label->setRect(getRect());
            label->update(ctx, renderer);
        }

        return hovered || pressed;
    }

    void Button::updateBackdrop(Squidl::Core::UIContext &ctx,
                                Squidl::Core::IRenderer &renderer) {
        Squidl::Utils::UIRect currentRect = getRect();
        Squidl::Utils::Color currentBgColor;

        if (!enabled) {
            currentBgColor = disabledColor;
        } else if (selected) {
            currentBgColor = selectedColor;
        } else if (pressed) {
            currentBgColor = pressedColor;
        } else if (hovered) {
            currentBgColor = hoverColor;
        } else {
            currentBgColor = getBackgroundColor();
        }

        currentBgColor.a = static_cast<Uint8>(currentBgColor.a * getOpacity());
        renderer.drawFilledRect(currentRect, currentBgColor);

        if (!isBorderless() && getBorderOpacity() > 0.0f) {
            Squidl::Utils::Color borderCol = getBorderColor();
            borderCol.a = static_cast<Uint8>(borderCol.a * getBorderOpacity());
            renderer.drawOutlineRect(currentRect, borderCol);
        }
    }

    void Button::autosize() {
        if (!font)
            return;

        if (label) {
            label->autosize();
            if (rect.w == 0 && rect.h == 0) {
                setRect(Squidl::Utils::UIRect(rect.x, rect.y,
                                              label->getRect().w + 20,
                                              label->getRect().h + 10));
            } else {
            }
        }
        applyConstraints();
    }
} // namespace Squidl::Elements
