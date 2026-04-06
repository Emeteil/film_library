/// @file AppController.cpp
/// @brief Реализация контроллера приложения.

#include "gui/AppController.h"
#include "gui/widgets/NotificationManager.h"
#include "core/utils/Logger.h"
#include "core/algorithms/QuickSort.h"
#include "core/algorithms/RabinKarp.h"

#include <algorithm>
#include <string>
#include <vector>

namespace FilmLibrary
{
    AppController::AppController(const std::string& moviesCsvPath, const std::string& actorsCsvPath)
        : moviesCsvPath(moviesCsvPath)
        , actorsCsvPath(actorsCsvPath)
    {
    }

    AppController::~AppController() = default;

    std::vector<std::string> AppController::Initialize()
    {
        std::vector<std::string> errors;
        auto movieErrors = dataManager.LoadFromCsv(moviesCsvPath);
        auto actorErrors = actorManager.LoadFromCsv(actorsCsvPath);

        errors.insert(errors.end(), movieErrors.begin(), movieErrors.end());
        errors.insert(errors.end(), actorErrors.begin(), actorErrors.end());
        return errors;
    }

    void AppController::Shutdown()
    {
        Logger::Instance().Info("Контроллер завершает работу.");
    }

    int AppController::AddMovie(Movie movieData)
    {
        int id = dataManager.AddMovie(std::move(movieData));
        displayDirty = true;
        return id;
    }

    bool AppController::UpdateMovie(int id, const Movie& updatedData)
    {
        bool ok = dataManager.UpdateMovie(id, updatedData);
        if (ok) displayDirty = true;
        return ok;
    }

