// squidl/base/UIElement.cpp
#include "Squidl/base/UIElement.h"
#include "Squidl/utils/Logger.h" // For logging

namespace Squidl::Base {

    Squidl::Utils::UIRect UIElement::getRect() {
        // If managed by layout, its rect is already updated by the layout.
        // Otherwise, it's its own rect.
        return rect;
    }

    void UIElement::setRect(const Squidl::Utils::UIRect &newRect) {
        rect = newRect;
        applyConstraints(); // Apply constraints after setting new rect
    }

    Squidl::Utils::UIRect UIElement::getLocalRect() const {
        return Squidl::Utils::UIRect(position.x, position.y, rect.w, rect.h);
    }

    void
    UIElement::updateAnchoredRect(const Squidl::Utils::UIRect &parentRect) {
        if (!anchorOffsetsInitialized) {
            // Calculate initial offsets relative to parent
            anchorOffsetLeft = rect.x - parentRect.x;
            anchorOffsetRight =
                (parentRect.x + parentRect.w) - (rect.x + rect.w);
            anchorOffsetTop = rect.y - parentRect.y;
            anchorOffsetBottom =
                (parentRect.y + parentRect.h) - (rect.y + rect.h);
            anchorOffsetsInitialized = true;
        }

        int newX = rect.x;
        int newY = rect.y;
        int newW = rect.w;
        int newH = rect.h;

        if (static_cast<int>(anchors & Squidl::Core::UIAnchor::Left) > 0 &&
            static_cast<int>(anchors & Squidl::Core::UIAnchor::Right) > 0) {
            // Stretch horizontally
            newX = parentRect.x + anchorOffsetLeft;
            newW = parentRect.w - anchorOffsetLeft - anchorOffsetRight;
        } else if (static_cast<int>(anchors & Squidl::Core::UIAnchor::Left) >
                   0) {
            // Anchor to left
            newX = parentRect.x + anchorOffsetLeft;
        } else if (static_cast<int>(anchors & Squidl::Core::UIAnchor::Right) >
                   0) {
            // Anchor to right
            newX = (parentRect.x + parentRect.w) - (rect.w + anchorOffsetRight);
        }

        if (static_cast<int>(anchors & Squidl::Core::UIAnchor::Top) > 0 &&
            static_cast<int>(anchors & Squidl::Core::UIAnchor::Bottom) > 0) {
            // Stretch vertically
            newY = parentRect.y + anchorOffsetTop;
            newH = parentRect.h - anchorOffsetTop - anchorOffsetBottom;
        } else if (static_cast<int>(anchors & Squidl::Core::UIAnchor::Top) >
                   0) {
            // Anchor to top
            newY = parentRect.y + anchorOffsetTop;
        } else if (static_cast<int>(anchors & Squidl::Core::UIAnchor::Bottom) >
                   0) {
            // Anchor to bottom
            newY =
                (parentRect.y + parentRect.h) - (rect.h + anchorOffsetBottom);
        }

        // Update the element's rect
        setRect({newX, newY, newW, newH});
    }

} // namespace Squidl::Base
