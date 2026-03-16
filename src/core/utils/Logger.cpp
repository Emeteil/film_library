/// @file Logger.cpp
/// @brief Реализация простого логгера.

#include "core/utils/Logger.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>

namespace FilmLibrary
{
    Logger& Logger::Instance()
    {
        static Logger instance;
        return instance;
    }

    Logger::~Logger() = default;

    void Logger::Init(const std::string& filePath)
    {
        logFilePath = filePath;
    }

    void Logger::Log(LogLevel level, const std::string& message)
    {
        if (level < minLevel)
            return;

        // TODO: Реализовать форматирование и вывод.
        //
        // Формат: [YYYY-MM-DD HH:MM:SS] [LEVEL] message
        //
        // 1. Сформировать timestamp.
        // 2. Вывести в stdout.
        // 3. Если logFilePath не пуст - дописать в файл (std::ofstream, append).

        (void)message;
    }

    void Logger::Info(const std::string& message)
    {
        Log(LogLevel::Info, message);
    }

    void Logger::Warning(const std::string& message)
    {
        Log(LogLevel::Warning, message);
    }

    void Logger::Error(const std::string& message)
    {
        Log(LogLevel::Error, message);
    }

    void Logger::SetMinLevel(LogLevel level)
    {
        minLevel = level;
    }

    const char* Logger::LevelToString(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Info:
                return "INFO";
            case LogLevel::Warning:
                return "WARNING";
            case LogLevel::Error:
                return "ERROR";
            default:
                return "UNKNOWN";
        }
    }
}
