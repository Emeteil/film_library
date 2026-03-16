#pragma once

/// @file SortCache.h
/// @brief Кэш отсортированных массивов указателей на фильмы.
///
/// Ленивая инициализация: сортировка выполняется только при первом
/// обращении по конкретному ключу. Кэш сбрасывается при изменении данных.

#include "core/models/Movie.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace FilmLibrary
{
    class SortCache
    {
        public:
            /// @brief Тип компаратора для сортировки Movie*.
            using MovieComparator = std::function<bool(const Movie*, const Movie*)>;

            SortCache();
            ~SortCache();

            /// @brief Получить отсортированный вектор для заданного ключа.
            ///
            /// Если кэш для данного ключа пуст - сортирует и кэширует.
            ///
            /// @param key      Строковый идентификатор ключа сортировки (например "rating", "year").
            /// @param movies   Основное хранилище фильмов.
            /// @param comp     Компаратор «меньше» для Movie*.
            /// @return Ссылка на отсортированный вектор Movie*.
            const std::vector<Movie*>& GetSorted(const std::string& key, const std::vector<std::unique_ptr<Movie>>& movies, MovieComparator comp);

            /// @brief Инвалидировать (сбросить) весь кэш.
            ///
            /// Вызывается при любом изменении данных (добавление, удаление, редактирование).
            void Invalidate();

            /// @brief Инвалидировать кэш для конкретного ключа.
            void Invalidate(const std::string& key);

        private:
            std::unordered_map<std::string, std::vector<Movie*>> cache;
    };
}