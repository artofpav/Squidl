#pragma once
#include "Squidl/base/UIElement.h"
#include "Squidl/core/IRenderer.h"
#include "Squidl/core/UIContext.h"
#include "Squidl/elements/Label.h"
#include "Squidl/utils/Color.h"
#include "Squidl/utils/UIRect.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <functional> // Для std::function

namespace Squidl::Elements {
    class SQUIDL_API Button : public Squidl::Base::UIElement {
      public:
        Button(std::string labelText, int x, int y, int w, int h,
               TTF_Font *font = nullptr, bool isBorderless = false);

        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;

        // State colors
        void setHoveredColor(Squidl::Utils::Color color) { hoverColor = color; }
        void setPressedColor(Squidl::Utils::Color color) {
            pressedColor = color;
        }
        void setDisabledColor(Squidl::Utils::Color color) {
            disabledColor = color;
        }
        void setSelectedColor(Squidl::Utils::Color color) {
            selectedColor = color;
        }

        void setEnabled(bool value) { enabled = value; }
        void setSelected(bool value) { selected = value; }

        void setToggleMode(bool value) { toggleMode = value; }
        bool isSelected() const { return selected; }

        void autosize() override;

        void setLabel(std::shared_ptr<Label> value) { label = value; }
        std::shared_ptr<Label> getLabel() const { return label; }
        void setLabelText(const std::string &text);
        std::string getLabelText() const {
            return label ? label->getText() : "";
        }

        // Переопределение метода onEvent для обработки событий
        void onEvent(Squidl::Core::UIEvent &event) override;

        // Callback для обработки клика
        std::function<void()> onClick;

      private:
        std::shared_ptr<Squidl::Elements::Label> label;

        bool hovered = false;
        bool pressed = false;
        bool enabled = true;
        bool selected = false;
        bool toggleMode = false;

        Squidl::Utils::Color hoverColor = {130, 130, 130, 255};
        Squidl::Utils::Color pressedColor = {160, 100, 100, 255};
        Squidl::Utils::Color disabledColor = {80, 80, 80, 255};
        Squidl::Utils::Color selectedColor = {200, 180, 80, 255};

        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;
    };
} // namespace Squidl::Elements
