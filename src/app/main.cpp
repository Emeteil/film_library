/// @file main.cpp
/// @brief Точка входа в приложение «Фильмотека».

#include "gui/AppWindow.h"
#include "gui/AppController.h"
#include "gui/widgets/NotificationManager.h"
#include "core/utils/Logger.h"

#include <iostream>
#include <string>

/// @brief Путь к CSV-файлу по умолчанию
static const std::string DEFAULT_CSV_PATH = "data/movies.csv";

int main(int argc, char* argv[])
{
    std::string csvPath = DEFAULT_CSV_PATH;
    
    if (argc > 1)
        csvPath = argv[1];

    FilmLibrary::Logger::Instance().Init("film_library.log");
    FilmLibrary::Logger::Instance().Info("Запуск приложения \"Фильмотека\"");

    FilmLibrary::AppWindow appWindow("Фильмотека", 1280, 720);
    if (!appWindow.Init())
    {
        FilmLibrary::Logger::Instance().Error("Не удалось инициализировать окно приложения.");
        return 1;
    }

    FilmLibrary::AppController controller(csvPath);
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

    if (!controller.Shutdown())
    {
        FilmLibrary::Logger::Instance().Error("Ошибка сохранения CSV при выходе.");

        // TODO: Показать ошибку пользователю (if окно уже закрыто - вывести в stderr).

        std::cerr << "ОШИБКА: Не удалось сохранить данные." << std::endl;
    }

    appWindow.Shutdown();
    FilmLibrary::Logger::Instance().Info("Приложение завершено.");

    return 0;
}
