#pragma once

/// @file AppController.h
/// @brief Контроллер приложения - связующее звено между GUI и DataManager.
///
/// Обрабатывает пользовательские действия (коллбэки GUI), делегирует
/// операции DataManager и возвращает данные для отображения.

#include "core/DataManager.h"

#include <string>
#include <vector>

namespace FilmLibrary
{
    class AppController
    {
        public:
            /// @param csvFilePath  Путь к CSV-файлу с данными.
            AppController(const std::string& csvFilePath);
            ~AppController();

            AppController(const AppController&) = delete;
            AppController& operator=(const AppController&) = delete;

            /// @brief Загрузить данные при старте приложения.
            /// @return Список ошибок парсинга (может быть пустым).
            std::vector<std::string> Initialize();

            /// @brief Сохранить данные при выходе.
            /// @return true при успехе.
            bool Shutdown();

            int AddMovie(Movie movieData);
            bool UpdateMovie(int id, const Movie& updatedData);
            bool DeleteMovie(int id);
            const Movie* GetMovieById(int id) const;

            /// @brief Получить текущий список фильмов для отображения в таблице.
            ///
            /// Возвращает вектор сырых указателей. Если активна сортировка или
            /// поиск - возвращает отфильтрованный/отсортированный набор.
            const std::vector<Movie*>& GetDisplayMovies();

            std::size_t GetMovieCount() const;

            /// @brief Режим поиска.
            enum class SearchMode
            {
                ByTitle,
                ByStudio,
                ByYear,
                ByDescription,
                None
            };

            /// @brief Выполнить поиск.
            void PerformSearch(SearchMode mode, const std::string& query);

            /// @brief Сбросить результаты поиска.
            void ClearSearch();

            void FilterByRatingRange(double low, double high);
            void FilterByLengthRange(int low, int high);
            void ClearFilter();

            /// @brief Ключ сортировки.
            enum class SortKey
            {
                Rating,
                Year,
                Title,
                None
            };

            void SetSort(SortKey key, bool ascending = true);
            void ClearSort();

        private:
            DataManager dataManager;
            std::string csvFilePath;

            std::vector<Movie*> displayMovies;
            bool displayDirty = true;

            SearchMode currentSearchMode = SearchMode::None;
            std::string currentSearchQuery;

            SortKey currentSortKey = SortKey::None;
            bool currentSortAscending = true;

            /// @brief Пересобрать displayMovies на основе текущих фильтров/сортировки.
            void RefreshDisplayList();
    };
}
