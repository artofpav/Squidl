#include "Squidl/layouts/HBoxLayout.h"
#include "Squidl/core/UIAlignment.h"
#include "Squidl/elements/Backdrop.h"
#include "Squidl/utils/Logger.h"
#include <iostream>

namespace Squidl::Layouts {
    HBoxLayout::HBoxLayout(int spacing_) : spacing(spacing_) {
        rect = {0, 0, 0, 0};
    }

    void HBoxLayout::setRect(const Squidl::Utils::UIRect &newRect) {
        rect = newRect;
        int availableWidth = rect.w - 2 * padding;
        int availableHeight = rect.h - 2 * padding;
        int maxChildWidth = (availableWidth - spacing*(children.size()-1)) / children.size();

        int childX = padding;
        int childY = padding;
        int childW = availableWidth;
        int childH = availableHeight;
        int index;
        int allChildsHeight = 0;
        int allChildsWidth = 0;
        int prevChildW = 0;

        for (auto &child : children) {
            child->autosize();
            allChildsHeight += child->getRect().h;
            allChildsWidth += child->getRect().w;
        }

        int stretchedWidth =
            (children.size() > 1)
                ? (rect.w - 2 * padding - (children.size() - 1) * spacing) /
                      children.size()
                : 0;

        int extraSpacing =
            (children.size() > 1)
                ? (availableWidth - allChildsWidth) / (children.size() - 1)
                : 0;

        for (auto &child : children) {
            Squidl::Utils::UIRect size = child->getRect();
            index = child->index;
            if (index - 1 >= 0) {
                prevChildW += children[index - 1]->getRect().w;
            }

            // --- Horizontal placement ---
            switch (getHorizontalAlign()) {
            case Squidl::Core::HorizontalAlign::Left:
                childX = padding + prevChildW + (index * spacing);
                childW = size.w;
                break;
            case Squidl::Core::HorizontalAlign::Center:
                childX = padding + (availableWidth / 2) - (allChildsWidth / 2) +
                         (size.w * index) + (index * spacing);
                childW = size.w;
                break;
            case Squidl::Core::HorizontalAlign::Right:
                // Original calculation might be off for Right alignment if not
                // managed by children This logic needs to align relative to the
                // *layout's* available width if not autosizing
                if (isManagedByChilds()) {
                    childX = availableWidth - allChildsWidth + prevChildW +
                             (index * spacing) - spacing;
                } else {
                    // If not managed by children, we need to respect the fixed
                    // width of the layout and distribute children within it.
                    // This is a simplified example.
                    childX = rect.w - padding - allChildsWidth + prevChildW +
                             (index * spacing) - spacing;
                }
                childW = size.w;
                break;
            case Squidl::Core::HorizontalAlign::Justify:
                childX = padding + prevChildW + (index * extraSpacing);
                childW = size.w;
                break;
            case Squidl::Core::HorizontalAlign::Stretch:
                childX = padding + index * (stretchedWidth + spacing);
                childW = stretchedWidth;
                break;
            }

            // --- Vertical alignment ---
            switch (getVerticalAlign()) {
            case Squidl::Core::VerticalAlign::Top:
                childY = padding;
                childH = size.h;
                break;
            case Squidl::Core::VerticalAlign::Center:
                childY = padding + (availableHeight / 2) - (size.h / 2);
                childH = size.h;
                break;
            case Squidl::Core::VerticalAlign::Bottom:
                childY = rect.h - padding - size.h;
                childH = size.h;
                break;
            case Squidl::Core::VerticalAlign::Justify:
            case Squidl::Core::VerticalAlign::Stretch:
                childY = padding;
                childH = availableHeight;
                break;
            }

            child->setPosition(childX, childY);

           // if (childW >= maxChildWidth)
                //childW = maxChildWidth;


            child->setRect({childX + rect.x, childY + rect.y, childW, childH});
        }

        applyConstraints();
    }

    bool HBoxLayout::update(Squidl::Core::UIContext &ctx,
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

    void HBoxLayout::updateBackdrop(Squidl::Core::UIContext &ctx,
                                    Squidl::Core::IRenderer &renderer) {
        if (backdrop) {

            backdrop->setRect(rect);
            ///
            std::cout << "Backdrop position: " << backdrop->getRect().x << ", "
                      << backdrop->getRect().y << std::endl;

            backdrop->setOpacity(opacity);
            backdrop->update(ctx, renderer);
        }
    }

    void HBoxLayout::autosize() {
        if (!isManagedByChilds()) {
            // If not managed by children, autosize should not change layout's
            // own dimensions significantly It should still call autosize on
            // children, but the layout's rect remains fixed.
            int maxChildH = 0;
            for (auto &child : children) {
                child->autosize();
                maxChildH = std::max(maxChildH, child->getRect().h);
            }
            rect.h = maxChildH + padding * 2;
            setRect(rect);
            applyConstraints();
            return;
        }

        int totalWidth = padding;
        int maxHeight = 0;

        for (auto &child : children) {
            child->autosize();
            SDL_Rect c = child->getRect();
            totalWidth += c.w + spacing;
            if (c.h > maxHeight)
                maxHeight = c.h;
        }

        if (!children.empty()) {
            totalWidth -= spacing;
        }

        totalWidth += padding;
        setRect({rect.x, rect.y, totalWidth, maxHeight + 2 * padding});

        applyConstraints();
    }
} // namespace Squidl::Layouts