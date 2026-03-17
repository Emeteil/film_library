#pragma once

/// @file Movie.h
/// @brief Модель данных фильма.

#include <string>
#include <vector>

namespace FilmLibrary
{
    /// @brief Структура, представляющая запись о фильме в каталоге.
    struct Movie
    {
        int id = 0;                         ///< Уникальный ID (автоинкремент, сохраняется в CSV).
        std::string title;                  ///< Название фильма.
        std::string studio;                 ///< Студия-производитель.
        std::string description;            ///< Описание / синопсис.
        int year = 0;                       ///< Год выпуска.
        int length = 0;                     ///< Длительность в секундах.
        double rating = 0.0;                ///< Рейтинг (0.0 – 10.0).
        std::string cover;                  ///< Путь к обложке (опционально).
        std::string streamLink;             ///< Ссылка для просмотра (опционально).
        std::vector<std::string> genres;    ///< Список жанров.
        std::vector<int> actorIds;          ///< ID актёров из таблицы Actor.
    };
}