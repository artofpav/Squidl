#include "Squidl/layouts/VBoxLayout.h"
#include "Squidl/elements/Backdrop.h"
#include <algorithm>

using namespace Squidl;

namespace Squidl::Layouts {

    VBoxLayout::VBoxLayout(int spacing_) { spacing = spacing_; }

    void VBoxLayout::setRect(const Utils::UIRect &newRect) {
        rect = newRect;

        const int innerW = std::max(0, rect.w - padding.pWidthSum());
        const int innerH = std::max(0, rect.h - padding.pHeightSum());

        // autosize для всех детей
        for (auto &child : children) {
            if (child)
                child->autosize();
        }

        // Рассчёт высоты слота для Stretch
        const int gaps = std::max(0, (int)children.size() - 1);
        int totalVMargins = 0, managedCount = 0;
        for (auto &ch : children) {
            if (!ch)
                continue;
            if (ch->isManagedByLayout()) {
                totalVMargins += ch->margin.top + ch->margin.bottom;
                managedCount++;
            }
        }
        int stretchSlotH = 0;
        if (getVerticalAlign() == Core::VerticalAlign::Stretch &&
            managedCount > 0) {
            const int availH = innerH - gaps * spacing - totalVMargins;
            stretchSlotH = std::max(0, availH / managedCount);
        }

        // Позиция курсора по вертикали
        int cursorY = rect.y + padding.top;

        for (auto &ch : children) {
            if (!ch)
                continue;
            if (!ch->isManagedByLayout())
                continue;

            auto c = ch->getRect();

            int childH = (getVerticalAlign() == Core::VerticalAlign::Stretch)
                             ? stretchSlotH
                             : c.h;

            // Слот для ребёнка
            Utils::UIRect slot = {
                rect.x + padding.left, cursorY + ch->margin.top,
                innerW - (ch->margin.left + ch->margin.right), childH};

            // Желаемый размер ребёнка (до выравнивания)
            Utils::UIRect desired = {0, 0, c.w, childH};

            auto useHX = childHXOverride.value_or(ch->getHorizontalAlign());
            auto useVY = childVYOverride.value_or(ch->getVerticalAlign());

            auto finalRect = alignInSlot(slot, desired, useHX, useVY);
            ch->setRect(finalRect);

            cursorY += childH + ch->margin.top + ch->margin.bottom + spacing;
        }

        applyConstraints();
    }

    bool VBoxLayout::update(Core::UIContext &ctx, Core::IRenderer &renderer) {
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

    void VBoxLayout::updateBackdrop(Core::UIContext &ctx,
                                    Core::IRenderer &renderer) {
        if (!backdrop)
            return;
        backdrop->setRect(rect);
        backdrop->setOpacity(opacity);
        backdrop->update(ctx, renderer);
    }

    void VBoxLayout::autosize() {
        if (!isManagedByChilds()) {
            for (auto &ch : children)
                if (ch)
                    ch->autosize();
            applyConstraints();
            return;
        }

        int totalH = padding.pHeightSum();
        int maxW = 0;
        int i = 0;
        for (auto &ch : children) {
            if (!ch)
                continue;
            ch->autosize();
            auto c = ch->getRect();
            totalH += c.h + ch->margin.top + ch->margin.bottom;
            if (i++ < (int)children.size() - 1)
                totalH += spacing;
            maxW = std::max(maxW, c.w + ch->margin.left + ch->margin.right);
        }
        const int totalW = maxW + padding.pWidthSum();

        setRect({rect.x, rect.y, totalW, totalH});
        applyConstraints();
    }

} // namespace Squidl::Layouts
