// squidl/elements/Label.cpp
#include "Squidl/elements/Label.h"
#include "Squidl/core/IRenderer.h" // Include IRenderer.h
#include "Squidl/core/UIContext.h"
#include "Squidl/utils/Color.h"  // For Squidl::Utils::Color
#include "Squidl/utils/Logger.h" // For logging
#include "Squidl/utils/UIRect.h" // For Squidl::Utils::UIRect
#include <SDL.h>
#include <SDL_ttf.h>
#include <algorithm> // For std::clamp
#include <sstream>
#include <vector>

// For convenience, use using-directives within .cpp file
using namespace Squidl::Core;  // For UIContext, HorizontalAlign, VerticalAlign
using namespace Squidl::Utils; // For UIRect, Color

namespace Squidl::Elements { // <--- НАЧАЛО ПРОСТРАНСТВА ИМЕН ELEMENTS

    Label::Label(std::string text_, int x, int y, int w, int h, TTF_Font *font)
        : text(text_) { // Инициализируем font здесь
        setRect(UIRect(x, y, w, h));
        setFont(font);
        // Default colors for label (сохраняем ваши цвета по умолчанию)
        setBackgroundColor(Color(150, 150, 150, 0)); // Transparent background
        setBorderColor(Color(0, 0, 0, 0));           // No border by default
        fgColor = Color(255, 255, 255, 255);         // White foreground color
        setBorderless(true);                         // По умолчанию без рамки

        // Инициализация отступов по умолчанию
        paddingLeft = 5; // Отступы по умолчанию 5, как в Input.cpp
        paddingRight = 5;
        paddingTop = 5;
        paddingBottom = 5;
    }

    void Label::setText(std::string t) { text = t; }

    std::string Label::getText() const { return text; }

    // Реализация autosize с учетом отступов
    void Label::autosize() {
        if (!font || text.empty()) {
            rect.w = 0;
            rect.h = 0;
            applyConstraints();
            return;
        }

        int textWidth, textHeight;
        TTF_SizeUTF8(font, text.c_str(), &textWidth, &textHeight);

        // Учитываем отступы при автоматическом определении размера
        // Автоматически устанавливаем размер только если ширина/высота явно
        // равны 0, в противном случае сохраняем фиксированный размер.
        if (rect.w == 0) {
            rect.w = textWidth + paddingLeft + paddingRight;
        }
        if (rect.h == 0) {
            rect.h = textHeight + paddingTop + paddingBottom;
        }
        applyConstraints();
    }

