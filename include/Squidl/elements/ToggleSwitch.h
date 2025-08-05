// include/Squidl/elements/ToggleSwitch.h
#pragma once
#include <Squidl/base/UIElement.h>
#include <Squidl/utils/Color.h>
#include <functional>

namespace Squidl::Elements {

    class ToggleSwitch : public Squidl::Base::UIElement {
      public:
        ToggleSwitch(int x, int y, int w, int h,
                     bool isOn = false); // Конструктор

        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;
        void onEvent(Squidl::Core::UIEvent &event) override;
        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;
        void autosize() override;

        void setState(bool isOn);
        bool getState() const;

        void setOnColor(Squidl::Utils::Color color);
        void setOffColor(Squidl::Utils::Color color);
        void setKnobColor(Squidl::Utils::Color color);

        std::function<void(bool)> onStateChange;

      private:
        bool mIsOn;

        Squidl::Utils::Color mOnColor = Squidl::Utils::Color(0, 150, 136, 255);
        Squidl::Utils::Color mOffColor =
            Squidl::Utils::Color(158, 158, 158, 255);
        Squidl::Utils::Color mKnobColor =
            Squidl::Utils::Color(255, 255, 255, 255);
        bool hovered = false;
        bool pressed = false;
        bool enabled = true;
    };

} // namespace Squidl::Elements