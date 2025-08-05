// include/Squidl/utils/Timer.h
#pragma once

#include "Squidl/SquidlConfig.h" // Для SQUIDL_API
#include <SDL.h>                 // Для SDL_GetTicks()

namespace Squidl::Utils {
    /**
     * @brief Простая утилита для измерения времени.
     * @ingroup Utils
     */
    class SQUIDL_API Timer {
      public:
        Timer();

        /**
         * @brief Запускает таймер. Если таймер уже запущен, перезапускает его.
         */
        void start();

        /**
         * @brief Останавливает таймер.
         */
        void stop();

        /**
         * @brief Возобновляет таймер после паузы.
         */
        void resume();

        /**
         * @brief Приостанавливает таймер.
         */
        void pause();

        /**
         * @brief Возвращает время в миллисекундах с момента запуска таймера
         * (или возобновления, если был на паузе).
         * @return Количество миллисекунд.
         */
        Uint32 getTicks() const;

        /**
         * @brief Проверяет, запущен ли таймер.
         * @return true, если таймер запущен; false в противном случае.
         */
        bool isStarted() const { return m_started; }

        /**
         * @brief Проверяет, находится ли таймер на паузе.
         * @return true, если таймер на паузе; false в противном случае.
         */
        bool isPaused() const { return m_paused; }

      private:
        Uint32 m_startTicks;  // Время, когда таймер был запущен
        Uint32 m_pausedTicks; // Время, когда таймер был поставлен на паузу

        bool m_started; // Флаг, указывающий, запущен ли таймер
        bool m_paused;  // Флаг, указывающий, находится ли таймер на паузе
    };
} // namespace Squidl::Utils

// Реализация методов Timer (можно было бы и в .cpp, но для простоты здесь)
// В более крупном проекте, возможно, лучше вынести в Squidl/utils/Timer.cpp
namespace Squidl::Utils {
    inline Timer::Timer()
        : m_startTicks(0), m_pausedTicks(0), m_started(false), m_paused(false) {
    }

    inline void Timer::start() {
        m_started = true;
        m_paused = false;
        m_startTicks = SDL_GetTicks();
        m_pausedTicks = 0;
    }

    inline void Timer::stop() {
        m_started = false;
        m_paused = false;
        m_startTicks = 0;
        m_pausedTicks = 0;
    }

    inline void Timer::resume() {
        if (m_started && m_paused) {
            m_paused = false;
            // Смещение начального времени, чтобы учесть время паузы
            m_startTicks = SDL_GetTicks() - m_pausedTicks;
            m_pausedTicks = 0;
        }
    }

    inline void Timer::pause() {
        if (m_started && !m_paused) {
            m_paused = true;
            m_pausedTicks = SDL_GetTicks() - m_startTicks;
        }
    }

    inline Uint32 Timer::getTicks() const {
        if (m_started) {
            if (m_paused) {
                return m_pausedTicks;
            } else {
                return SDL_GetTicks() - m_startTicks;
            }
        }
        return 0;
    }
} // namespace Squidl::Utils