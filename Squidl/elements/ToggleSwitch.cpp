#include <Squidl/core/IRenderer.h>
#include <Squidl/core/UIContext.h>
#include <Squidl/core/UIEvent.h>
#include <Squidl/elements/ToggleSwitch.h>
#include <Squidl/utils/UIRect.h>
#include <Squidl/utils/Logger.h>

namespace Squidl::Elements {

    ToggleSwitch::ToggleSwitch(int x, int y, int w, int h, bool isOn)
        : mIsOn(isOn) {
        setRect({x, y, w, h});
        bgColor = Squidl::Utils::Color(22, 43, 56, 0);
        mOnColor = Squidl::Utils::Color(0, 150, 136, 255);
        mOffColor =
            Squidl::Utils::Color(158, 158, 158, 255);
        mKnobColor =
            Squidl::Utils::Color(200, 200, 200, 255);
        setBorderless(true); // No border by default
    }

    Utils::UIRect ToggleSwitch::calcSwitchRect() const {
        // Слот элемента (его rect)
        const auto &r = getRect();

        // Базовая высота — не больше высоты слота
        int h = r.h;

        // Поддерживаем приятный аспект свитча: ширина ≈ 1.8 * высота
        // и не больше ширины слота
        const float aspect = 1.8f;
        int w = std::min(r.w, static_cast<int>(h * aspect));
        // Если slot слишком узкий — подожмём высоту, чтобы влезло
        if (w > r.w) {
            w = r.w;
            h = static_cast<int>(w / aspect);
        }
        w = std::max(1, w);
        h = std::max(1, h);

        // Позиционирование по align внутри слота
        int x = r.x, y = r.y;

        switch (getHorizontalAlign()) {
        case Core::HorizontalAlign::Left:
            x = r.x;
            break;
        case Core::HorizontalAlign::Center:
            x = r.x + (r.w - w) / 2;
            break;
        case Core::HorizontalAlign::Right:
            x = r.x + r.w - w;
            break;
        case Core::HorizontalAlign::Stretch:
        case Core::HorizontalAlign::Justify:
            x = r.x;
            w = r.w; // если хочешь реально растягивать — оставь так
            break;
        }
        switch (getVerticalAlign()) {
        case Core::VerticalAlign::Top:
            y = r.y;
            break;
        case Core::VerticalAlign::Center:
            y = r.y + (r.h - h) / 2;
            break;
        case Core::VerticalAlign::Bottom:
            y = r.y + r.h - h;
            break;
        case Core::VerticalAlign::Stretch:
        case Core::VerticalAlign::Justify:
            y = r.y;
            h = r.h; // аналогично по высоте при Stretch/Justify
            break;
        }

        return {x, y, w, h};
    }

    bool ToggleSwitch::update(Squidl::Core::UIContext &ctx,
                              Squidl::Core::IRenderer &renderer) {
        // рисуем фон элемента (обычный bg/бордер, не тело свитча)
        updateBackdrop(ctx, renderer);

        // тело переключателя — внутри слота по align
        const auto body = calcSwitchRect();

        // фон тела (полоса)
        const auto trackColor = mIsOn ? mOnColor : mOffColor;
        renderer.fillRoundedRect(body, body.h / 2, trackColor);
        renderer.drawRoundedRect(body, body.h / 2, trackColor.lighter(0.7f));

        // ручка
        const int pad = 2; // небольшой внутренний отступ
        const int knobSize = body.h - pad * 2;
        int knobX = mIsOn ? (body.x + body.w - knobSize - pad) : (body.x + pad);
        Utils::UIRect knob{knobX, body.y + pad, knobSize, knobSize};

        renderer.fillRoundedRect(knob, knob.w / 2, mKnobColor);
        renderer.drawRoundedRect(knob, knob.w / 2,
                                 Utils::Color(100, 100, 100, 100));

        return true;
    }

    void ToggleSwitch::onEvent(Core::UIEvent &event) {
        if (event.type != Core::EventType::MouseEvent)
            return;

        auto &mouseEvent = static_cast<Core::MouseEvent &>(event);
        const auto body =
            calcSwitchRect(); // хит‑тест по телу, а не по всему rect

        const bool hoveredLocal =
            body.contains(mouseEvent.position.x, mouseEvent.position.y);

        if (mouseEvent.mouseEventType == Core::MouseEventType::ButtonPressed &&
            hoveredLocal && mouseEvent.button == SDL_BUTTON_LEFT) {
            pressed = true;
            event.handled = true;
        }
        if (mouseEvent.mouseEventType == Core::MouseEventType::ButtonReleased &&
            hoveredLocal && pressed && mouseEvent.button == SDL_BUTTON_LEFT) {
            pressed = false;
            setState(!mIsOn);
            event.handled = true;
        } else if (mouseEvent.mouseEventType ==
                       Core::MouseEventType::ButtonReleased &&
                   pressed) {
            // отпустили вне — сброс
            pressed = false;
        }
    }

    void ToggleSwitch::updateBackdrop(Core::UIContext &ctx,
                                      Core::IRenderer &renderer) {
        // стандартный фон элемента (обычно прозрачен)
        const auto r = getRect();
        auto bg = getBackgroundColor();
        if (bg.a > 0) {
            bg.a = static_cast<Uint8>(bg.a * getOpacity());
            renderer.drawFilledRect(r, bg);
        }
        if (!isBorderless() && getBorderOpacity() > 0.0f) {
            auto bc = getBorderColor();
            bc.a = static_cast<Uint8>(bc.a * getBorderOpacity());
            renderer.drawOutlineRect(r, bc);
        }
    }

    void ToggleSwitch::setState(bool isOn) {
        if (mIsOn != isOn) {
            mIsOn = isOn;
            // setBackgroundColor(mIsOn ? mOnColor : mOffColor);

            // Call the callback if it exists
            if (onStateChange) {
                onStateChange(mIsOn);
            }
        }
    }

    bool ToggleSwitch::getState() const { return mIsOn; }

    void ToggleSwitch::setKnobColor(Squidl::Utils::Color color) {
        // mKnobColor = color;
    }

    void ToggleSwitch::autosize() {
        // Если не задан размер, предложим дефолтный «приятный» размер
        if (rect.w == 0 && rect.h == 0) {
            const int h = 22; // базовая высота по умолчанию
            const int w = static_cast<int>(h * 1.8f);
            setRect({rect.x, rect.y, w, h});
        }
        applyConstraints();
    }

} // namespace Squidl::Elements
