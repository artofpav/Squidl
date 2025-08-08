#pragma once
#include "Squidl/base/UIElement.h"
#include "Squidl/core/UIAlignment.h"
#include "Squidl/core/IRenderer.h" // Include IRenderer.h
#include "Squidl/core/UIContext.h"
#include "Squidl/utils/Color.h"  // Use Squidl::Utils::Color
#include "Squidl/utils/UIRect.h" // Use Squidl::Utils::UIRect
#include <SDL_ttf.h>
#include <memory>
#include <vector>

namespace Squidl::Layouts {
    SQUIDL_API class Layout : public Squidl::Base::UIElement {
      public:
      
        void setChildAlignmentOverride(
            std::optional<Squidl::Core::HorizontalAlign> hx,
            std::optional<Squidl::Core::VerticalAlign> vy) {
            childHXOverride = hx;
            childVYOverride = vy;
        }

        virtual ~Layout() = default;

        // Add element to internal list
        virtual void add(std::shared_ptr<Squidl::Base::UIElement> child);

        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;

        void setFont(TTF_Font *f) override;

        /**
         * @brief Возвращает список дочерних элементов.
         * @return Константная ссылка на вектор дочерних shared_ptr.
         */
        const std::vector<std::shared_ptr<Squidl::Base::UIElement>> &
        getChildren() const {
            return children;
        }
        void setSpacing(int value) { spacing = value; }
        int getSpacing() const { return spacing; }

      protected:
        std::vector<std::shared_ptr<Squidl::Base::UIElement>> children;
        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;

        int spacing = 1;

        static Squidl::Utils::UIRect
        alignInSlot(const Squidl::Utils::UIRect &slot,
                    const Squidl::Utils::UIRect
                        &desired, // размер ребёнка до выравнивания
                    Squidl::Core::HorizontalAlign hx,
                    Squidl::Core::VerticalAlign vy) {
            using namespace Squidl::Utils;
            UIRect r = desired;

            // Горизонталь
            switch (hx) {
            case Squidl::Core::HorizontalAlign::Left:
                r.x = slot.x;
                r.w = desired.w;
                break;
            case Squidl::Core::HorizontalAlign::Center:
                r.x = slot.x + (slot.w - desired.w) / 2;
                r.w = desired.w;
                break;
            case Squidl::Core::HorizontalAlign::Right:
                r.x = slot.x + slot.w - desired.w;
                r.w = desired.w;
                break;
            case Squidl::Core::HorizontalAlign::Stretch:
            case Squidl::Core::HorizontalAlign::Justify:
                r.x = slot.x;
                r.w = slot.w;
                break;
            }

            // Вертикаль
            switch (vy) {
            case Squidl::Core::VerticalAlign::Top:
                r.y = slot.y;
                r.h = desired.h;
                break;
            case Squidl::Core::VerticalAlign::Center:
                r.y = slot.y + (slot.h - desired.h) / 2;
                r.h = desired.h;
                break;
            case Squidl::Core::VerticalAlign::Bottom:
                r.y = slot.y + slot.h - desired.h;
                r.h = desired.h;
                break;
            case Squidl::Core::VerticalAlign::Stretch:
            case Squidl::Core::VerticalAlign::Justify:
                r.y = slot.y;
                r.h = slot.h;
                break;
            }
            return r;
        }

        // Опциональные оверрайды на уровне лэйаута (по умолчанию — не заданы)
        std::optional<Squidl::Core::HorizontalAlign> childHXOverride;
        std::optional<Squidl::Core::VerticalAlign> childVYOverride;

    };
} // namespace Squidl::Layouts
