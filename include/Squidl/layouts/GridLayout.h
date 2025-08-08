#pragma once
#include "Squidl/layouts/Layout.h"

namespace Squidl::Layouts {

    class SQUIDL_API GridLayout : public Layout {
      public:
        explicit GridLayout(int columns = 2, int spacing = 5, int padding = 5);

        void setColumns(int value) { columns = (value < 1 ? 1 : value); }
        int getColumns() const { return columns; }

        void setRect(const Squidl::Utils::UIRect &newRect) override;
        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;
        void autosize() override;

        void setPadding(int value) { padding = value; }
        Squidl::Core::Padding getPadding() const { return padding; }

        void setSpacing(int value) { spacing = value; }
        int getSpacing() const { return spacing; }

      private:
        int columns;
        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;
    };

} // namespace Squidl::Layouts
