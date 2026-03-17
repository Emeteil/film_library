#pragma once

/// @file Actor.h
/// @brief Модель данных актёра.

#include <set>
#include <string>

namespace FilmLibrary
{
    /// @brief Структура, представляющая запись об актёре.
    struct Actor
    {
        int id = 0;                 ///< Уникальный ID (автоинкремент, сохраняется в CSV).
        std::string name;           ///< Имя (ФИО).
        std::string description;    ///< Описание.
        long int birthdate = 0;     ///< Дата рождения (UNIX timestamp).
        std::set<int> filmIds;      ///< Уникальные идентификаторы фильмов.
        std::string photo;          ///< Путь либо ссылка на фотографию.
    };
}