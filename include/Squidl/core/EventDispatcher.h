// include/Squidl/core/EventDispatcher.h
#pragma once

#include "Squidl/SquidlConfig.h"   // For SQUIDL_API
#include "Squidl/base/UIElement.h" // For UIElement (forward declaration)
#include "Squidl/core/UIEvent.h"   // For UIEvent and derived types
#include <functional>              // For std::function
#include <map>                     // For std::map
#include <memory>                  // For std::weak_ptr, std::shared_ptr
#include <vector>

namespace Squidl::Base {
    class UIElement; // Forward declaration
}

namespace Squidl::Core {

    /**
     * @brief Диспетчер событий UI.
     * @ingroup Core
     * Отвечает за рассылку событий зарегистрированным слушателям.
     */
    class SQUIDL_API EventDispatcher {
      public:
        EventDispatcher();

        /**
         * @brief Добавляет UIElement в качестве слушателя для всех событий.
         * @param element Указатель на UIElement, который будет получать
         * события.
         * @note Элемент должен быть жив, пока он зарегистрирован. Используется
         * weak_ptr для предотвращения циклических зависимостей и обработки
         * удаленных элементов.
         */
        void addListener(std::shared_ptr<Squidl::Base::UIElement> element);

        /**
         * @brief Удаляет UIElement из списка слушателей.
         * @param element Указатель на UIElement, который нужно удалить.
         */
        void removeListener(std::shared_ptr<Squidl::Base::UIElement> element);

        /**
         * @brief Рассылает событие всем зарегистрированным слушателям.
         * @param event Событие для рассылки.
         */
        void dispatchEvent(UIEvent &event); // Принимаем по ссылке, чтобы
                                            // handled мог быть изменен

      private:
        // Используем vector для простоты, можно оптимизировать с map<EventType,
        // vector<...>> если элементов и типов событий будет очень много.
        std::vector<std::weak_ptr<Squidl::Base::UIElement>> listeners;

        // Вспомогательная функция для очистки "мертвых" weak_ptr
        void cleanupListeners();
    };

} // namespace Squidl::Core
