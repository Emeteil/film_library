#pragma once

/// @file DataManager.h
/// @brief Центральный менеджер данных - единая точка входа для CRUD-операций,
///        поиска и сортировки фильмов.
///
/// Владеет вектором std::unique_ptr<Movie>, управляет IndexManager и SortCache.
/// Это «ядро» бэкенда приложения.

#include "core/index/IndexManager.h"
#include "core/index/SortCache.h"
#include "core/models/Movie.h"

#include <memory>
#include <string>
#include <vector>

namespace FilmLibrary
{
    class DataManager
    {
        public:
            DataManager();
            ~DataManager();

            DataManager(const DataManager&) = delete;
            DataManager& operator=(const DataManager&) = delete;

            /// @brief Загрузить данные из CSV-файла.
            /// @param filePath  Путь к CSV-файлу.
            /// @return Список ошибок парсинга (пустой при полном успехе).
            std::vector<std::string> LoadFromCsv(const std::string& filePath);

            /// @brief Сохранить данные в CSV-файл.
            /// @param filePath  Путь к CSV-файлу.
            /// @return true при успехе.
            bool SaveToCsv(const std::string& filePath) const;

            /// @brief Добавить новый фильм. ID назначается автоматически.
            /// @return ID добавленного фильма.
            int AddMovie(Movie movieData);

            /// @brief Обновить данные существующего фильма по ID.
            /// @return true, если фильм с указанным ID найден и обновлён.
            bool UpdateMovie(int id, const Movie& updatedData);

            /// @brief Удалить фильм по ID.
            /// @return true, если фильм был найден и удалён.
            bool DeleteMovie(int id);

            /// @brief Получить фильм по ID (только для чтения).
            /// @return Указатель на фильм или nullptr, если не найден.
            const Movie* GetMovieById(int id) const;

            /// @brief Получить все фильмы (только для чтения).
            const std::vector<std::unique_ptr<Movie>>& GetAllMovies() const;

            /// @brief Общее количество фильмов.
            std::size_t GetMovieCount() const;

            std::vector<Movie*> FindByTitlePrefix(const std::string& prefix) const;
            std::vector<Movie*> FindByStudioPrefix(const std::string& prefix) const;
            std::vector<Movie*> FindByYear(int year) const;
            std::vector<Movie*> FilterByRatingRange(double low, double high) const;
            std::vector<Movie*> FilterByLengthRange(int low, int high) const;
            std::vector<Movie*> SearchByDescription(const std::string& pattern) const;

            /// @brief Получить фильмы, отсортированные по рейтингу.
            /// @param ascending  true - по возрастанию, false - по убыванию.
            const std::vector<Movie*>& GetSortedByRating(bool ascending = true);

            /// @brief Получить фильмы, отсортированные по году.
            const std::vector<Movie*>& GetSortedByYear(bool ascending = true);

            /// @brief Получить фильмы, отсортированные по названию.
            const std::vector<Movie*>& GetSortedByTitle(bool ascending = true);

        private:
            std::vector<std::unique_ptr<Movie>> movies;
            IndexManager indexManager;
            SortCache sortCache;
            int nextId = 1;

            /// @brief Перестроить все индексы и сбросить кэш сортировки.
            void OnDataChanged();
    };
}
