#pragma once

/// @file Movie.h
/// @brief Модель данных фильма.

#include <string>

namespace FilmLibrary
{
    /// @brief Структура, представляющая запись о фильме в каталоге.
    struct Movie
    {
        int id = 0;                 ///< Служебный ID (автоинкремент, не сохраняется в CSV).
        std::string title;          ///< Название фильма.
        std::string studio;         ///< Студия-производитель.
        std::string description;    ///< Описание / синопсис.
        int year = 0;               ///< Год выпуска.
        int length = 0;             ///< Длительность в секундах.
        double rating = 0.0;        ///< Рейтинг (0.0 – 10.0).
        std::string cover;          ///< Путь к обложке (опционально).
        std::string streamLink;     ///< Ссылка для просмотра (опционально).
    };
}
