/// @file AppController.cpp
/// @brief Реализация контроллера приложения.

#include "gui/AppController.h"
#include "core/utils/Logger.h"

namespace FilmLibrary
{
    AppController::AppController(const std::string& csvFilePath) : csvFilePath(csvFilePath)
    {
    }

    AppController::~AppController() = default;

    std::vector<std::string> AppController::Initialize()
    {
        // TODO: Загрузить данные из CSV через dataManager.
        return dataManager.LoadFromCsv(csvFilePath);
    }

    bool AppController::Shutdown()
    {
        // TODO: Сохранить данные в CSV.
        return dataManager.SaveToCsv(csvFilePath);
    }

    int AppController::AddMovie(Movie movieData)
    {
        int id = dataManager.AddMovie(std::move(movieData));
        displayDirty = true;
        return id;
    }

    bool AppController::UpdateMovie(int id, const Movie& updatedData)
    {
        bool result = dataManager.UpdateMovie(id, updatedData);
        if (result)
        {
            displayDirty = true;
        }
        return result;
    }

    bool AppController::DeleteMovie(int id)
    {
        bool result = dataManager.DeleteMovie(id);
        if (result)
        {
            displayDirty = true;
        }
        return result;
    }

    const Movie* AppController::GetMovieById(int id) const
    {
        return dataManager.GetMovieById(id);
    }

    const std::vector<Movie*>& AppController::GetDisplayMovies()
    {
        if (displayDirty)
        {
            RefreshDisplayList();
            displayDirty = false;
        }
        return displayMovies;
    }

    std::size_t AppController::GetMovieCount() const
    {
        return dataManager.GetMovieCount();
    }

    void AppController::PerformSearch(SearchMode mode, const std::string& query)
    {
        currentSearchMode = mode;
        currentSearchQuery = query;
        displayDirty = true;
    }

    void AppController::ClearSearch()
    {
        currentSearchMode = SearchMode::None;
        currentSearchQuery.clear();
        displayDirty = true;
    }

    void AppController::FilterByRatingRange(double low, double high)
    {
        // TODO: Сохранить параметры фильтра, пометить displayDirty.
        (void)low;
        (void)high;
        displayDirty = true;
    }

    void AppController::FilterByLengthRange(int low, int high)
    {
        // TODO: Сохранить параметры фильтра, пометить displayDirty.
        (void)low;
        (void)high;
        displayDirty = true;
    }

    void AppController::ClearFilter()
    {
        displayDirty = true;
    }

    void AppController::SetSort(SortKey key, bool ascending)
    {
        currentSortKey = key;
        currentSortAscending = ascending;
        displayDirty = true;
    }

    void AppController::ClearSort()
    {
        currentSortKey = SortKey::None;
        displayDirty = true;
    }

    void AppController::RefreshDisplayList()
    {
        // TODO: Реализовать сборку displayMovies.
        //
        // 1. Если активен поиск - получить результаты через dataManager.
        // 2. Если активен фильтр - пересечь с результатами фильтрации.
        // 3. Если активна сортировка - использовать отсортированный вектор.
        // 4. Иначе - показать все фильмы.
        //
        // displayMovies = ...;
    }
}
