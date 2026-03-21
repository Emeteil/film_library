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

    Logger::~Logger()
    {
        if (logStream.is_open())
            logStream.close();
    }

    void Logger::Init(const std::string& filePath)
    {
        if (logStream.is_open())
            logStream.close();

        logFilePath = filePath;
        if (!logFilePath.empty())
            logStream.open(logFilePath, std::ios::app);
    }

    void Logger::Log(LogLevel level, const std::string& message)
    {
        if (static_cast<int>(level) < static_cast<int>(minLevel))
            return;

        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);

        char timestamp[22];
        std::strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", localTime);

        std::string formattedMessage = std::string(timestamp) + " [" + LevelToString(level) + "] " + message;
        std::cout << formattedMessage << std::endl;

        if (logStream.is_open())
            logStream << formattedMessage << std::endl;
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