    bool Label::update(Squidl::Core::UIContext &ctx,
                       Squidl::Core::IRenderer &renderer) {
        // Обновляем позицию привязки, если управляется макетом
        if (isManagedByLayout()) {
            // Убедимся, что родительский элемент существует и является
            // shared_ptr
            if (auto parent_shared = getParent()) {
                updateAnchoredRect(parent_shared->getRect());
            }
        }

        updateBackdrop(ctx, renderer); // Отрисовываем фон и рамку

        if (!font || text.empty())
            return false;

        std::vector<std::string> lines;
        std::istringstream stream(getText());
        std::string line;
        while (std::getline(stream, line,
                            '\n')) { // Разделяем по символу новой строки
            lines.push_back(line);
        }

        // Вычисляем общую высоту, необходимую для текста
        int lineHeight = TTF_FontHeight(font);
        int totalTextHeight = lines.size() * lineHeight;

        // Начальная позиция Y для текста с учетом вертикального выравнивания
        // Примечание: Отступы уже учтены в UIRect метки, установленном Input.
        // Поэтому отрисовка текста начинается относительно rect.y.
        int yOffset = rect.y;
        switch (m_verticalAlign) {
        case VerticalAlign::Top:
            yOffset = rect.y;
            break;
        case VerticalAlign::Center:
            yOffset = rect.y + (rect.h - totalTextHeight) / 2;
            break;
        case VerticalAlign::Bottom:
            yOffset = rect.y + rect.h - totalTextHeight;
            break;
        case VerticalAlign::Stretch: // Не применимо для прямой отрисовки текста
        case VerticalAlign::Justify: // Не применимо для простой отрисовки
                                     // текста
            yOffset = rect.y + (rect.h - totalTextHeight) /
                                   2; // По умолчанию - по центру
            break;
        }

        // Область клиппирования для отрисовки текста
        // Это собственный UIRect метки, который Input устанавливает в свою
        // область содержимого.
        UIRect clippingRect = getRect();

        // Устанавливаем область отсечения рендерера перед отрисовкой текста
        // Предполагается, что IRenderer предоставляет эти методы.
        renderer.setClipRect(clippingRect);

        // Отрисовываем каждую строку
        for (const auto &lineText : lines) {
            // Начальное смещение xOffset для текста, относительно
            // clippingRect.x Это то место, где текст *начинался бы*, если бы не
            // было горизонтального выравнивания или смещения текста.
            int xOffset = clippingRect.x;
            int line_width_actual = 0;
            TTF_SizeUTF8(font, lineText.c_str(), &line_width_actual, nullptr);

            // Применяем горизонтальное выравнивание в пределах ширины
            // clippingRect
            switch (m_horizontalAlign) {
            case HorizontalAlign::Left:
                // xOffset остается clippingRect.x
                break;
            case HorizontalAlign::Center:
                xOffset =
                    clippingRect.x + (clippingRect.w - line_width_actual) / 2;
                break;
            case HorizontalAlign::Right:
                xOffset = clippingRect.x + clippingRect.w - line_width_actual;
                break;
            // ... (Stretch, Justify - по умолчанию по центру для простого
            // текста)
            default:
                break;
            }

            // Применяем смещение текста, полученное от Input
            // Это сдвигает *содержимое* текста внутри области клиппирования
            xOffset += m_textOffsetX;

            // UIRect для drawText должен указывать фактическую позицию
            // отрисовки и ее полную ширину. Рендерер будет обрабатывать
            // обрезку.
            UIRect textRenderPosRect = {xOffset, yOffset, line_width_actual,
                                        lineHeight};

            renderer.drawText(font, lineText, fgColor, textRenderPosRect);
            yOffset += lineHeight; // Переходим на следующую строку
        }

        // Сбрасываем область отсечения рендерера после отрисовки текста
        renderer.resetClipRect();

        return false; // Метка обычно не потребляет события
    }

    void Label::updateBackdrop(Squidl::Core::UIContext &ctx,
                               Squidl::Core::IRenderer &renderer) {
        UIRect currentRect = getRect(); // <--- Теперь возвращает UIRect
        // Применяем прозрачность элемента к фону и рамке
        Color bgCol = getBackgroundColor(); // <--- Теперь возвращает Color
        // Отрисовываем фон, только если он не полностью прозрачный
        if (bgCol.a > 0) {
            bgCol.a = static_cast<Uint8>(bgCol.a * getOpacity());
            renderer.drawFilledRect(currentRect, bgCol);
        }

        if (!isBorderless() && getBorderOpacity() > 0.0f) {
            Color borderCol = getBorderColor(); // <--- Теперь возвращает Color
            borderCol.a = static_cast<Uint8>(borderCol.a * getBorderOpacity());
            renderer.drawOutlineRect(currentRect, borderCol);
        }
    }

    // Методы для установки и получения отступов
    void Label::setPadding(int p) {
        paddingLeft = paddingRight = paddingTop = paddingBottom = p;
    }
    void Label::setPadding(int horizontal, int vertical) {
        paddingLeft = paddingRight = horizontal;
        paddingTop = paddingBottom = vertical;
    }
    void Label::setPadding(int left, int top, int right, int bottom) {
        paddingLeft = left;
        paddingTop = top;
        paddingRight = right;
        paddingBottom = bottom;
    }

} // namespace Squidl::Elements
