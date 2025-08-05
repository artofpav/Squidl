// squidl/core/EventDispatcher.cpp
#include "Squidl/core/EventDispatcher.h"
#include "Squidl/utils/Logger.h" // For logging

namespace Squidl::Core {

    EventDispatcher::EventDispatcher() {
        SQUIDL_LOG_DEBUG << "EventDispatcher: Initialized.";
    }

    void EventDispatcher::addListener(
        std::shared_ptr<Squidl::Base::UIElement> element) {
        if (!element) {
            SQUIDL_LOG_WARNING
                << "EventDispatcher: Trying to add undifined listener.";
            return;
        }
        // Проверяем, не добавлен ли уже этот элемент
        for (const auto &listener_weak_ptr : listeners) {
            if (auto listener_shared_ptr = listener_weak_ptr.lock()) {
                if (listener_shared_ptr == element) {
                    SQUIDL_LOG_DEBUG << "EventDispatcher: element already added as listener.";
                    return;
                }
            }
        }
        listeners.push_back(element);
        SQUIDL_LOG_DEBUG
            << "EventDispatcher: Element added as listener. Total number of listeners: "
            << listeners.size();
    }

    void EventDispatcher::removeListener(
        std::shared_ptr<Squidl::Base::UIElement> element) {
        if (!element) {
            SQUIDL_LOG_WARNING
                << "EventDispatcher: Trying to remove undefined listener.";
            return;
        }
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(),
                           [&](const std::weak_ptr<Squidl::Base::UIElement>
                                   &listener_weak_ptr) {
                               if (auto listener_shared_ptr =
                                       listener_weak_ptr.lock()) {
                                   return listener_shared_ptr == element;
                               }
                               return true; // Удаляем "мертвые" указатели сразу
                           }),
            listeners.end());
        SQUIDL_LOG_DEBUG
            << "EventDispatcher: LIstener removed. Total number of listeners: "
            << listeners.size();
    }

    void EventDispatcher::dispatchEvent(UIEvent &event) {
        cleanupListeners(); // Очищаем мертвые указатели перед рассылкой

        // Проходим по слушателям в обратном порядке, чтобы элементы,
        // которые находятся поверх других, могли обработать событие первыми.
        // Это важно для корректной обработки кликов (например, кнопка поверх
        // фона).
        for (auto it = listeners.rbegin(); it != listeners.rend(); ++it) {
            if (event.handled) { // Если событие уже обработано, прекращаем
                                 // рассылку
                break;
            }
            if (auto listener_shared_ptr = it->lock()) {
                listener_shared_ptr->onEvent(event);
            }
        }
    }

    void EventDispatcher::cleanupListeners() {
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(),
                           [](const std::weak_ptr<Squidl::Base::UIElement>
                                  &listener_weak_ptr) {
                               return listener_weak_ptr
                                   .expired(); // Удаляем просроченные weak_ptr
                           }),
            listeners.end());
    }

} // namespace Squidl::Core
