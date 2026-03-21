/// @file main.cpp
/// @brief Точка входа в приложение "Фильмотека".

#include "gui/AppWindow.h"
#include "gui/AppController.h"
#include "gui/widgets/NotificationManager.h"
#include "core/utils/Logger.h"

#include <iostream>
#include <string>

/// @brief Путь к CSV-файлу фильмов по умолчанию
static const std::string DEFAULT_MOVIES_CSV_PATH = "data/movies.csv";

/// @brief Путь к CSV-файлу актёров по умолчанию
static const std::string DEFAULT_ACTORS_CSV_PATH = "data/actors.csv";

int main(int argc, char* argv[])
{
    std::string moviesCsvPath = DEFAULT_MOVIES_CSV_PATH;
    std::string actorsCsvPath = DEFAULT_ACTORS_CSV_PATH;

    if (argc > 1)
        moviesCsvPath = argv[1];
    if (argc > 2)
        actorsCsvPath = argv[2];

    FilmLibrary::Logger::Instance().Init("logs/film_library.log");
    FilmLibrary::Logger::Instance().Info("Запуск приложения \"Фильмотека\"");

    FilmLibrary::AppWindow appWindow("Фильмотека", 1280, 720);
    if (!appWindow.Init())
    {
        FilmLibrary::Logger::Instance().Error("Не удалось инициализировать окно приложения.");
        return 1;
    }

    FilmLibrary::AppController controller(moviesCsvPath, actorsCsvPath);
    auto errors = controller.Initialize();

    if (!errors.empty())
    {
        FilmLibrary::NotificationManager::Instance().Push(
            FilmLibrary::NotificationType::Warning,
            "Обнаружены ошибки при чтении CSV. Проверьте лог.",
            5.0f
        );

        for (const auto& err : errors)
        {
            FilmLibrary::Logger::Instance().Warning(err);
        }
    }

    FilmLibrary::Logger::Instance().Info("Загружено фильмов: " + std::to_string(controller.GetMovieCount()));

    appWindow.Run(controller);

    controller.Shutdown();
    appWindow.Shutdown();
    FilmLibrary::Logger::Instance().Info("Приложение завершено.");

    return 0;
}
