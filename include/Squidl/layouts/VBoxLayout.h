#pragma once
#include "Squidl/layouts/Layout.h"

namespace Squidl::Layouts {

    class SQUIDL_API VBoxLayout : public Layout {
      public:
        explicit VBoxLayout(int spacing = 5);

        void setRect(const Squidl::Utils::UIRect &newRect) override;
        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;
        void autosize() override;

        void setPadding(int value) { padding = value; }
        Squidl::Core::Padding getPadding() const { return padding; }

        void setSpacing(int value) { spacing = value; }
        int getSpacing() const { return spacing; }

      private:
        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;
    };

} // namespace Squidl::Layouts
