#include <Squidl/core/IRenderer.h>
#include <Squidl/core/UIContext.h>
#include <Squidl/core/UIEvent.h>
#include <Squidl/elements/ToggleSwitch.h>
#include <Squidl/utils/UIRect.h>

namespace Squidl::Elements {

    ToggleSwitch::ToggleSwitch(int x, int y, int w, int h, bool isOn)
        : mIsOn(isOn){
        setRect({x, y, w, h});
        bgColor = Squidl::Utils::Color(22, 43, 56, 120);
        setBorderless(true); // No border by default
    }

    bool ToggleSwitch::update(Squidl::Core::UIContext &ctx,
                              Squidl::Core::IRenderer &renderer) {
        updateBackdrop(ctx, renderer);

        // Calculate knob position and size
        Squidl::Utils::UIRect rect = getRect();
        int knobSize =
            rect.h - 4; // Knob is slightly smaller than the switch height
        int knobX;

        if (mIsOn) {
            knobX = rect.x + rect.w - knobSize - 3; // Right-aligned when on
        } else {
            knobX = rect.x + 3; // Left-aligned when off
        }

        // Draw the knob
        Squidl::Utils::UIRect knobRect = {knobX, rect.y + 2, knobSize,
                                          knobSize};
        renderer.fillRoundedRect(knobRect, knobRect.w/2, mKnobColor);
        renderer.drawRoundedRect(knobRect, knobRect.w / 2,
                                 mKnobColor.darker(0.5f));

        return true;
    }

    void ToggleSwitch::onEvent(Squidl::Core::UIEvent &event) {
        Squidl::Utils::UIRect rect = getRect();

        if (event.type == Squidl::Core::EventType::MouseEvent) {

            auto &mouseEvent = static_cast<Squidl::Core::MouseEvent &>(event);
            bool hovered =
                rect.contains(mouseEvent.position.x, mouseEvent.position.y);

            if (mouseEvent.mouseEventType ==
                    Squidl::Core::MouseEventType::ButtonPressed &&
                hovered && mouseEvent.button == SDL_BUTTON_LEFT) {
                pressed = true;
            }
            if (mouseEvent.mouseEventType ==
                    Squidl::Core::MouseEventType::ButtonReleased &&
                hovered && pressed && mouseEvent.button == SDL_BUTTON_LEFT) {
                pressed = false;
                setState(!mIsOn);
                event.handled = true;
            }
        }
    }

    void ToggleSwitch::updateBackdrop(Squidl::Core::UIContext &ctx,
                                      Squidl::Core::IRenderer &renderer) {
        Squidl::Utils::UIRect rect = getRect();

        // Draw the switch background
        //Squidl::Utils::Color bgColor = mIsOn ? mOnColor : mOffColor;

        // Draw with rounded corners for a switch-like appearance
        renderer.fillRoundedRect(rect, rect.h / 2, bgColor);
        //renderer.drawRoundedRect(rect, rect.h / 2, bgColor.darker(0.3f));
    }

    void ToggleSwitch::setState(bool isOn) {
        if (mIsOn != isOn) {
            mIsOn = isOn;
            //setBackgroundColor(mIsOn ? mOnColor : mOffColor);

            // Call the callback if it exists
            if (onStateChange) {
                onStateChange(mIsOn);
            }
        }
    }

    bool ToggleSwitch::getState() const { return mIsOn; }

    void ToggleSwitch::setKnobColor(Squidl::Utils::Color color) {
        mKnobColor = color;
    }

    void ToggleSwitch::autosize() {
        // Для ToggleSwitch автоматическое изменение размера не требуется,
        // так как он имеет фиксированный размер.
        // Оставляем тело метода пустым.
    }

} // namespace Squidl::Elements
