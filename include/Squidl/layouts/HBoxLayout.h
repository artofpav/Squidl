#pragma once
#include "Squidl/SquidlConfig.h"
#include "Squidl/layouts/Layout.h"
#include "Squidl/core/UIContext.h"
#include "Squidl/core/IRenderer.h"  
#include "Squidl/utils/UIRect.h"
#include <SDL.h>

namespace Squidl::Layouts {
    SQUIDL_API class HBoxLayout : public Layout {
      public:
        HBoxLayout(int spacing = 8);

        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;
        void setRect(const Squidl::Utils::UIRect &newRect) override;

        void autosize() override;

      private:

        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;
    };
} // namespace Squidl::Layouts
