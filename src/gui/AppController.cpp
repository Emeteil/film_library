/// @file AppController.cpp
/// @brief Реализация контроллера приложения.

#include "gui/AppController.h"
#include "gui/widgets/NotificationManager.h"
#include "core/utils/Logger.h"

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
        // TODO: Реализовать инициализацию.
        //
        // 1. dataManager.LoadFromCsv(moviesCsvPath).
        // 2. actorManager.LoadFromCsv(actorsCsvPath).
        // 3. Собрать и вернуть все ошибки.

        std::vector<std::string> errors;
        auto movieErrors = dataManager.LoadFromCsv(moviesCsvPath);
        auto actorErrors = actorManager.LoadFromCsv(actorsCsvPath);

        errors.insert(errors.end(), movieErrors.begin(), movieErrors.end());
        errors.insert(errors.end(), actorErrors.begin(), actorErrors.end());
        return errors;
    }

    void AppController::Shutdown()
    {
        // Данные уже сохранены автоматически при каждом изменении.
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
        bool ok = dataManager.DeleteMovie(id);
        if (ok) displayDirty = true;
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
        // TODO: Сохранить параметры фильтра и пометить displayDirty.
        (void)low;
        (void)high;
        displayDirty = true;
    }

    void AppController::FilterByLengthRange(int low, int high)
    {
        // TODO: Сохранить параметры фильтра и пометить displayDirty.
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
        // TODO: Реализовать построение displayMovies на основе:
        //
        // 1. Текущего поискового режима (currentSearchMode + currentSearchQuery).
        // 2. Текущих фильтров (рейтинг, длительность).
        // 3. Текущей сортировки (currentSortKey + currentSortAscending).

        displayMovies.clear();

        const auto& allMovies = dataManager.GetAllMovies();
        for (const auto& m : allMovies)
        {
            displayMovies.push_back(m.get());
        }
    }
}
