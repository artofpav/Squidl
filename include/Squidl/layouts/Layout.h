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

      protected:
        std::vector<std::shared_ptr<Squidl::Base::UIElement>> children;
        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;

        int spacing = 1;
    };
} // namespace Squidl::Layouts
