// HBoxLayout.cpp
#include "Squidl/layouts/HBoxLayout.h"
#include "Squidl/elements/Backdrop.h"

using namespace Squidl;

namespace Squidl::Layouts {

    HBoxLayout::HBoxLayout(int spacing_) { spacing = spacing_; }

    void HBoxLayout::setRect(const Utils::UIRect &newRect) {
        rect = newRect;

        const int availW = rect.w - padding.pWidthSum();
        const int availH = rect.h - padding.pHeightSum();

        // Считаем суммарные горизонтальные margin управляемых детей
        int totalHorzMargins = 0, managedCount = 0;
        for (auto &ch : children)
            if (ch->isManagedByLayout()) {
                totalHorzMargins += ch->margin.left + ch->margin.right;
                managedCount++;
            }
        const int gaps = std::max(0, (int)children.size() - 1);

        int stretchSlotW = 0;
        if (getHorizontalAlign() == Core::HorizontalAlign::Stretch &&
            managedCount > 0) {
            const int widthForChildren =
                availW - totalHorzMargins - gaps * spacing;
            stretchSlotW = std::max(0, widthForChildren / managedCount);
        }

        // Начальный X по верт. выравниванию не влияет; Y зависит от
        // VerticalAlign
        int childX = rect.x + padding.left;

        for (auto &ch : children) {
            if (!ch->isManagedByLayout())
                continue;

            auto c = ch->getRect();
            int cw = (getHorizontalAlign() == Core::HorizontalAlign::Stretch)
                         ? stretchSlotW
                         : c.w;
            cw = std::max(0, cw);

            // Высота
            int chH = (getVerticalAlign() == Core::VerticalAlign::Stretch)
                          ? availH - (ch->margin.top + ch->margin.bottom)
                          : c.h;

            int cx = childX + ch->margin.left;
            int cy = rect.y + padding.top; // базовая точка

            switch (getVerticalAlign()) {
            case Core::VerticalAlign::Top:
                cy += ch->margin.top;
                break;
            case Core::VerticalAlign::Center:
                cy += (availH - chH) / 2;
                break;
            case Core::VerticalAlign::Bottom:
                cy += availH - chH - ch->margin.bottom;
                break;
            case Core::VerticalAlign::Stretch:
            case Core::VerticalAlign::Justify:
                // Stretch: уже растянули chH
                break;
            }
            
            // внутри цикла по children, после вычисления cw/chH и cx/cy:
            Squidl::Utils::UIRect slot = {
                cx, rect.y + padding.top, std::max(0, cw),
                std::max(0, (rect.h - padding.pHeightSum()))};
            // "желаемый" размер ребёнка до выравнивания (его autosize/rect.h)
            Squidl::Utils::UIRect desired = {0, 0, c.w, chH};

            // выберем выравнивание: override от лэйаута ИЛИ per-child
            auto useHX = childHXOverride.value_or(ch->getHorizontalAlign()); //
            auto useVY = childVYOverride.value_or(ch->getVerticalAlign());   //

            // финальный прямоугольник
            auto finalRect = alignInSlot(slot, desired, useHX, useVY);

            // учесть margin (мы строили slot уже с учётом margin.left/right в
            // твоём коде; если нет — вычти тут)
            ch->setRect(finalRect);
            ch->setRect({cx, cy, cw, chH});
            childX += cw + ch->margin.left + ch->margin.right + spacing;
        }
    }

    bool HBoxLayout::update(Core::UIContext &ctx, Core::IRenderer &renderer) {
        if (backdrop) {
            updateBackdrop(ctx, renderer);
        } else {
            auto col = getBackgroundColor();
            col.a = (Uint8)(col.a * getOpacity());
            renderer.drawFilledRect(rect, col);

            if (!isBorderless() && getBorderOpacity() > 0.f) {
                auto bc = getBorderColor();
                bc.a = (Uint8)(bc.a * getBorderOpacity());
                renderer.drawOutlineRect(rect, bc);
            }
        }
        bool any = false;
        for (auto &ch : children)
            any |= ch->update(ctx, renderer);
        return any;
    }

    void HBoxLayout::updateBackdrop(Core::UIContext &ctx,
                                    Core::IRenderer &renderer) {
        if (!backdrop)
            return;
        backdrop->setRect(rect);
        backdrop->setOpacity(opacity);
        backdrop->update(ctx, renderer);
    }

    void HBoxLayout::autosize() {
        if (!managedByChilds) {
            applyConstraints();
            return;
        }

        // ширина = сумма детей + margins + spacing + padding
        int totalW = padding.pWidthSum();
        int maxH = 0;
        int i = 0;
        for (auto &ch : children) {
            ch->autosize();
            auto c = ch->getRect();
            totalW += c.w + ch->margin.left + ch->margin.right;
            if (i++ < (int)children.size() - 1)
                totalW += spacing;
            maxH = std::max(maxH, c.h + ch->margin.top + ch->margin.bottom);
        }
        const int totalH = maxH + padding.pHeightSum();
        setRect({rect.x, rect.y, totalW, totalH});
        applyConstraints();
    }

} // namespace Squidl::Layouts
