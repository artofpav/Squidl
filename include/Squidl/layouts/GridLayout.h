#pragma once
#include "Squidl/SquidlConfig.h"
#include "Squidl/utils/UIRect.h"
#include "Squidl/layouts/Layout.h"

namespace Squidl::Layouts {
    SQUIDL_API class GridLayout : public Layout {
      public:
        GridLayout(int columns = 2, int spacing = 5, int padding = 10);

        void setRect(const Squidl::Utils::UIRect &newRect) override;
        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;
        void autosize() override;

        void setPadding(int value) { padding = value; }
        int getPadding() const { return padding; }
        void setSpacing(int value) { spacing = value; }
        int getSpacing() const { return spacing; }
        void setColumns(int value) { columns = value; }

      private:
        int columns;
        int spacing;
        int padding;

        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;
    };
} // namespace Squidl::Layouts
