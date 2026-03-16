/// @file DataManager.cpp
/// @brief Реализация центрального менеджера данных.

#include "core/DataManager.h"
#include "core/storage/CsvParser.h"
#include "core/utils/Logger.h"

#include <algorithm>

namespace FilmLibrary
{
    DataManager::DataManager() = default;
    DataManager::~DataManager() = default;

    std::vector<std::string> DataManager::LoadFromCsv(const std::string& filePath)
    {
        // TODO: Реализовать загрузку.
        //
        // 1. Вызвать CsvParser::LoadFromFile(filePath).
        // 2. Перенести movies из результата в this->movies.
        // 3. Назначить уникальные ID каждому фильму (nextId++).
        // 4. Вызвать OnDataChanged() для построения индексов.
        // 5. Залогировать результат.
        // 6. Вернуть список ошибок.

        (void)filePath;
        return {};
    }

    bool DataManager::SaveToCsv(const std::string& filePath) const
    {
        // TODO: Делегировать CsvParser::SaveToFile().
        (void)filePath;
        return false;
    }

    int DataManager::AddMovie(Movie movieData)
    {
        // TODO: Реализовать добавление.
        //
        // 1. Назначить movieData.id = nextId++.
        // 2. Создать unique_ptr и добавить в movies.
        // 3. Вызвать OnDataChanged().
        // 4. Вернуть id.

        (void)movieData;
        return -1;
    }

    bool DataManager::UpdateMovie(int id, const Movie& updatedData)
    {
        // TODO: Реализовать обновление.
        //
        // 1. Найти фильм по id в movies.
        // 2. Обновить все поля (кроме id).
        // 3. Вызвать OnDataChanged().

        (void)id;
        (void)updatedData;
        return false;
    }

    bool DataManager::DeleteMovie(int id)
    {
        // TODO: Реализовать удаление.
        //
        // 1. Найти фильм по id в movies.
        // 2. Удалить из вектора (erase-remove idiom с unique_ptr).
        // 3. Вызвать OnDataChanged().

        (void)id;
        return false;
    }

    const Movie* DataManager::GetMovieById(int id) const
    {
        // TODO: Линейный поиск по вектору movies.
        (void)id;
        return nullptr;
    }

    const std::vector<std::unique_ptr<Movie>>& DataManager::GetAllMovies() const
    {
        return movies;
    }

    std::size_t DataManager::GetMovieCount() const
    {
        return movies.size();
    }

    std::vector<Movie*> DataManager::FindByTitlePrefix(const std::string& prefix) const
    {
        return indexManager.FindByTitlePrefix(prefix);
    }

    std::vector<Movie*> DataManager::FindByStudioPrefix(const std::string& prefix) const
    {
        return indexManager.FindByStudioPrefix(prefix);
    }

    std::vector<Movie*> DataManager::FindByYear(int year) const
    {
        return indexManager.FindByYear(year);
    }

    std::vector<Movie*> DataManager::FilterByRatingRange(double low, double high) const
    {
        return indexManager.FilterByRatingRange(low, high);
    }

    std::vector<Movie*> DataManager::FilterByLengthRange(int low, int high) const
    {
        return indexManager.FilterByLengthRange(low, high);
    }

    std::vector<Movie*> DataManager::SearchByDescription(const std::string& pattern) const
    {
        return indexManager.SearchByDescription(movies, pattern);
    }

    const std::vector<Movie*>& DataManager::GetSortedByRating(bool ascending)
    {
        // TODO: Делегировать SortCache::GetSorted() с соответствующим компаратором.
        (void)ascending;
        static std::vector<Movie*> empty;
        return empty;
    }

    const std::vector<Movie*>& DataManager::GetSortedByYear(bool ascending)
    {
        (void)ascending;
        static std::vector<Movie*> empty;
        return empty;
    }

    const std::vector<Movie*>& DataManager::GetSortedByTitle(bool ascending)
    {
        (void)ascending;
        static std::vector<Movie*> empty;
        return empty;
    }

    void DataManager::OnDataChanged()
    {
        // TODO: Реализовать перестройку индексов и сброс кэша.
        //
        // 1. indexManager.Rebuild(movies);
        // 2. sortCache.Invalidate();
    }
}
