// include/Squidl/core/IRenderer.h
#pragma once

#include "Squidl/SquidlConfig.h" // For SQUIDL_API
#include <SDL_ttf.h>
#include <memory> // For std::shared_ptr

#include "Squidl/utils/Color.h"  // <--- Use Squidl::Utils::Color
#include "Squidl/utils/UIRect.h" // <--- Use Squidl::Utils::UIRect

// Forward declarations to avoid circular dependencies
namespace Squidl::Base {
    class UIElement;
}
namespace Squidl::Core {
    class UIContext;
}

struct SDL_Texture; // SDL.h defines SDL_Texture

namespace Squidl::Core {

    /**
     * @brief Abstract interface for a UI rendering system.
     * @ingroup Core
     * Defines common rendering operations, independent of the specific graphics
     * API.
     */
    class SQUIDL_API IRenderer {
      public:
        virtual ~IRenderer() = default;

        /**
         * @brief Renders the root UI element and all its children.
         *
         * @param rootElement Pointer to the root UI element to be rendered.
         * @param ctx The UI context.
         */
        virtual void
        render(std::shared_ptr<Squidl::Base::UIElement> rootElement,
               UIContext &ctx) = 0;

        /**
         * @brief Sets the draw color for subsequent drawing operations.
         *
         * @param color The color to set.
         */
        virtual void setDrawColor(Squidl::Utils::Color color) = 0;

        /**
         * @brief Clears the current rendering target (usually the screen) with
         * the specified color.
         *
         * @param color The color to clear with.
         */
        virtual void clearScreen(Squidl::Utils::Color color) = 0;

        /**
        * @brief Draw line 
         */
        virtual void drawLine(int x1, int y1, int x2, int y2, Squidl::Utils::Color color) = 0;

        /**
         * @brief Draws a filled rectangle.
         *
         * @param rect The rectangle to draw.
         * @param color The color of the rectangle.
         */
        virtual void drawFilledRect(const Squidl::Utils::UIRect &rect,
                                    Squidl::Utils::Color color) = 0;

        /**
         * @brief Draws a filled rectangle with rounded corners.
         *
         * @param rect The rectangle to draw.
         * @param radius The radius of the corners.
         * @param color The color of the rectangle.
         */
        virtual void fillRoundedRect(const Squidl::Utils::UIRect &rect,
                                     int radius,
                                     Squidl::Utils::Color color) = 0;

        virtual void drawRoundedRect(const Squidl::Utils::UIRect &rect,
                                     int radius,
                                     Squidl::Utils::Color color) = 0;
        /**
         * @brief Draws an outlined rectangle.
         *
         * @param rect The rectangle to draw.
         * @param color The color of the outline.
         */
        virtual void drawOutlineRect(const Squidl::Utils::UIRect &rect,
                                     Squidl::Utils::Color color) = 0;

        /**
         * @brief Draws a texture to the rendering target.
         *
         * @param texture The SDL_Texture to draw.
         * @param srcRect Source rectangle for the texture (nullptr for full
         * texture).
         * @param destRect Destination rectangle on the target.
         * @param opacity Opacity from 0.0f (transparent) to 1.0f (opaque).
         */
        virtual void drawTexture(SDL_Texture *texture, const SDL_Rect *srcRect,
                                 const SDL_Rect *destRect,
                                 float opacity = 1.0f) = 0;
        // NOTE: SDL_Texture and SDL_Rect* are kept here for now, full
        // abstraction requires more complex resource management.

        /**
         * @brief Renders text using a specified font.
         *
         * @param font The TTF_Font to use.
         * @param text The text string to render.
         * @param color The color of the text.
         * @param destRect The destination rectangle for the text (used for
         * positioning).
         */
        virtual void drawText(TTF_Font *font, const std::string &text,
                              Squidl::Utils::Color color,
                              const Squidl::Utils::UIRect &destRect) = 0;

        /**
         * @brief Sets the clipping rectangle for all subsequent drawing
         * operations. Drawing will only occur within this rectangle.
         *
         * @param rect The UIRect to set as the clipping area.
         */
        virtual void setClipRect(const Squidl::Utils::UIRect &rect) = 0;

        /**
         * @brief Resets the clipping rectangle, allowing drawing to the entire
         * target.
         */
        virtual void resetClipRect() = 0;

        // Potentially more drawing primitives
        // virtual void drawLine(Squidl::Utils::Point p1, Squidl::Utils::Point
        // p2, Squidl::Utils::Color color) = 0; virtual void
        // drawCircle(Squidl::Utils::Point center, int radius,
        // Squidl::Utils::Color color, bool filled) = 0;
    };

} // namespace Squidl::Core
