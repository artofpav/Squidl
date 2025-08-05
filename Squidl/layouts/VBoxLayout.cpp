#include "Squidl/layouts/VBoxLayout.h"
#include "Squidl/elements/Backdrop.h"
#include <iostream>

namespace Squidl::Layouts {
    VBoxLayout::VBoxLayout(int spacing_){
        spacing = spacing_;
        rect = {0, 0, 0, 0};
    }

    void VBoxLayout::setRect(const Squidl::Utils::UIRect &newRect) {
        rect = newRect;
        int availableWidth = 9;//rect.w - 2 * padding;
        int availableHeight = rect.h - padding.pHeightSum();
        int childY = padding.top;
        int childX = padding.left;
        int childW = availableWidth;
        int childH = availableHeight;
        int index;
        int allChildsHeight = 0;
        int prevChildH = 0;

        for (auto &child : children) {
            child->autosize();
            allChildsHeight += child->getRect().h;
            if(managedByChilds) {
                rect.w = std::max(rect.w - padding.pWeightSum(), child->getRect().w) +
                         padding.pHeightSum();
            }
            
        }

        availableWidth = rect.w - padding.pWeightSum();

        int stretchedHeight =
            (children.size() > 1)
                ? (rect.h - padding.pHeightSum() - (children.size() - 1) * spacing) /
                      children.size()
                : 0;
        int extraSpacing =
            (children.size() > 1)
                ? (availableHeight - allChildsHeight) / (children.size() - 1)
                : 0;

        for (auto &child : children) {
            // пусть каждый сам установит свой размер
            SDL_Rect size = child->getRect();
            index = child->index;

            if (index - 1 >= 0) {
                prevChildH += children[index - 1]->getRect().h;
            }

            switch (getVerticalAlign()) {
            case Squidl::Core::VerticalAlign::Top:
                childY = padding.Top + prevChildH + (index * spacing);
                childH = size.h;
                break;
            case Squidl::Core::VerticalAlign::Center:
                childY = padding.Top + (availableHeight / 2) -
                         (allChildsHeight / 2) + (size.h * index) +
                         (index * spacing);
                childH = size.h;
                break;
            case Squidl::Core::VerticalAlign::Bottom:
                childY = availableHeight - allChildsHeight + prevChildH +
                         (index * spacing) - spacing;
                childH = size.h;
                break;
            case Squidl::Core::VerticalAlign::Justify:
                childY = padding + (size.h * index) + (index * extraSpacing);
                childH = size.h;
                break;
            case Squidl::Core::VerticalAlign::Stretch:
                childY = padding + index * (stretchedHeight + spacing);
                childH = stretchedHeight;
                break;
            }

            switch (getHorizontalAlign()) {
            case Squidl::Core::HorizontalAlign::Left:
                childX = padding;
                childW = size.w;
                break;
            case Squidl::Core::HorizontalAlign::Center:
                childX = padding + (availableWidth / 2) - (size.w / 2);
                childW = size.w;
                break;
            case Squidl::Core::HorizontalAlign::Right:
                childX = rect.w - padding - size.w;
                childW = size.w;
                break;
            case Squidl::Core::HorizontalAlign::Justify:
            case Squidl::Core::HorizontalAlign::Stretch:
                childX = padding;
                childW = availableWidth;
                break;
            }

            child->setPosition(childX, childY);
            child->setRect({childX + rect.x, childY + rect.y, childW, childH});
            // y += size.h + spacing;
        }

        applyConstraints();
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
        int totalHeight = padding;
        int maxWidth = 0;

        for (auto &child : children) {
            child->autosize();
            Squidl::Utils::UIRect c = child->getRect();
            totalHeight += c.h + spacing;
            if (c.w > maxWidth)
                maxWidth = c.w;
        }

        if (!children.empty()) {
            totalHeight -= spacing; // убрать последний лишний spacing
        }
        totalHeight += padding.top;
        if(managedByChilds)
            setRect({rect.x, rect.y, maxWidth, totalHeight + padding.top});
        else 
            setRect({rect.x, rect.y, rect.w, rect.h});
        applyConstraints();
    }
} // namespace Squidl::Layouts
