#include "Squidl/layouts/VBoxLayout.h"
#include "Squidl/elements/Backdrop.h"
#include <iostream>

namespace Squidl::Layouts {
    VBoxLayout::VBoxLayout(int spacing_) {
        spacing = spacing_;
        rect = {0, 0, 0, 0};
    }

    void VBoxLayout::setRect(const Squidl::Utils::UIRect &newRect) {
        rect = newRect;

        // The available space for children is the layout's rect minus its
        // padding.
        int availableWidth = rect.w - padding.pWeightSum();
        int childY = padding.top;

        // Calculate total minimum height of all children
        int totalChildrenMinHeight = 0;
        for (const auto &child : children) {
            if (child->isManagedByLayout()) {
                totalChildrenMinHeight += child->getRect().h +
                                          child->margin.top +
                                          child->margin.bottom;
            }
        }
        // Add spacing between children
        if (!children.empty()) {
            totalChildrenMinHeight += (children.size() - 1) * spacing;
        }

        // Determine the starting Y position based on vertical alignment
        switch (getVerticalAlign()) {
        case Squidl::Core::VerticalAlign::Top:
        case Squidl::Core::VerticalAlign::Stretch:
            childY = padding.top;
            break;
        case Squidl::Core::VerticalAlign::Center:
            childY =
                padding.top +
                (rect.h - padding.pHeightSum() - totalChildrenMinHeight) / 2;
            break;
        case Squidl::Core::VerticalAlign::Bottom:
            childY = rect.h - padding.bottom - totalChildrenMinHeight;
            break;
        case Squidl::Core::VerticalAlign::Justify:
            // Justify logic needs more complex calculation
            childY = padding.top;
            break;
        }

        for (auto &child : children) {
            if (child->isManagedByLayout()) {
                int childW;
                int childH;

                // Calculate child width based on horizontal alignment
                switch (getHorizontalAlign()) {
                case Squidl::Core::HorizontalAlign::Left:
                case Squidl::Core::HorizontalAlign::Right:
                case Squidl::Core::HorizontalAlign::Center:
                    // Child's width is its own size, clamped by available space
                    childW = std::min(child->getRect().w,
                                      availableWidth - child->margin.left -
                                          child->margin.right);
                    break;
                case Squidl::Core::HorizontalAlign::Stretch:
                case Squidl::Core::HorizontalAlign::Justify:
                    // Child stretches to fill available width
                    childW = availableWidth - child->margin.left -
                             child->margin.right;
                    break;
                }

                // Calculate child height
                if (getVerticalAlign() ==
                    Squidl::Core::VerticalAlign::Stretch) {
                    // If the layout is stretched, distribute height among
                    // children
                    childH = (rect.h - padding.pHeightSum() -
                              (children.size() - 1) * spacing) /
                             children.size();
                } else {
                    // Otherwise, the child's height is its autosized height
                    childH = child->getRect().h;
                }

                // Set child position
                int childX;
                switch (getHorizontalAlign()) {
                case Squidl::Core::HorizontalAlign::Left:
                    childX = padding.left + child->margin.left;
                    break;
                case Squidl::Core::HorizontalAlign::Right:
                    childX = rect.w - padding.right - childW -
                             child->margin.right;
                    break;
                case Squidl::Core::HorizontalAlign::Center:
                    childX = padding.left + child->margin.left +
                             (availableWidth - childW - child->margin.left -
                              child->margin.right) /
                                 2;
                    break;
                case Squidl::Core::HorizontalAlign::Stretch:
                case Squidl::Core::HorizontalAlign::Justify:
                    childX = padding.left + child->margin.left;
                    break;
                }

                // Apply the final rect to the child
                child->setRect({childX + rect.x,
                                childY + rect.y + child->margin.top, childW,
                                childH});

                // Update the Y position for the next child
                childY += childH + child->margin.top +
                          child->margin.bottom + spacing;
            } else {
                // For unmanaged children, their position is relative to the
                // layout's rect. You might have a separate function here for
                // anchoring logic.
            }
        }
    }

    bool VBoxLayout::update(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) {
        if (backdrop) {
            updateBackdrop(ctx, renderer);
        } else {
            Squidl::Utils::Color currentBgColor = getBackgroundColor();
            currentBgColor.a =
                static_cast<Uint8>(currentBgColor.a * getOpacity());
            renderer.drawFilledRect(rect, bgColor);

            if (!isBorderless() && getBorderOpacity() > 0.0f) {
                Squidl::Utils::Color borderCol =
                    getBorderColor(); // <--- Now returns Color
                borderCol.a = static_cast<Uint8>(
                    borderCol.a * getBorderOpacity()); // Apply border opacity
                renderer.drawOutlineRect(rect, borderCol);
            }
        }

        bool anyActivated = false;
        for (auto &child : children) {
            anyActivated |= child->update(ctx, renderer);
        }
        return anyActivated;
    }

    void VBoxLayout::updateBackdrop(Squidl::Core::UIContext &ctx,
                                    Squidl::Core::IRenderer &renderer) {
        if (backdrop) {

            backdrop->setRect(rect);
            backdrop->setOpacity(opacity); // ← наследуем прозрачность
            backdrop->update(ctx, renderer);
        }
    }

    void VBoxLayout::autosize() {
        if (!managedByChilds) {
            // If not managed by children, layout's size is
            // fixed. We only apply constraints. The size should
            // be set externally.
            applyConstraints();
            return;
        }

        // Calculate required size based on children
        int totalHeight = 0;
        int maxWidth = 0;

        // First pass: let children autosize themselves and
        // calculate required size
        for (const auto &child : children) {
            child->autosize();
            Squidl::Utils::UIRect c = child->getRect();

            // The child's required width includes its
            // horizontal margins
            int childTotalWidth =
                c.w + child->margin.left + child->margin.right;
            if (childTotalWidth > maxWidth) {
                maxWidth = childTotalWidth;
            }

            // The child's required height includes its vertical
            // margins and spacing
            totalHeight += c.h + child->margin.top + child->margin.bottom;
        }

        // Adjust total height by subtracting the last spacing,
        // as it's not needed
        if (!children.empty()) {
            totalHeight += (children.size() - 1) * spacing;
        }

        // Add layout's padding to the total size
        totalHeight += padding.pHeightSum();
        maxWidth += padding.pWeightSum();

        // Set the new rect dimensions
        setRect({rect.x, rect.y, maxWidth, totalHeight});
        applyConstraints();
    }
} // namespace Squidl::Layouts
