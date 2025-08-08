// squidl/renderers/SDL2Renderer.cpp
#include "Squidl/renderers/SDL2Renderer.h"
#include "Squidl/base/UIElement.h" // For UIElement::update
#include "Squidl/core/UIContext.h" // For UIContext
#include "Squidl/utils/Logger.h"   // For logging (if needed)
#include <SDL2_gfxPrimitives.h>
#include <SDL_image.h> // For SDL_image functions (if loading textures directly in renderer)

namespace Squidl::Renderers {

    SDL2Renderer::SDL2Renderer(SDL_Renderer *renderer)
        : m_sdlRenderer(renderer) {
        if (!m_sdlRenderer) {
            SQUIDL_LOG_ERROR
                << "SDL2Renderer: Initialized with a null SDL_Renderer.";
        }
    }

    void
    SDL2Renderer::render(std::shared_ptr<Squidl::Base::UIElement> rootElement,
                         Squidl::Core::UIContext &ctx) {
        if (!m_sdlRenderer) {
            SQUIDL_LOG_ERROR
                << "SDL2Renderer: Cannot render: SDL_Renderer is null.";
            return;
        }
        if (!rootElement) {
            SQUIDL_LOG_WARNING
                << "SDL2Renderer: Attempting to render a null root UIElement.";
            return;
        }

        // Call update on the root element, passing the IRenderer reference
        // This is the key change for polymorphism
        rootElement->update(ctx, *this); // Pass 'this' as IRenderer&
    }

    void SDL2Renderer::setDrawColor(Squidl::Utils::Color color) {
        if (m_sdlRenderer) {
            SDL_SetRenderDrawColor(m_sdlRenderer, color.r, color.g, color.b,
                                   color.a);
        }
    }

    void SDL2Renderer::clearScreen(Squidl::Utils::Color color) {
        if (m_sdlRenderer) {
            SDL_SetRenderDrawColor(m_sdlRenderer, color.r, color.g, color.b,
                                   color.a);
            SDL_RenderClear(m_sdlRenderer);
        }
    }

    void SDL2Renderer::drawLine(int x1, int y1, int x2, int y2,
                                Squidl::Utils::Color color) {
        SDL_SetRenderDrawColor(m_sdlRenderer, color.r, color.g, color.b,
                               color.a);
        
        SDL_RenderDrawLine(m_sdlRenderer, x1, y1, x2, y2);
        // SDL_RenderDrawLineF();
    }

    void SDL2Renderer::drawFilledRect(const Squidl::Utils::UIRect &rect,
                                      Squidl::Utils::Color color) {
        if (m_sdlRenderer) {
            SDL_SetRenderDrawColor(m_sdlRenderer, color.r, color.g, color.b,
                                   color.a);
            SDL_Rect sdlRect = rect;
            SDL_RenderFillRect(m_sdlRenderer, &sdlRect);
        }
    }

    void SDL2Renderer::fillRoundedRect(const Squidl::Utils::UIRect &rect,
                                       int radius, Squidl::Utils::Color color) {
        if (m_sdlRenderer) {
            roundedBoxRGBA(m_sdlRenderer, rect.x, rect.y, rect.x + rect.w - 1,
                           rect.y + rect.h - 1, radius, color.r, color.g,
                           color.b, color.a);
        }
    }

    void SDL2Renderer::drawRoundedRect(const Squidl::Utils::UIRect &rect,
                                       int radius, Squidl::Utils::Color color) {
        SDL_SetRenderDrawColor(m_sdlRenderer, color.r, color.g, color.b,
                               color.a);
        if (m_sdlRenderer) {
            roundedRectangleRGBA(m_sdlRenderer, rect.x, rect.y,
                                 rect.x + rect.w - 1, rect.y + rect.h - 1,
                                 radius, color.r, color.g, color.b, color.a);
        }
    }

    void SDL2Renderer::drawOutlineRect(const Squidl::Utils::UIRect &rect,
                                       Squidl::Utils::Color color) {
        if (m_sdlRenderer) {
            SDL_SetRenderDrawColor(m_sdlRenderer, color.r, color.g, color.b,
                                   color.a);
            SDL_Rect sdlRect = rect;
            SDL_RenderDrawRect(m_sdlRenderer, &sdlRect);
        }
    }

    void SDL2Renderer::drawTexture(SDL_Texture *texture,
                                   const SDL_Rect *srcRect,
                                   const SDL_Rect *destRect, float opacity) {
        if (m_sdlRenderer && texture) {
            Uint8 current_alpha;
            SDL_GetTextureAlphaMod(texture,
                                   &current_alpha); // Save current alpha
            SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(opacity * 255));

            SDL_RenderCopy(m_sdlRenderer, texture, srcRect, destRect);

            SDL_SetTextureAlphaMod(texture,
                                   current_alpha); // Restore original alpha
        }
    }

    void SDL2Renderer::drawText(TTF_Font *font, const std::string &text,
                                Squidl::Utils::Color color,
                                const Squidl::Utils::UIRect &destRect) {
        if (!m_sdlRenderer || !font || text.empty())
            return;

        // Create surface from text
        SDL_Surface *textSurface = TTF_RenderUTF8_Blended(
            font, text.c_str(), static_cast<SDL_Color>(color));
        if (!textSurface) {
            SQUIDL_LOG_ERROR << "SDL2Renderer: Failed to create text surface: "
                             << TTF_GetError();
            return;
        }

        // Create texture from surface
        SDL_Texture *textTexture =
            SDL_CreateTextureFromSurface(m_sdlRenderer, textSurface);
        if (!textTexture) {
            SQUIDL_LOG_ERROR << "SDL2Renderer: Failed to create text texture: "
                             << SDL_GetError();
            SDL_FreeSurface(textSurface);
            return;
        }

        // Render the texture
        SDL_Rect sdlDestRect = destRect;
        SDL_RenderCopy(m_sdlRenderer, textTexture, nullptr, &sdlDestRect);

        // Clean up
        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(textSurface);
    }

    void SDL2Renderer::setClipRect(const Squidl::Utils::UIRect &rect) {
        if (m_sdlRenderer) {
            SDL_Rect sdlClipRect = rect;
            SDL_RenderSetClipRect(m_sdlRenderer, &sdlClipRect);
        }
    }

    void SDL2Renderer::resetClipRect() {
        if (m_sdlRenderer) {
            SDL_RenderSetClipRect(m_sdlRenderer, nullptr);
        }
    }

} // namespace Squidl::Renderers
