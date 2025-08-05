// squidl/elements/Backdrop.cpp
#include "Squidl/elements/Backdrop.h"
#include "Squidl/utils/Logger.h" // For logging
#include <SDL_image.h>           // For IMG_LoadTexture

namespace Squidl::Elements {

    Backdrop::Backdrop(SDL_Texture *tex) : texture(tex), ownsTexture(false) {
        // Default background color for backdrop
        setBackgroundColor({100, 100, 100, 50});
        setBorderColor({0, 0, 0, 255});
    }

    Backdrop::~Backdrop() {
        if (ownsTexture && texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

    void Backdrop::setTexture(SDL_Texture *tex, bool owns) {
        if (ownsTexture && texture) {
            SDL_DestroyTexture(texture);
        }
        texture = tex;
        ownsTexture = owns;
    }

    void Backdrop::setTextureFromFile(SDL_Renderer *renderer,
                                      const std::string &path) {
        if (ownsTexture && texture) {
            SDL_DestroyTexture(texture);
        }
        texture = IMG_LoadTexture(renderer, path.c_str());
        if (!texture) {
            SQUIDL_LOG_ERROR << "Backdrop: Failed to load texture from file '"
                             << path << "': " << IMG_GetError();
            ownsTexture = false;
        } else {
            ownsTexture = true;
        }
    }

    bool Backdrop::update(Squidl::Core::UIContext &ctx,
                          Squidl::Core::IRenderer &renderer) {
        // Update anchored position if managed by layout
        if (isManagedByLayout()) {
            updateAnchoredRect(getParent()->getRect());
        }

        updateBackdrop(ctx, renderer); // Draw the backdrop itself

        // Children are not handled by Backdrop's update, but by Layout if it's
        // a parent. If Backdrop was to have children, they would be updated
        // here. For simplicity, Backdrop itself is considered a "leaf" visual
        // element in this context.

        return false; // Backdrop usually doesn't consume events or change state
                      // in a way that needs bool return
    }

    void Backdrop::autosize() {
        // Backdrop typically fills its rect, or its texture's size if no
        // explicit size. For simplicity, we assume its size is managed by
        // setRect or a layout. If texture-based autosizing is needed, add logic
        // here: if (texture) {
        //     int w, h;
        //     SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
        //     setRect({rect.x, rect.y, w, h});
        // }
    }

    void Backdrop::updateBackdrop(Squidl::Core::UIContext &ctx,
                                  Squidl::Core::IRenderer &renderer) {
        SDL_Rect currentRect =
            getRect(); // Get the calculated position and size

        // Draw background color
        SDL_Color bgCol = getBackgroundColor();
        bgCol.a = static_cast<Uint8>(bgCol.a *
                                     getOpacity()); // Apply element's opacity
        renderer.drawFilledRect(currentRect, bgCol);

        // Draw texture if present
        if (texture) {
            renderer.drawTexture(texture, nullptr, &currentRect, getOpacity());
        }

        // Draw border if not borderless and border opacity > 0
        if (!isBorderless() && getBorderOpacity() > 0.0f) {
            SDL_Color borderCol = getBorderColor();
            borderCol.a = static_cast<Uint8>(
                borderCol.a * getBorderOpacity()); // Apply border opacity
            renderer.drawOutlineRect(currentRect, borderCol);
        }
    }

} // namespace Squidl::Elements
