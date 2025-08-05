#pragma once
#include <SDL.h>
#include "Squidl/layouts/Layout.h"


namespace Squidl::Layouts{
    SQUIDL_API class VBoxLayout : public Layout {
      public:
        VBoxLayout(int spacing = 8);

        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;
        void setRect(const Squidl::Utils::UIRect &newRect) override;

        void autosize() override;

        void setPadding(int value) { padding = value; }
        Squidl::Core::Padding getPadding() const { return padding; }
        void setSpacing(int value) { spacing = value; }
        int getSpacing() const { return spacing; }

      private:
        


        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;
    };
}