    bool AppController::DeleteMovie(int id)
    {
        for (auto& actor : actorManager.GetAllActors())
        {
            actor->filmIds.erase(id);
        }
        
        bool ok = dataManager.DeleteMovie(id);
        if (ok)
        {
            displayDirty = true;
            actorManager.SaveToCsv();
        }
        return ok;
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

    const std::vector<Actor*>& AppController::GetDisplayActors()
    {
        if (actorDisplayDirty)
        {
            RefreshActorDisplayList();
            actorDisplayDirty = false;
        }
        return displayActors;
    }

    std::size_t AppController::GetMovieCount() const
    {
        return dataManager.GetMovieCount();
    }

    const Actor* AppController::GetActorById(int id) const
    {
        return actorManager.GetActorById(id);
    }

    const std::vector<std::unique_ptr<Actor>>& AppController::GetAllActors() const
    {
        return actorManager.GetAllActors();
    }

    int AppController::AddActor(Actor data)
    {
        int id = actorManager.AddActor(std::move(data));
        actorDisplayDirty = true;
        return id;
    }

    bool AppController::UpdateActor(int id, const Actor& data)
    {
        bool ok = actorManager.UpdateActor(id, data);
        if (ok) actorDisplayDirty = true;
        return ok;
    }

    bool AppController::DeleteActor(int id)
    {
        const auto& allMovies = dataManager.GetAllMovies();
        bool moviesChanged = false;
        
        for (const auto& movie : allMovies)
        {
            auto it = std::find(movie->actorIds.begin(), movie->actorIds.end(), id);
            if (it != movie->actorIds.end())
            {
                movie->actorIds.erase(it);
                moviesChanged = true;
            }
        }
        
        bool ok = actorManager.DeleteActor(id);
        if (ok)
        {
            actorDisplayDirty = true;
            if (moviesChanged)
            {
                dataManager.SaveToCsv();
            }
        }
        return ok;
    }

    void AppController::PerformSearch(SearchMode mode, const std::string& query)
    {
        currentSearchMode = mode;
        currentSearchQuery = query;
        displayDirty = true;
    }

    void AppController::PerformActorSearch(const std::string& query)
    {
        currentActorSearchQuery = query;
        actorDisplayDirty = true;
    }

    void AppController::ClearSearch()
    {
        currentSearchMode = SearchMode::None;
        currentSearchQuery.clear();
        displayDirty = true;
    }

    void AppController::ClearActorSearch()
    {
        currentActorSearchQuery.clear();
        actorDisplayDirty = true;
    }

    void AppController::FilterByRatingRange(double low, double high)
    {
        isRatingFilterActive = true;
        ratingFilterLow = low;
        ratingFilterHigh = high;
        displayDirty = true;
    }

    void AppController::FilterByLengthRange(int low, int high)
    {
        isLengthFilterActive = true;
        lengthFilterLow = low;
        lengthFilterHigh = high;
        displayDirty = true;
    }

    void AppController::ClearFilter()
    {
        isRatingFilterActive = false;
        isLengthFilterActive = false;
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

    void AppController::SetActorSort(SortKey key, bool ascending)
    {
        currentActorSortKey = key;
        currentActorSortAscending = ascending;
        actorDisplayDirty = true;
    }

    void AppController::ClearActorSort()
    {
        currentActorSortKey = SortKey::None;
        actorDisplayDirty = true;
    }

    void AppController::RefreshDisplayList()
    {
        displayMovies.clear();
        
        std::vector<Movie*> currentList;
        bool listInitialized = false;

        if (currentSearchMode != SearchMode::None && !currentSearchQuery.empty())
        {
            if (currentSearchMode == SearchMode::ByTitle)
            {
                currentList = dataManager.FindByTitlePrefix(currentSearchQuery);
            }
            else if (currentSearchMode == SearchMode::ByStudio)
            {
                currentList = dataManager.FindByStudioPrefix(currentSearchQuery);
            }
            else if (currentSearchMode == SearchMode::ByYear)
            {
                try
                {
                    currentList = dataManager.FindByYear(std::stoi(currentSearchQuery));
                }
                catch(...) {}
            }
            else if (currentSearchMode == SearchMode::ByDescription)
            {
                currentList = dataManager.SearchByDescription(currentSearchQuery);
            }
            else if (currentSearchMode == SearchMode::ByGenre)
            {
                currentList = dataManager.FilterByGenre(currentSearchQuery);
            }
                
            listInitialized = true;
        }
        else if (isRatingFilterActive)
        {
            currentList = dataManager.FilterByRatingRange(ratingFilterLow, ratingFilterHigh);
            listInitialized = true;
        }
        else if (isLengthFilterActive)
        {
            currentList = dataManager.FilterByLengthRange(lengthFilterLow, lengthFilterHigh);
            listInitialized = true;
        }

        if (!listInitialized)
        {
            if (currentSortKey != SortKey::None && !isRatingFilterActive && !isLengthFilterActive)
            {
                if (currentSortKey == SortKey::Rating)
                    displayMovies = dataManager.GetSortedByRating(currentSortAscending);
                else if (currentSortKey == SortKey::Year)
                    displayMovies = dataManager.GetSortedByYear(currentSortAscending);
                else if (currentSortKey == SortKey::Title)
                    displayMovies = dataManager.GetSortedByTitle(currentSortAscending);
                else if (currentSortKey == SortKey::Length)
                    displayMovies = dataManager.GetSortedByLength(currentSortAscending);
                return;
            }
            
            for (const auto& m : dataManager.GetAllMovies())
            {
                currentList.push_back(m.get());
            }
        }

        for (Movie* m : currentList)
        {
            if (isRatingFilterActive)
            {
                if (m->rating < ratingFilterLow || m->rating > ratingFilterHigh)
                    continue;
            }
            
            if (isLengthFilterActive)
            {
                if (m->length < lengthFilterLow || m->length > lengthFilterHigh)
                    continue;
            }
            
            displayMovies.push_back(m);
        }

        if (currentSortKey != SortKey::None)
        {
            if (currentSortKey == SortKey::Rating)
            {
                QuickSort::Sort(displayMovies, [asc = currentSortAscending](const Movie* a, const Movie* b) {
                    return asc ? (a->rating < b->rating) : (a->rating > b->rating);
                });
            }
            else if (currentSortKey == SortKey::Year)
            {
                QuickSort::Sort(displayMovies, [asc = currentSortAscending](const Movie* a, const Movie* b) {
                    return asc ? (a->year < b->year) : (a->year > b->year);
                });
            }
            else if (currentSortKey == SortKey::Title)
            {
                QuickSort::Sort(displayMovies, [asc = currentSortAscending](const Movie* a, const Movie* b) {
                    return asc ? (a->title < b->title) : (a->title > b->title);
                });
            }
            else if (currentSortKey == SortKey::Length)
            {
                QuickSort::Sort(displayMovies, [asc = currentSortAscending](const Movie* a, const Movie* b) {
                    return asc ? (a->length < b->length) : (a->length > b->length);
                });
            }
        }
    }

    void AppController::RefreshActorDisplayList()
    {
        displayActors.clear();
        for (const auto& a : actorManager.GetAllActors())
        {
            if (!currentActorSearchQuery.empty())
            {
                if (!RabinKarp::Contains(a->name, currentActorSearchQuery))
                    continue;
            }
            displayActors.push_back(a.get());
        }

        if (currentActorSortKey != SortKey::None)
        {
            if (currentActorSortKey == SortKey::Name)
            {
                QuickSort::Sort(displayActors, [asc = currentActorSortAscending](const Actor* a, const Actor* b) {
                    return asc ? (a->name < b->name) : (a->name > b->name);
                });
            }
            else if (currentActorSortKey == SortKey::Rating)
            {
                QuickSort::Sort(displayActors, [asc = currentActorSortAscending](const Actor* a, const Actor* b) {
                    return asc ? (a->birthdate < b->birthdate) : (a->birthdate > b->birthdate);
                });
            }
            else if (currentActorSortKey == SortKey::Length)
            {
                QuickSort::Sort(displayActors, [asc = currentActorSortAscending](const Actor* a, const Actor* b) {
                    return asc ? (a->filmIds.size() < b->filmIds.size()) : (a->filmIds.size() > b->filmIds.size());
                });
            }
        }
    }
}