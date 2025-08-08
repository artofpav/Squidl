#include "Squidl/elements/Checkbox.h"
#include <SDL.h>
#include <Squidl/core/UIEvent.h>
#include <Squidl/utils/Logger.h>

namespace Squidl::Elements {

    Checkbox::Checkbox(const std::string &labelText, int x, int y, int size,
                       bool isChecked, TTF_Font *font)
        : mIsChecked(isChecked) {
        // Высота чекбокса будет равна высоте текста
        int textWidth, textHeight;
        TTF_SizeUTF8(font, labelText.c_str(), &textWidth, &textHeight);
        int boxSize = textHeight;

        // Общий прямоугольник для чекбокса и текста
        setRect({x, y, size, size});
        // mLabel = std::make_shared<Label>(labelText, x + boxSize + 5, y,
        //                                  textWidth, boxSize, font);
        // mLabel->setParent(shared_from_this());
    }

    void Checkbox::autosize() {
        // Autosize logic can be implemented here if needed.
        // In this case, size is determined in the constructor.
    }

    // Checkbox.cpp :: update
    bool Checkbox::update(Core::UIContext &ctx, Core::IRenderer &renderer) {
        updateBackdrop(ctx, renderer);

        auto box = calcBoxRect();
        renderer.drawFilledRect(box, mCheckboxColor);
        renderer.drawOutlineRect(box, mCheckmarkColor);

        if (mIsChecked) {
            int x1 = box.x + box.w / 4, y1 = box.y + box.h / 2;
            int x2 = box.x + box.w / 2, y2 = box.y + box.h - box.h / 4;
            int x3 = box.x + box.w - box.w / 4, y3 = box.y + box.h / 4;
            renderer.drawLine(x1, y1, x2, y2, mCheckmarkColor);
            renderer.drawLine(x2, y2, x3, y3, mCheckmarkColor);
        }
        return false;
    }

    void Checkbox::onEvent(Core::UIEvent &event) {
        if (event.type != Core::EventType::MouseEvent)
            return;

        auto &me = static_cast<Core::MouseEvent &>(event);
        if (me.mouseEventType == Core::MouseEventType::ButtonPressed &&
            me.button == SDL_BUTTON_LEFT) {

            const auto box = calcBoxRect();
            const bool inBox = box.contains(me.position.x, me.position.y);
            const bool inLabel = false;
            //  (mLabel && mLabel->getRect().contains(
            //                                     me.position.x, me.position.y));

            if (inBox || inLabel) {
                mIsChecked = !mIsChecked;
                if (mToggleCallback)
                    mToggleCallback(mIsChecked);
                event.handled = true;
            }
        }
    }

    // Checkbox.cpp :: updateBackdrop
    void Checkbox::updateBackdrop(Squidl::Core::UIContext &,
                                  Squidl::Core::IRenderer &renderer) {
        // auto currentRect = getRect();

        // // Фон — обычный bgColor элемента (по умолчанию лучше сделать
        // // прозрачным)
        // auto bg = getBackgroundColor();
        // if (bg.a > 0) {
        //     bg.a = static_cast<Uint8>(bg.a * getOpacity());
        //     renderer.drawFilledRect(currentRect, bg);
        // }

        // if (!isBorderless() && getBorderOpacity() > 0.0f) {
        //     auto bc = getBorderColor();
        //     bc.a = static_cast<Uint8>(bc.a * getBorderOpacity());
        //     renderer.drawOutlineRect(currentRect, bc);
        // }
    }

    Utils::UIRect Checkbox::calcBoxRect() const {

        auto r = getRect();
        int boxW = std::min(r.w, r.h);
        int boxH = boxW;
        int boxX = r.x;
        int boxY = r.y;

        // Горизонтальное выравнивание
        switch (getHorizontalAlign()) {
        case Core::HorizontalAlign::Left:
            boxX = r.x;
            break;
        case Core::HorizontalAlign::Center:
            boxX = r.x + (r.w - boxW) / 2;
            break;
        case Core::HorizontalAlign::Right:
            boxX = r.x + r.w - boxW;
            break;
        case Core::HorizontalAlign::Stretch:
        case Core::HorizontalAlign::Justify:
            boxX = r.x;
            boxW = r.w;
            break;
        }
        // Вертикальное выравнивание
        switch (getVerticalAlign()) {
        case Core::VerticalAlign::Top:
            boxY = r.y;
            break;
        case Core::VerticalAlign::Center:
            boxY = r.y + (r.h - boxH) / 2;
            break;
        case Core::VerticalAlign::Bottom:
            boxY = r.y + r.h - boxH;
            break;
        case Core::VerticalAlign::Stretch:
        case Core::VerticalAlign::Justify:
            boxY = r.y;
            boxH = r.h;
            break;
        }
        return {boxX, boxY, boxW, boxH};
    }

} // namespace Squidl::Elements