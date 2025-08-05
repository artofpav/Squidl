// include/Squidl/utils/Logger.h
#pragma once

#include "Squidl/SquidlConfig.h" // Для SQUIDL_API
#include <chrono>                // Для std::chrono
#include <iomanip>               // Для std::put_time
#include <iostream>
#include <sstream>
#include <string>

namespace Squidl::Utils {

    // Внутри пространства имен Squidl::Utils
    namespace ConsoleColors {
        const std::string RESET = "\033[0m";
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string YELLOW = "\033[33m";
        const std::string BLUE = "\033[34m";
        const std::string CYAN = "\033[36m";
        const std::string MAGENTA = "\033[35m";
        const std::string BRIGHT_RED = "\033[91m";
        const std::string BRIGHT_YELLOW = "\033[93m";
    } // namespace ConsoleColors

    /**
     * @brief Уровни логирования.
     * @ingroup Utils
     */
    enum class LogLevel { Debug, Info, Warning, Error, Fatal };

    // --- Предварительное объявление класса Logger ---
    // Правильный синтаксис предварительного объявления внутри пространства имен
    class Logger; // <--- ИСПРАВЛЕНО: УБРАНО "Squidl::Utils::"

    // --- Вспомогательный класс для добавления std::endl в конце ---
    class SQUIDL_API LogStreamProxy {
      public:
        // Объявляем конструктор и деструктор, но определяем их out-of-line
        LogStreamProxy(std::ostream &os, LogLevel level);
        ~LogStreamProxy();

        // Перегрузка оператора <<, чтобы можно было писать в прокси
        template <typename T> LogStreamProxy &operator<<(const T &value) {
            if (!m_os.fail()) { // Пишем только если поток не в состоянии ошибки
                m_os << value;
            }
            return *this;
        }

        // Перегрузка для манипуляторов потока, таких как std::flush
        LogStreamProxy &operator<<(std::ostream &(*manip)(std::ostream &)) {
            if (!m_os.fail()) {
                m_os << manip;
            }
            return *this;
        }

      private:
        std::ostream &m_os;
        LogLevel m_level;
    };

    /**
     * @brief Простая утилита для логирования сообщений в консоль.
     * @ingroup Utils
     */
    class SQUIDL_API Logger {
        // --- ОБЪЯВЛЯЕМ LogStreamProxy ДРУЖЕСТВЕННЫМ КЛАССОМ ---
        // Это дает LogStreamProxy доступ к приватным членам Logger, таким как
        // s_currentLogLevel
        friend class LogStreamProxy; // <--- ДОБАВЛЕНО

      public:
        // Устанавливает минимальный уровень для вывода сообщений
        static void setLogLevel(LogLevel level) { s_currentLogLevel = level; }

        static LogLevel getLogLevel() { return s_currentLogLevel; }

        // Методы для логирования сообщений разных уровней (теперь используют
        // Stream)
        static void Debug(const std::string &message) {
            log(LogLevel::Debug, message);
        }
        static void Info(const std::string &message) {
            log(LogLevel::Info, message);
        }
        static void Warning(const std::string &message) {
            log(LogLevel::Warning, message);
        }
        static void Error(const std::string &message) {
            log(LogLevel::Error, message);
        }
        static void Fatal(const std::string &message) {
            log(LogLevel::Fatal, message);
        }

        // Метод, возвращающий прокси-объект для потоковой записи
        static LogStreamProxy Stream(LogLevel level);

        // Эти функции должны быть доступны для LogStreamProxy и других
        // внутренних нужд
        static std::string getTimestamp();
        static std::string levelToString(LogLevel level);

      private:
        // s_currentLogLevel теперь доступен LogStreamProxy благодаря
        // friend-объявлению
        static LogLevel s_currentLogLevel;
        static std::ostringstream
            s_nullStream; // Поток для отфильтрованных сообщений

        // Внутренний метод log, который теперь также использует
        // Stream/LogStreamProxy
        static void log(LogLevel level, const std::string &message);
    };

    // --- Реализация статических членов Logger и методов LogStreamProxy
    // (ОПРЕДЕЛЕНИЯ ПОСЛЕ КЛАССА Logger) ---

    // Инициализация статических членов Logger (должны быть определены после
    // класса Logger)
    inline LogLevel Logger::s_currentLogLevel =
        LogLevel::Info; // По умолчанию Info
    inline std::ostringstream
        Logger::s_nullStream; // Инициализируем пустой поток

    // Реализация статических вспомогательных функций Logger
    inline std::string Logger::getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    inline std::string Logger::levelToString(LogLevel level) {
        switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Fatal:
            return "FATAL";
        default:
            return "UNKNOWN";
        }
    }

    // Реализация метода log класса Logger
    inline void Logger::log(LogLevel level, const std::string &message) {
        if (level >= s_currentLogLevel) {
            Logger::Stream(level) << message;
        }
    }

    // Реализация метода Stream класса Logger
    inline LogStreamProxy Logger::Stream(LogLevel level) {
        if (level < s_currentLogLevel) {
            s_nullStream.str("");
            return LogStreamProxy(s_nullStream, level);
        }
        return LogStreamProxy(std::cout, level);
    }

    // --- ОПРЕДЕЛЕНИЕ МЕТОДОВ LogStreamProxy ---
    // Эти методы должны быть определены ПОСЛЕ класса Logger, чтобы он был
    // известен

    // В LogStreamProxy::LogStreamProxy (конструктор)
    inline LogStreamProxy::LogStreamProxy(std::ostream &os, LogLevel level)
        : m_os(os), m_level(level) {
        if (m_level < Logger::s_currentLogLevel) {
            m_os.setstate(std::ios_base::badbit);
        } else {
            // Выбор цвета в зависимости от уровня
            std::string colorCode = ConsoleColors::RESET;
            switch (m_level) {
            case LogLevel::Debug:
                colorCode = ConsoleColors::CYAN;
                break;
            case LogLevel::Info:
                colorCode = ConsoleColors::GREEN;
                break;
            case LogLevel::Warning:
                colorCode = ConsoleColors::YELLOW;
                break;
            case LogLevel::Error:
                colorCode = ConsoleColors::RED;
                break;
            case LogLevel::Fatal:
                colorCode = ConsoleColors::BRIGHT_RED;
                break;
            default:
                break;
            }
            m_os << colorCode; // Устанавливаем цвет
            m_os << "[" << Logger::getTimestamp() << "] ["
                 << Logger::levelToString(m_level) << "] ";
        }
    }

    // В LogStreamProxy::~LogStreamProxy (деструктор)
    inline LogStreamProxy::~LogStreamProxy() {
        if (!m_os.fail()) {
            m_os << ConsoleColors::RESET
                 << std::endl; // Сбрасываем цвет и добавляем перенос строки
        }
    }

} // namespace Squidl::Utils

// Макросы для удобного логирования (их не нужно менять!)
#define SQUIDL_LOG_DEBUG                                                       \
    Squidl::Utils::Logger::Stream(Squidl::Utils::LogLevel::Debug)
#define SQUIDL_LOG_INFO                                                        \
    Squidl::Utils::Logger::Stream(Squidl::Utils::LogLevel::Info)
#define SQUIDL_LOG_WARNING                                                     \
    Squidl::Utils::Logger::Stream(Squidl::Utils::LogLevel::Warning)
#define SQUIDL_LOG_ERROR                                                       \
    Squidl::Utils::Logger::Stream(Squidl::Utils::LogLevel::Error)
#define SQUIDL_LOG_FATAL                                                       \
    Squidl::Utils::Logger::Stream(Squidl::Utils::LogLevel::Fatal)