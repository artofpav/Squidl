// include/Squidl/core/UIManager.h
#pragma once

#include "Squidl/SquidlConfig.h"   // For SQUIDL_API
#include "Squidl/base/UIElement.h" // For std::shared_ptr<UIElement>
#include "Squidl/core/EventDispatcher.h"
#include "Squidl/core/IRenderer.h"
#include "Squidl/core/UIContext.h"
#include <SDL.h>  // For SDL_Event, SDL_Renderer
#include <memory> // For std::unique_ptr, std::shared_ptr

namespace Squidl::Core {

    /**
     * @brief Менеджер UI системы.
     * @ingroup Core
     * Отвечает за инициализацию, обработку событий, обновление и отрисовку UI.
     */
    class SQUIDL_API UIManager {
      public:
        UIManager();
        ~UIManager();

        /**
         * @brief Инициализирует менеджер UI.
         * @param window Указатель на окно SDL.
         * @param renderer Указатель на рендерер SDL.
         * @param initialRootElement Корневой элемент UI для управления.
         */
        void init(SDL_Window *window, SDL_Renderer *renderer,
                  std::shared_ptr<Squidl::Base::UIElement> initialRootElement);

        /**
         * @brief Обрабатывает событие SDL, обновляя UIContext и рассылая
         * UIEvent.
         * @param sdlEvent Событие SDL.
         */
        void handleSDLEvent(const SDL_Event &sdlEvent);

        /**
         * @brief Обновляет состояние UI и отрисовывает его.
         */
        void updateAndRender();

        /**
         * @brief Добавляет элемент UI в список слушателей событий.
         * @param element Элемент для добавления.
         */
        void addUIElement(std::shared_ptr<Squidl::Base::UIElement> element);

        /**
         * @brief Удаляет элемент UI из списка слушателей событий.
         * @param element Элемент для удаления.
         */
        void removeUIElement(std::shared_ptr<Squidl::Base::UIElement> element);

        // Getters
        UIContext &getUIContext() { return m_context; }
        EventDispatcher &getEventDispatcher() { return m_eventDispatcher; }
        SDL_Renderer *getSDLRenderer() const {
            return m_sdlRenderer;
        } // For texture loading in Backdrop etc.

      private:
        UIContext m_context;
        EventDispatcher m_eventDispatcher;
        std::shared_ptr<Squidl::Base::UIElement> m_rootElement;
        std::unique_ptr<IRenderer> m_uiRenderer; // Наш абстрактный рендерер
        SDL_Window *m_sdlWindow = nullptr;
        SDL_Renderer *m_sdlRenderer =
            nullptr; // Сырой указатель на рендерер SDL
    };

} // namespace Squidl::Core
