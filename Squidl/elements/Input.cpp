// Input.cpp
#include "Squidl/elements/Input.h"
#include "Squidl/core/IRenderer.h"
#include "Squidl/core/UIContext.h"
#include "Squidl/utils/Logger.h"
#include <SDL_ttf.h>
#include <algorithm> // Для std::min, std::max

namespace Squidl::Elements {

    Input::Input(std::string placeholderText, int x, int y, int w, int h,
                 TTF_Font *font)
        : placeholderText(placeholderText), font(font) {
        setRect(Squidl::Utils::UIRect(x, y, w, h));
        setBackgroundColor(
            Squidl::Utils::Color(255, 255, 255, 200));      // Белый фон
        setBorderColor(Squidl::Utils::Color(0, 0, 0, 255)); // Черная рамка
        setBorderless(false);

        label = std::make_shared<Squidl::Elements::Label>("", x, y, w, h, font);
        label->setHorizontalAlignment(Squidl::Core::HorizontalAlign::Left);
        label->setVerticalAlignment(Squidl::Core::VerticalAlign::Center);
        // Устанавливаем отступы для текста внутри поля ввода
        label->setPadding(5);

        updateLabelTextAndColor(); // Устанавливаем начальный текст и цвет
    }

    void Input::setText(const std::string &text) {
        currentText = text;
        updateLabelTextAndColor();
        if (onTextChange) {
            onTextChange(currentText);
        }
        // Убедимся, что курсор не выходит за пределы текста
        cursorPosition = std::min(cursorPosition, (int)currentText.length());
        adjustTextOffset(); // Корректируем смещение текста при изменении текста
    }

    std::string Input::getText() const { return currentText; }

    void Input::setPlaceholderText(const std::string &text) {
        placeholderText = text;
        updateLabelTextAndColor();
    }

    void Input::updateLabelTextAndColor() {
        if (label) {
            if (currentText.empty() && !focused) {
                label->setText(placeholderText);
                label->setTextColor(placeholderColor);
            } else {
                label->setText(currentText);
                label->setTextColor(textColor);
            }
        }
    }

    void Input::onEvent(Squidl::Core::UIEvent &event) {
        Squidl::Utils::UIRect currentRect = getRect();

        if (event.type == Squidl::Core::EventType::MouseEvent) {
            auto &mouseEvent = static_cast<Squidl::Core::MouseEvent &>(event);
            bool mouseOver = currentRect.contains(mouseEvent.position.x,
                                                  mouseEvent.position.y);

            if (mouseEvent.mouseEventType ==
                    Squidl::Core::MouseEventType::ButtonPressed &&
                mouseEvent.button == SDL_BUTTON_LEFT) {
                if (mouseOver) {
                    if (!focused) {
                        focused = true;
                        SDL_StartTextInput(); // Начинаем принимать текстовый
                                              // ввод
                        SQUIDL_LOG_INFO << "Input gained focus.";
                    }
                    // Устанавливаем позицию курсора по клику
                    cursorPosition = getCharIndexAt(mouseEvent.position.x);
                    cursorTimer = SDL_GetTicks(); // Сброс таймера курсора
                    showCursor = true;    // Показать курсор сразу после клика
                    adjustTextOffset();   // Корректируем смещение текста
                    event.handled = true; // Событие обработано
                } else {
                    if (focused) {
                        focused = false;
                        SDL_StopTextInput(); // Останавливаем прием текстового
                                             // ввода
                        SQUIDL_LOG_INFO << "Input lost focus.";
                    }
                }
                updateLabelTextAndColor(); // Обновляем цвет, если фокус
                                           // изменился
            }
        } else if (event.type == Squidl::Core::EventType::TextInputEvent) {
            if (focused) {
                auto &textEvent =
                    static_cast<Squidl::Core::TextInputEvent &>(event);
                currentText.insert(cursorPosition, textEvent.text);
                cursorPosition += textEvent.text.length();
                setText(currentText); // Обновляем текст и вызываем callback
                cursorTimer = SDL_GetTicks(); // Сброс таймера курсора
                showCursor = true;  // Показать курсор сразу после ввода
                adjustTextOffset(); // Корректируем смещение текста
                event.handled = true;
            }
        } else if (event.type == Squidl::Core::EventType::KeyboardEvent) {
            if (focused) {
                auto &keyEvent =
                    static_cast<Squidl::Core::KeyboardEvent &>(event);
                if (keyEvent.isPressed) { // Только при нажатии клавиши
                    if (keyEvent.scancode == SDL_SCANCODE_BACKSPACE) {
                        if (cursorPosition > 0) {
                            currentText.erase(cursorPosition - 1, 1);
                            cursorPosition--;
                            setText(currentText);
                            cursorTimer = SDL_GetTicks();
                            showCursor = true;
                            adjustTextOffset();
                            event.handled = true;
                        }
                    } else if (keyEvent.scancode == SDL_SCANCODE_DELETE) {
                        if (cursorPosition < currentText.length()) {
                            currentText.erase(cursorPosition, 1);
                            setText(currentText);
                            cursorTimer = SDL_GetTicks();
                            showCursor = true;
                            adjustTextOffset();
                            event.handled = true;
                        }
                    } else if (keyEvent.scancode == SDL_SCANCODE_LEFT) {
                        if (cursorPosition > 0) {
                            cursorPosition--;
                            cursorTimer = SDL_GetTicks();
                            showCursor = true;
                            adjustTextOffset();
                            event.handled = true;
                        }
                    } else if (keyEvent.scancode == SDL_SCANCODE_RIGHT) {
                        if (cursorPosition < currentText.length()) {
                            cursorPosition++;
                            cursorTimer = SDL_GetTicks();
                            showCursor = true;
                            adjustTextOffset();
                            event.handled = true;
                        }
                    } else if (keyEvent.scancode == SDL_SCANCODE_HOME) {
                        cursorPosition = 0;
                        cursorTimer = SDL_GetTicks();
                        showCursor = true;
                        adjustTextOffset();
                        event.handled = true;
                    } else if (keyEvent.scancode == SDL_SCANCODE_END) {
                        cursorPosition = currentText.length();
                        cursorTimer = SDL_GetTicks();
                        showCursor = true;
                        adjustTextOffset();
                        event.handled = true;
                    }
                }
            }
        }
    }

