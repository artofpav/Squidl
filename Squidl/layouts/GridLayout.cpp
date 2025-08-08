#include "Squidl/layouts/GridLayout.h"
#include "Squidl/elements/Backdrop.h"
#include <algorithm>

using namespace Squidl;

namespace Squidl::Layouts {

    GridLayout::GridLayout(int columns_, int spacing_, int padding_) : columns(columns_) {
        if (columns < 1)
            columns = 1;
        spacing = spacing_;
        padding = padding_;
    }

    void GridLayout::setRect(const Utils::UIRect &newRect) {
        rect = newRect;

        const int innerW = std::max(0, rect.w - padding.pWidthSum());

        for (auto &child : children) {
            if (child)
                child->autosize();
        }

        const int gapsX = std::max(0, columns - 1);
        const int cellW =
            (columns > 0) ? std::max(0, (innerW - gapsX * spacing) / columns)
                          : innerW;

        int col = 0;
        int cursorY = rect.y + padding.top;
        int rowMaxH = 0;

        for (auto &ch : children) {
            if (!ch)
                continue;
            ch->setManagedByLayout(true);

            auto c = ch->getRect();
            const int childMH = ch->margin.left + ch->margin.right;
            const int childMV = ch->margin.top + ch->margin.bottom;

            // Слот
            Utils::UIRect slot = {
                rect.x + padding.left + col * (cellW + spacing) +
                    ch->margin.left,
                cursorY + ch->margin.top, std::max(0, cellW - childMH),
                std::max(0, c.h) // или высота строки, если нужен ровный ряд
            };

            Utils::UIRect desired = {0, 0, c.w, c.h};

            auto useHX = childHXOverride.value_or(ch->getHorizontalAlign());
            auto useVY = childVYOverride.value_or(ch->getVerticalAlign());

            auto finalRect = alignInSlot(slot, desired, useHX, useVY);
            ch->setRect(finalRect);

            rowMaxH = std::max(rowMaxH, c.h + childMV);

            if (++col >= columns) {
                col = 0;
                cursorY += rowMaxH + spacing;
                rowMaxH = 0;
            }
        }

        applyConstraints();
    }

    bool GridLayout::update(Core::UIContext &ctx, Core::IRenderer &renderer) {
        if (backdrop) {
            updateBackdrop(ctx, renderer);
        } else {
            Utils::Color col = getBackgroundColor();
            col.a = static_cast<Uint8>(col.a * getOpacity());
            renderer.drawFilledRect(rect, col);

            if (!isBorderless() && getBorderOpacity() > 0.0f) {
                Utils::Color bc = getBorderColor();
                bc.a = static_cast<Uint8>(bc.a * getBorderOpacity());
                renderer.drawOutlineRect(rect, bc);
            }
        }

        bool any = false;
        for (auto &ch : children) {
            if (ch)
                any |= ch->update(ctx, renderer);
        }
        return any;
    }

    void GridLayout::updateBackdrop(Core::UIContext &ctx,
                                    Core::IRenderer &renderer) {
        if (!backdrop)
            return;
        backdrop->setRect(rect);
        backdrop->setOpacity(opacity);
        backdrop->update(ctx, renderer);
    }

    void GridLayout::autosize() {
        if (!isManagedByChilds()) {
            for (auto &ch : children)
                if (ch)
                    ch->autosize();
            applyConstraints();
            return;
        }

        int maxCellW = 0;
        int rows = 0;
        int rowMaxH = 0;
        int col = 0;
        int totalHeight = 0;

        for (auto &ch : children) {
            if (!ch)
                continue;
            ch->autosize();
            auto c = ch->getRect();

            maxCellW = std::max(maxCellW, c.w);

            const int childMV = ch->margin.top + ch->margin.bottom;
            rowMaxH = std::max(rowMaxH, c.h + childMV);

            if (++col >= columns) {
                ++rows;
                totalHeight += rowMaxH;
                rowMaxH = 0;
                col = 0;
            }
        }
        if (col > 0) {
            ++rows;
            totalHeight += rowMaxH;
        }

        const int totalWidth = padding.pWidthSum() +
                               std::max(0, columns) * maxCellW +
                               std::max(0, columns - 1) * spacing;

        totalHeight = padding.pHeightSum() + totalHeight +
                      std::max(0, rows - 1) * spacing;

        setRect({rect.x, rect.y, totalWidth, totalHeight});
        applyConstraints();
    }

} // namespace Squidl::Layouts
