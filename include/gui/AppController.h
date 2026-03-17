#pragma once

/// @file AppController.h
/// @brief Контроллер приложения - связующее звено между GUI и менеджерами данных.
///
/// Обрабатывает пользовательские действия (коллбэки GUI), делегирует
/// операции DataManager / ActorManager и возвращает данные для отображения.

#include "core/DataManager.h"
#include "core/ActorManager.h"

#include <string>
#include <vector>

namespace FilmLibrary
{
    class AppController
    {
        public:
            /// @param moviesCsvPath  Путь к CSV-файлу фильмов.
            /// @param actorsCsvPath  Путь к CSV-файлу актёров.
            AppController(const std::string& moviesCsvPath, const std::string& actorsCsvPath);
            ~AppController();

            AppController(const AppController&) = delete;
            AppController& operator=(const AppController&) = delete;

            /// @brief Загрузить данные при старте приложения.
            /// @return Список ошибок парсинга (может быть пустым).
            std::vector<std::string> Initialize();

            /// @brief Завершить работу контроллера (ресурсы освобождаются).
            void Shutdown();

            int AddMovie(Movie movieData);
            bool UpdateMovie(int id, const Movie& updatedData);
            bool DeleteMovie(int id);
            const Movie* GetMovieById(int id) const;

            /// @brief Получить текущий список фильмов для отображения в таблице.
            const std::vector<Movie*>& GetDisplayMovies();

            std::size_t GetMovieCount() const;

            /// @brief Получить актёра по ID.
            const Actor* GetActorById(int id) const;

            /// @brief Получить всех актёров.
            const std::vector<std::unique_ptr<Actor>>& GetAllActors() const;

            /// @brief Режим поиска.
            enum class SearchMode
            {
                ByTitle,
                ByStudio,
                ByYear,
                ByDescription,
                ByGenre,
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
            ActorManager actorManager;
            std::string moviesCsvPath;
            std::string actorsCsvPath;

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
