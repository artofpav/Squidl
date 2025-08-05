#pragma once
#include "Squidl/base/UIElement.h"
#include "Squidl/core/IRenderer.h" // <--- Add IRenderer.h
#include "Squidl/core/UIContext.h"
#include "Squidl/utils/Color.h"  // <--- Use Squidl::Utils::Color
#include "Squidl/utils/UIRect.h" // <--- Use Squidl::Utils::UIRect
#include <SDL.h>                 // For SDL_Texture (still used for now)
#include <string>                // For setTextureFromFile

namespace Squidl::Elements {
    class SQUIDL_API Backdrop : public Squidl::Base::UIElement {
      public:
        Backdrop(SDL_Texture *tex = nullptr);
        ~Backdrop(); // Destructor to manage texture ownership

        void setTexture(SDL_Texture *tex, bool owns = false);
        void setTextureFromFile(
            SDL_Renderer *renderer,
            const std::string &path); // Still needs SDL_Renderer* for loading

        // Update signature changed to IRenderer&
        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;
        void autosize() override;

      private:
        SDL_Texture *texture = nullptr;
        bool ownsTexture =
            false; // Flag to indicate if this Backdrop owns the texture

        // UpdateBackdrop signature changed to IRenderer&
        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;
    };
} // namespace Squidl::Elements
