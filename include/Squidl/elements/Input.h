#pragma once
#include "Squidl/base/UIElement.h"
#include "Squidl/core/IRenderer.h"
#include "Squidl/core/UIContext.h"
#include "Squidl/elements/Label.h" // Убедитесь, что Label.h включен
#include "Squidl/utils/Color.h"
#include "Squidl/utils/UIRect.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <functional> // Для std::function
#include <string>

namespace Squidl::Elements {
    class SQUIDL_API Input : public Squidl::Base::UIElement {
      public:
        Input(std::string placeholderText, int x, int y, int w, int h,
              TTF_Font *font = nullptr);

        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;

        void autosize() override;

        void setText(const std::string &text);
        std::string getText() const;

        void setPlaceholderText(const std::string &text);

        // Переопределение метода onEvent для обработки событий
        void onEvent(Squidl::Core::UIEvent &event) override;

        // Callback для изменения текста
        std::function<void(const std::string &)> onTextChange;

      private:
        std::shared_ptr<Squidl::Elements::Label>
            label; // Используется для отображения текста
        std::string currentText;
        std::string placeholderText;
        TTF_Font *font = nullptr; // Шрифт для рендеринга текста

        bool focused = false;    // Находится ли поле ввода в фокусе
        int cursorPosition = 0;  // Позиция текстового курсора
        Uint32 cursorTimer = 0;  // Таймер для мигания курсора
        bool showCursor = false; // Флаг для отображения курсора
        int textOffsetX = 0; // Смещение текста для прокрутки внутри поля ввода

        // Цвета для различных состояний
        Squidl::Utils::Color focusedBorderColor = {
            50, 150, 255, 255}; // Синяя рамка при фокусе
        Squidl::Utils::Color textColor = {255, 255, 255, 225}; // Черный текст
        Squidl::Utils::Color placeholderColor = {
            255, 255, 255, 150}; // Серый текст-заполнитель
        Squidl::Utils::Color cursorColor = {255, 255, 255, 225}; // Цвет курсора

        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;

        // Вспомогательные функции
        int getCharIndexAt(
            int mouseX); // Получить индекс символа по координате X мыши
        void updateLabelTextAndColor(); // Обновить текст и цвет Label в
                                        // зависимости от состояния
        void adjustTextOffset();        // Корректировка смещения текста
    };
} // namespace Squidl::Elements
