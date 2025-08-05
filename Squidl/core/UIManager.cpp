// squidl/core/UIManager.cpp
#include "Squidl/core/UIManager.h"
#include "Squidl/base/UIElement.h"         // For UIElement and derived types
#include "Squidl/core/UIEvent.h"           // For UIEvent and derived types
#include "Squidl/layouts/Layout.h"         // For Layouts
#include "Squidl/renderers/SDL2Renderer.h" // For concrete SDL2Renderer
#include "Squidl/utils/Color.h"            // For Squidl::Utils::Color
#include "Squidl/utils/Logger.h"           // For logging

namespace Squidl::Core {

    UIManager::UIManager() : m_sdlWindow(nullptr), m_sdlRenderer(nullptr) {
        SQUIDL_LOG_DEBUG << "UIManager: Инициализирован.";
    }

    UIManager::~UIManager() {
        SQUIDL_LOG_DEBUG << "UIManager: Деинициализация.";
        // m_uiRenderer будет удален автоматически unique_ptr
        // m_rootElement будет удален автоматически shared_ptr
        // SDL_Renderer и SDL_Window не управляются UIManager, поэтому не
        // удаляются здесь.
    }

    void UIManager::init(
        SDL_Window *window, SDL_Renderer *renderer,
        std::shared_ptr<Squidl::Base::UIElement> initialRootElement) {
        if (!window || !renderer || !initialRootElement) {
            SQUIDL_LOG_ERROR
                << "UIManager: Ошибка инициализации: нулевые указатели.";
            return;
        }
        m_sdlWindow = window;
        m_sdlRenderer = renderer;
        m_rootElement = initialRootElement;
        m_uiRenderer =
            std::make_unique<Squidl::Renderers::SDL2Renderer>(m_sdlRenderer);

        int w, h;
        SDL_GetWindowSize(m_sdlWindow, &w, &h);
        m_context.setSize(w, h); // Устанавливаем начальный размер контекста

        // Добавляем корневой элемент в диспетчер событий
        // Это добавит его и всех его детей, если они вызывают addUIElement
        addUIElement(m_rootElement);
        SQUIDL_LOG_INFO << "UIManager: Инициализация завершена.";
    }

    void UIManager::handleSDLEvent(const SDL_Event &sdlEvent) {
        m_context.handleEvent(
            sdlEvent); // Обновляем внутреннее состояние UIContext

        // Создаем UIEvent и рассылаем его через EventDispatcher
        // Mouse Events
        if (sdlEvent.type == SDL_MOUSEMOTION) {
            MouseEvent event(MouseEventType::Moved, sdlEvent.motion.x,
                             sdlEvent.motion.y);
            m_eventDispatcher.dispatchEvent(event);
        } else if (sdlEvent.type == SDL_MOUSEBUTTONDOWN) {
            MouseEvent event(MouseEventType::ButtonPressed, sdlEvent.button.x,
                             sdlEvent.button.y, sdlEvent.button.button,
                             sdlEvent.button.clicks);
            m_eventDispatcher.dispatchEvent(event);
        } else if (sdlEvent.type == SDL_MOUSEBUTTONUP) {
            MouseEvent event(MouseEventType::ButtonReleased, sdlEvent.button.x,
                             sdlEvent.button.y, sdlEvent.button.button,
                             sdlEvent.button.clicks);
            m_eventDispatcher.dispatchEvent(event);
        } else if (sdlEvent.type == SDL_MOUSEWHEEL) {
            MouseEvent event(MouseEventType::Wheel, m_context.mouseX,
                             m_context.mouseY, 0, 0, sdlEvent.wheel.x,
                             sdlEvent.wheel.y);
            m_eventDispatcher.dispatchEvent(event);
        }
        // Keyboard Events
        else if (sdlEvent.type == SDL_KEYDOWN) {
            // Исправлено: Явное приведение sdlEvent.key.keysym.mod к SDL_Keymod
            KeyboardEvent event(
                sdlEvent.key.keysym.scancode,
                static_cast<SDL_Keymod>(sdlEvent.key.keysym.mod), true);
            m_eventDispatcher.dispatchEvent(event);
        } else if (sdlEvent.type == SDL_KEYUP) {
            // Исправлено: Явное приведение sdlEvent.key.keysym.mod к SDL_Keymod
            KeyboardEvent event(
                sdlEvent.key.keysym.scancode,
                static_cast<SDL_Keymod>(sdlEvent.key.keysym.mod), false);
            m_eventDispatcher.dispatchEvent(event);
        }
        // Text Input Event
        else if (sdlEvent.type == SDL_TEXTINPUT) {
            TextInputEvent event(sdlEvent.text.text);
            m_eventDispatcher.dispatchEvent(event);
        }
        // Window Resize Event (handled directly in main for now, but could be
        // an UIEvent) For now, we update context size here as well, in case
        // main loop doesn't
        if (sdlEvent.type == SDL_WINDOWEVENT &&
            sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
            int newW = sdlEvent.window.data1;
            int newH = sdlEvent.window.data2;
            m_context.setSize(newW, newH);
            
            // {{ edit_4 }} Dispatch ResizeEvent
            ResizeEvent event(newW, newH);
            m_eventDispatcher.dispatchEvent(event);
        }
    }

    void UIManager::updateAndRender() {
        m_context.beginFrame(); // Сброс временных флагов в контексте

        // Очистка экрана
        m_uiRenderer->clearScreen(Squidl::Utils::Color(20, 20, 20, 255));

        // Обновление и отрисовка корневого элемента и всех его детей
        if (m_rootElement) {
            m_rootElement->update(m_context, *m_uiRenderer);
        }
    }

    void
    UIManager::addUIElement(std::shared_ptr<Squidl::Base::UIElement> element) {
        if (element) {
            m_eventDispatcher.addListener(element);
            // Рекурсивно добавляем детей, если элемент является Layout
            if (auto layout =
                    std::dynamic_pointer_cast<Squidl::Layouts::Layout>(
                        element)) {
                for (const auto &child :
                     layout->getChildren()) { // Предполагаем, что Layout имеет
                                              // getChildren()
                    addUIElement(child);
                }
            }
        }
    }

    void UIManager::removeUIElement(
        std::shared_ptr<Squidl::Base::UIElement> element) {
        if (element) {
            m_eventDispatcher.removeListener(element);
            // Рекурсивно удаляем детей, если элемент является Layout
            if (auto layout =
                    std::dynamic_pointer_cast<Squidl::Layouts::Layout>(
                        element)) {
                for (const auto &child : layout->getChildren()) {
                    removeUIElement(child);
                }
            }
        }
    }

} // namespace Squidl::Core
