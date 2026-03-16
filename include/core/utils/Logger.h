#pragma once

/// @file Logger.h
/// @brief Простой логгер для диагностических сообщений.
///
/// Пишет в файл и/или stdout. Поддерживает уровни: INFO, WARNING, ERROR.

#include <string>

namespace FilmLibrary
{
    enum class LogLevel
    {
        Info,
        Warning,
        Error
    };

    class Logger
    {
        public:
            /// @brief Получить единственный экземпляр логгера, синглтон.
            static Logger& Instance();

            /// @brief Инициализировать логгирование в файл.
            /// @param filePath Путь к лог-файлу. Если пустая строка - только stdout.
            void Init(const std::string& filePath = "");

            /// @brief Записать сообщение.
            void Log(LogLevel level, const std::string& message);

            void Info(const std::string& message);
            void Warning(const std::string& message);
            void Error(const std::string& message);

            /// @brief Установить минимальный уровень (сообщения ниже будут игнорироваться).
            void SetMinLevel(LogLevel level);

        private:
            Logger() = default;
            ~Logger();

            Logger(const Logger&) = delete;
            Logger& operator=(const Logger&) = delete;

            LogLevel minLevel = LogLevel::Info;
            std::string logFilePath;

            /// @brief Преобразовать уровень в строку.
            static const char* LevelToString(LogLevel level);
    };
}