    bool Input::update(Squidl::Core::UIContext &ctx,
                       Squidl::Core::IRenderer &renderer) {
        Squidl::Utils::UIRect currentRect = getRect();
        bool hovered = currentRect.contains(ctx.getMouseX(), ctx.getMouseY());

        updateBackdrop(ctx, renderer);

        // UIRect метки должен представлять видимую область содержимого внутри
        // поля ввода.
        // Этот UIRect будет использоваться рендерером для клиппирования.
        Squidl::Utils::UIRect labelContentRect = {
            currentRect.x + label->getPaddingLeft(),
            currentRect.y + label->getPaddingTop(),
            currentRect.w - label->getPaddingLeft() - label->getPaddingRight(),
            currentRect.h - label->getPaddingTop() - label->getPaddingBottom()};
        label->setRect(
            labelContentRect); // Устанавливаем область клиппирования для метки

        // Передаем рассчитанное смещение текста метке.
        // Метка будет использовать это смещение при отрисовке своего текстового
        // содержимого.
        label->setTextOffset(textOffsetX);

        // Обновляем и отрисовываем метку. Метка будет отрисовывать свой текст
        // в пределах назначенного ей UIRect, применяя textOffsetX.
        label->update(ctx, renderer);

        // Логика мигания курсора
        if (focused) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - cursorTimer > 500) { // Мигать каждые 500 мс
                showCursor = !showCursor;
                cursorTimer = currentTime;
            }

