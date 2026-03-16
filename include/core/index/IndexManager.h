#pragma once

/// @file IndexManager.h
/// @brief Менеджер индексов - управляет АВЛ-деревьями и отсортированными
///        векторами для быстрого поиска и фильтрации.
///
/// При любом изменении данных (CRUD) индексы перестраиваются через Rebuild().

#include "core/algorithms/AvlTree.h"
#include "core/models/Movie.h"

#include <memory>
#include <vector>

namespace FilmLibrary
{
    class IndexManager
    {
        public:
            IndexManager();
            ~IndexManager();

            IndexManager(const IndexManager&) = delete;
            IndexManager& operator=(const IndexManager&) = delete;

            /// @brief Полная перестройка всех индексов по текущему вектору фильмов.
            /// @param movies  Ссылка на основное хранилище.
            void Rebuild(const std::vector<std::unique_ptr<Movie>>& movies);

            /// @brief Найти фильмы, название которых начинается с prefix.
            std::vector<Movie*> FindByTitlePrefix(const std::string& prefix) const;

            /// @brief Найти фильмы студии, название которой начинается с prefix.
            std::vector<Movie*> FindByStudioPrefix(const std::string& prefix) const;

            /// @brief Найти фильмы по году выпуска.
            std::vector<Movie*> FindByYear(int year) const;

            /// @brief Получить фильмы с рейтингом в диапазоне [low, high].
            std::vector<Movie*> FilterByRatingRange(double low, double high) const;

            /// @brief Получить фильмы с длительностью в диапазоне [low, high] (в секундах).
            std::vector<Movie*> FilterByLengthRange(int low, int high) const;

            /// @brief Найти фильмы, описание которых содержит подстроку pattern.
            std::vector<Movie*> SearchByDescription(
                const std::vector<std::unique_ptr<Movie>>& movies,
                const std::string& pattern) const;

        private:
            AvlTree<Movie*, std::string> titleIndex;
            AvlTree<Movie*, std::string> studioIndex;
            AvlTree<Movie*, int> yearIndex;

            std::vector<Movie*> sortedByRating;
            std::vector<Movie*> sortedByLength;

            /// @brief Пересортировать вспомогательные векторы.
            void RebuildSortedVectors(const std::vector<std::unique_ptr<Movie>>& movies);
    };
}
