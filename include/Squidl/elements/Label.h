#pragma once
#include <SDL_ttf.h> // For TTF_Font
#include <string>    // For std::string

#include "Squidl/SquidlConfig.h"   // For SQUIDL_API and namespace declarations
#include "Squidl/base/UIElement.h" // For Squidl::Base::UIElement
#include "Squidl/core/IRenderer.h" // Include IRenderer.h
#include "Squidl/core/UIAlignment.h" // For Squidl::Core::HorizontalAlign, Squidl::Core::VerticalAlign
#include "Squidl/utils/Color.h"  // <--- Use Squidl::Utils::Color
#include "Squidl/utils/UIRect.h" // <--- Use Squidl::Utils::UIRect

namespace Squidl::Core {
    class UIContext;
}

namespace Squidl::Elements {

    class SQUIDL_API Label
        : public Squidl::Base::UIElement { // Qualify UIElement
      public:
        Label(std::string text, int x, int y, int w, int h,
              TTF_Font *font = nullptr);
        void setText(std::string t);
        std::string getText() const;
        void autosize() override;
        // UIContext now qualified by Squidl::Core namespace
        // Renderer now accepts IRenderer&
        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;

        // Setters for alignment
        // HorizontalAlign and VerticalAlign now qualified by Squidl::Core
        // namespace
        void setHorizontalAlignment(Squidl::Core::HorizontalAlign align) {
            m_horizontalAlign = align;
        }
        void setVerticalAlignment(Squidl::Core::VerticalAlign align) {
            m_verticalAlign = align;
        }

        // New padding methods
        void setPadding(int p);
        void setPadding(int horizontal, int vertical);
        void setPadding(int left, int top, int right, int bottom);
        int getPaddingLeft() const { return paddingLeft; }
        int getPaddingRight() const { return paddingRight; }
        int getPaddingTop() const { return paddingTop; }
        int getPaddingBottom() const { return paddingBottom; }

        // New methods for TextColor
        void setTextColor(Squidl::Utils::Color color) { fgColor = color; }
        Squidl::Utils::Color getTextColor() const { return fgColor; }

        // Новый метод для установки смещения текста для прокрутки/клиппирования
        void setTextOffset(int offset) { m_textOffsetX = offset; }
        int getTextOffset() const { return m_textOffsetX; }

      protected:
        // UpdateBackdrop signature changed to IRenderer&
        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;
        std::string text;
        Squidl::Utils::Color fgColor = {
            0, 0, 0, 255}; // <--- Changed to Squidl::Utils::Color

        Squidl::Core::HorizontalAlign m_horizontalAlign =
            Squidl::Core::HorizontalAlign::Left; // Default to Left
        Squidl::Core::VerticalAlign m_verticalAlign =
            Squidl::Core::VerticalAlign::Center; // Default to Center

        // Padding members
        int paddingLeft = 5;
        int paddingRight = 5;
        int paddingTop = 5;
        int paddingBottom = 5;

        int m_textOffsetX = 0; // Внутреннее смещение для отрисовки текста
    };

} // namespace Squidl::Elements
