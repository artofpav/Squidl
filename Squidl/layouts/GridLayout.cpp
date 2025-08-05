#include "Squidl/layouts/GridLayout.h"
#include "Squidl/elements/Backdrop.h"
#include "Squidl/utils/UIRect.h"

namespace Squidl::Layouts {

    GridLayout::GridLayout(int columns_, int spacing_, int padding_)
        : columns(columns_), spacing(spacing_), padding(padding_) {}

    void GridLayout::setRect(const Squidl::Utils::UIRect &newRect) {
        rect = newRect;

        int availableWidth = rect.w - 2 * padding;
        int availableHeight = rect.h - 2 * padding;

        int cellW = (availableWidth - (columns - 1) * spacing) / columns;
        int x = rect.x + padding;
        int y = rect.y + padding;

        int col = 0;
        int rowHeight = 0;
        int currentRowMaxChildHeight =
            0; // Track max height in current row for correct Y advancement
        int currentX = padding; // Relative X position within the layout

        for (auto &child : children) {
            child->autosize();
        }

        // If not managed by children, we need to pre-calculate sizes for fixed
        // layout
        if (!isManagedByChilds()) {
            // Distribute children according to the fixed grid cell size
            int currentY = padding;
            int currentColumn = 0;
            int maxRowHeightInFixedGrid = 0;

            for (size_t i = 0; i < children.size(); ++i) {
                auto &child = children[i];
                Squidl::Utils::UIRect childCalculatedSize =
                    child->getRect(); // Use child's autosized dimensions

                int childRenderW = cellW; // Children will stretch to cellW
                int childRenderH =
                    childCalculatedSize
                        .h; // Children keep their autosized height

                int childDrawX =
                    rect.x + padding + currentColumn * (cellW + spacing);
                int childDrawY = rect.y + currentY;

                child->setRect(
                    {childDrawX, childDrawY, childRenderW, childRenderH});
                child->setManagedByLayout(true);

                maxRowHeightInFixedGrid =
                    std::max(maxRowHeightInFixedGrid, childRenderH);

                currentColumn++;
                if (currentColumn >= columns) {
                    currentColumn = 0;
                    currentY += maxRowHeightInFixedGrid + spacing;
                    maxRowHeightInFixedGrid = 0;
                }
            }
        } else {
            // Original logic for when layout dimensions are managed by children
            int currentY = padding;
            int currentColumn = 0;
            int currentRowMaxHeight = 0;
            for (size_t i = 0; i < children.size(); ++i) {
                auto &child = children[i];
                Squidl::Utils::UIRect size = child->getRect();

                int childX =
                    padding +
                    currentColumn *
                        (cellW + spacing); // Position within the layout
                int childY = currentY;

                child->setRect(
                    {childX + rect.x, childY + rect.y, cellW, size.h});
                child->setManagedByLayout(true);

                currentRowMaxHeight = std::max(currentRowMaxHeight, size.h);

                currentColumn++;
                if (currentColumn >= columns) {
                    currentColumn = 0;
                    currentY += currentRowMaxHeight + spacing;
                    currentRowMaxHeight = 0;
                }
            }
        }

        applyConstraints();
    }

    bool GridLayout::update(Squidl::Core::UIContext &ctx,
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

    void GridLayout::updateBackdrop(Squidl::Core::UIContext &ctx,
                                    Squidl::Core::IRenderer &renderer) {
        if (backdrop) {
            backdrop->setRect(rect);
            backdrop->setOpacity(opacity);
            backdrop->update(ctx, renderer);
        }
    }

    void GridLayout::autosize() {
        if (!isManagedByChilds()) {
            for (auto &child : children) {
                child->autosize();
            }
            applyConstraints();
            return;
        }

        int totalHeight = 2 * padding;
        int totalWidth = 0;
        int currentRowWidth = 0;
        int currentRowMaxHeight = 0;
        int col = 0;

        for (auto &child : children) {
            child->autosize();
            Squidl::Utils::UIRect c = child->getRect();

            if (col == 0) {
                currentRowWidth = 2 * padding; // Start of a new row
            }

            currentRowWidth += c.w + spacing;
            currentRowMaxHeight = std::max(currentRowMaxHeight, c.h);

            col++;
            if (col >= columns) {
                totalWidth =
                    std::max(totalWidth,
                             currentRowWidth -
                                 spacing); // Remove last spacing for this row
                totalHeight += currentRowMaxHeight + spacing;
                currentRowWidth = 0;     // Reset for next row
                currentRowMaxHeight = 0; // Reset for next row
                col = 0;
            }
        }

        // Handle the last row if it's not full
        if (col > 0) {
            totalWidth = std::max(totalWidth, currentRowWidth - spacing);
            totalHeight +=
                currentRowMaxHeight; // No extra spacing at the very end
        }

        totalHeight -= spacing; // Remove the extra spacing added at the end of
                                // the last full row
        totalHeight += padding; // Add padding for the bottom

        // If there are no children, ensure minimum padding is applied
        if (children.empty()) {
            totalWidth = 2 * padding;
            totalHeight = 2 * padding;
        }

        setRect({rect.x, rect.y, totalWidth, totalHeight});
        applyConstraints();
    }
} // namespace Squidl::Layouts