            if (showCursor) {
                // Определяем позицию курсора с учетом textOffsetX
                // CursorX относительно начала области содержимого поля ввода
                // (labelContentRect.x)
                int cursorX = labelContentRect.x;
                if (font && cursorPosition > 0) {
                    std::string textBeforeCursor =
                        currentText.substr(0, cursorPosition);
                    int textWidth = 0;
                    TTF_SizeUTF8(font, textBeforeCursor.c_str(), &textWidth,
                                 nullptr);
                    cursorX += textWidth;
                }
                // Применяем смещение текста к визуальной позиции курсора
                cursorX += textOffsetX;

                // Ограничиваем позицию курсора видимой областью Input
                int visibleAreaStart = labelContentRect.x;
                int visibleAreaEnd = labelContentRect.x + labelContentRect.w;

                if (cursorX >= visibleAreaStart && cursorX <= visibleAreaEnd) {
                    // Отрисовка курсора
                    Squidl::Utils::UIRect cursorRect = {
                        cursorX, labelContentRect.y+5, 2, labelContentRect.h-10};
                    renderer.drawFilledRect(cursorRect, cursorColor);
                }
            }
        }

        return focused ||
               hovered; // Возвращаем true, если элемент в фокусе или наведен
    }

    void Input::updateBackdrop(Squidl::Core::UIContext &ctx,
                               Squidl::Core::IRenderer &renderer) {
        Squidl::Utils::UIRect currentRect = getRect();
        Squidl::Utils::Color currentBgColor = getBackgroundColor();
        Squidl::Utils::Color currentBorderColor = getBorderColor();

        // Применяем opacity к фону
        currentBgColor.a = static_cast<Uint8>(currentBgColor.a * getOpacity());
        renderer.drawFilledRect(currentRect, currentBgColor);

        // Изменяем цвет рамки при фокусе
        if (focused) {
            currentBorderColor = focusedBorderColor;
        }

        if (!isBorderless() && getBorderOpacity() > 0.0f) {
            currentBorderColor.a =
                static_cast<Uint8>(currentBorderColor.a * getBorderOpacity());
            renderer.drawOutlineRect(currentRect, currentBorderColor);
        }
    }

    void Input::autosize() {
        if (!font)
            return;

        if (label) {
            std::string textToMeasure =
                currentText.empty() ? placeholderText : currentText;
            label->setText(textToMeasure);
            // Label autosize должен просто измерять текст, не влияя на свой
            // собственный UIRect напрямую, когда он является дочерним элементом
            // Input. Input контролирует UIRect метки. Однако нам нужна ширина
            // текста для adjustTextOffset.
            int textWidth, textHeight;
            TTF_SizeUTF8(font, textToMeasure.c_str(), &textWidth, &textHeight);

            // Если ширина/высота Input равны 0, автоматически устанавливаем их
            // на основе измеренного текста + отступов
            if (rect.w == 0 || rect.h == 0) {
                setRect(
                    Squidl::Utils::UIRect(rect.x, rect.y,
                                          textWidth + label->getPaddingLeft() +
                                              label->getPaddingRight(),
                                          textHeight + label->getPaddingTop() +
                                              label->getPaddingBottom()));
            }
        }
        applyConstraints();
    }

    int Input::getCharIndexAt(int mouseX) {
        if (!font || currentText.empty()) {
            return 0;
        }

        // Позиция мыши относительно начала видимой области текста
        // (labelContentRect.x)
        int relativeX =
            mouseX - (getRect().x + label->getPaddingLeft() + textOffsetX);

        if (relativeX <= 0)
            return 0;

        int closestIndex = 0;
        int minDiff = 99999;

        for (int i = 0; i <= currentText.length(); ++i) {
            std::string sub = currentText.substr(0, i);
            int subWidth = 0;
            TTF_SizeUTF8(font, sub.c_str(), &subWidth, nullptr);

            int diff = std::abs(relativeX - subWidth);
            if (diff < minDiff) {
                minDiff = diff;
                closestIndex = i;
            }
        }
        return closestIndex;
    }

    void Input::adjustTextOffset() {
        if (!font)
            return;

        int textWidth = 0;
        TTF_SizeUTF8(font, currentText.c_str(), &textWidth, nullptr);

        // Вычисляем ширину текста до курсора
        int cursorVisualX = 0;
        if (cursorPosition > 0) {
            std::string textBeforeCursor =
                currentText.substr(0, cursorPosition);
            TTF_SizeUTF8(font, textBeforeCursor.c_str(), &cursorVisualX,
                         nullptr);
        }

        // Видимая ширина для текста внутри поля ввода, с учетом отступов
        int visibleWidth =
            rect.w - label->getPaddingLeft() - label->getPaddingRight();

        // Если текст короче видимой области, сбрасываем смещение
        if (textWidth <= visibleWidth) {
            textOffsetX = 0;
            return;
        }

        // Рассчитываем желаемое textOffsetX, чтобы курсор оставался видимым
        // Фактическая позиция курсора на экране, относительно начала области
        // содержимого Input (после отступов), это cursorVisualX + textOffsetX.
        // Мы хотим:
        // 1. cursorVisualX + textOffsetX >= 0 (курсор не выходит за левый край
        // видимой области)
        // 2. cursorVisualX + textOffsetX <= visibleWidth (курсор не выходит за
        // правый край видимой области)

        // Если курсор выходит за правый край
        if (cursorVisualX + textOffsetX > visibleWidth) {
            textOffsetX = visibleWidth - cursorVisualX;
        }
        // Если курсор выходит за левый край
        else if (cursorVisualX + textOffsetX < 0) {
            textOffsetX = -cursorVisualX;
        }

        // Убедимся, что textOffsetX не приводит к тому, что текст уходит
        // слишком далеко влево (т.е. чтобы конец текста был виден)
        // Крайняя правая точка текста это textWidth + textOffsetX
        // Она не должна быть меньше visibleWidth
        if (textWidth + textOffsetX < visibleWidth) {
            textOffsetX = visibleWidth - textWidth;
        }
        // Убедимся, что textOffsetX не приводит к тому, что текст уходит
        // слишком далеко вправо (т.е. чтобы начало текста было видно, если есть
        // место)
        if (textOffsetX > 0) {
            textOffsetX = 0;
        }
    }

} // namespace Squidl::Elements
