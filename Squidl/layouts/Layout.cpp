#include "Squidl/layouts/Layout.h"
#include "Squidl/core/IRenderer.h" // For IRenderer
#include "Squidl/core/UIContext.h" // For UIContext
#include "Squidl/utils/Color.h"    // For Squidl::Utils::Color
#include "Squidl/utils/Logger.h"   // For logging
#include "Squidl/utils/UIRect.h"   // For Squidl::Utils::UIRect

namespace Squidl::Layouts {

    void Layout::add(std::shared_ptr<Squidl::Base::UIElement> child) {
        if (child) {
            children.push_back(child);
            child->setParent(shared_from_this()); // Set parent to this layout
            child->setManagedByLayout(true);      // Mark as managed by layout
            // Inherit font if child doesn't have one
            if (!child->getFont() && font) {
                child->setFont(font);
            }
            child->index = static_cast<int>(children.size()) - 1;
        }
    }

    bool Layout::update(Squidl::Core::UIContext &ctx,
                        Squidl::Core::IRenderer &renderer) {
        // Update anchored position for the layout itself
        if (isManagedByLayout()) {
            updateAnchoredRect(getParent()->getRect());
        }

        updateBackdrop(ctx, renderer); // Draw layout's background and border

        // Update and render children
        for (const auto &child : children) {
            if (child) {
                // Pass the same IRenderer reference to children
                child->update(ctx, renderer);
            }
        }
        return false; // Layouts usually don't consume events directly
    }

    void Layout::setFont(TTF_Font *f) {
        UIElement::setFont(f); // Call base class method
        // Also update fonts for existing children
        for (const auto &child : children) {
            if (child &&
                !child->getFont()) { // Only if child doesn't have its own font
                child->setFont(f);
            }
        }
    }

    void Layout::updateBackdrop(Squidl::Core::UIContext &ctx,
                                Squidl::Core::IRenderer &renderer) {
        Squidl::Utils::UIRect currentRect =
            getRect(); // <--- Now returns UIRect
        Squidl::Utils::Color bgCol =
            getBackgroundColor(); // <--- Now returns Color
        bgCol.a = static_cast<Uint8>(bgCol.a * getOpacity());
        renderer.drawFilledRect(currentRect, bgCol);

        if (!isBorderless() && getBorderOpacity() > 0.0f) {
            Squidl::Utils::Color borderCol =
                getBorderColor(); // <--- Now returns Color
            borderCol.a = static_cast<Uint8>(borderCol.a * getBorderOpacity());
            renderer.drawOutlineRect(currentRect, borderCol);
        }
    }

} // namespace Squidl::Layouts
