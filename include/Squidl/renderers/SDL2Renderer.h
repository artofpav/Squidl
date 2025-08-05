// squidl/renderers/SDL2Renderer.h
#pragma once

#include "Squidl/core/IRenderer.h" // Inherit from IRenderer
#include <SDL.h>                   // For SDL_Renderer
#include <SDL_ttf.h> // For TTF_Font (as it's used in drawText signature)

// Forward declarations
namespace Squidl::Base {
    class UIElement;
}
namespace Squidl::Core {
    class UIContext;
}

namespace Squidl::Renderers { // New namespace for specific renderer
                              // implementations

    /**
     * @brief Concrete SDL2 implementation of the UI renderer.
     * @ingroup Renderers
     * Implements the IRenderer interface using SDL2 functions for rendering.
     */
    class SQUIDL_API SDL2Renderer : public Squidl::Core::IRenderer {
      public:
        /**
         * @brief Constructor.
         * @param renderer Pointer to the SDL_Renderer. SDL2Renderer does not
         * own this pointer and does not free it upon destruction.
         */
        SDL2Renderer(SDL_Renderer *renderer);

        // Implementation of all pure virtual methods from IRenderer
        void render(std::shared_ptr<Squidl::Base::UIElement> rootElement,
                    Squidl::Core::UIContext &ctx) override;
        void setDrawColor(Squidl::Utils::Color color)
            override;
        void clearScreen(Squidl::Utils::Color color)
            override;

        void drawFilledRect(const Squidl::Utils::UIRect &rect,
                            Squidl::Utils::Color color)
            override;
        void fillRoundedRect(const Squidl::Utils::UIRect &rect, int radius,
                             Squidl::Utils::Color color) override;

        void drawRoundedRect(const Squidl::Utils::UIRect &rect, int radius,
                             Squidl::Utils::Color color) override;

        void drawOutlineRect(const Squidl::Utils::UIRect &rect,
                             Squidl::Utils::Color color)
            override;
        void drawTexture(SDL_Texture *texture, const SDL_Rect *srcRect,
                         const SDL_Rect *destRect,
                         float opacity = 1.0f) override;
        void drawText(TTF_Font *font, const std::string &text,
                      Squidl::Utils::Color color,
                      const Squidl::Utils::UIRect &destRect)
            override;
        void setClipRect(const Squidl::Utils::UIRect &rect) override;
        void resetClipRect() override;

      private:
        SDL_Renderer *m_sdlRenderer; // Pointer to the underlying SDL_Renderer
    };

} // namespace Squidl::Renderers
