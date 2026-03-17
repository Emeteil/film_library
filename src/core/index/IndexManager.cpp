/// @file IndexManager.cpp
/// @brief Реализация менеджера индексов.

#include "core/index/IndexManager.h"
#include "core/algorithms/BinarySearch.h"
#include "core/algorithms/QuickSort.h"
#include "core/algorithms/RabinKarp.h"

#include <algorithm>

namespace FilmLibrary
{
    IndexManager::IndexManager() : titleIndex([](Movie* const& m) -> std::string { return m->title; })
        , studioIndex([](Movie* const& m) -> std::string { return m->studio; })
        , yearIndex([](Movie* const& m) -> int { return m->year; })
    {
    }

    IndexManager::~IndexManager() = default;

    void IndexManager::Rebuild(const std::vector<std::unique_ptr<Movie>>& movies)
    {
        // TODO: Реализовать полную перестройку индексов.
        //
        // 1. Собрать сырые указатели в std::vector<Movie*>.
        // 2. titleIndex.BuildTree(pointers) — ключи извлекаются автоматически через KeyExtractor.
        // 3. studioIndex.BuildTree(pointers).
        // 4. yearIndex.BuildTree(pointers).
        // 5. Перестроить отсортированные векторы.

        (void)movies;
    }

    std::vector<Movie*> IndexManager::FindByTitlePrefix(const std::string& prefix) const
    {
        // TODO: Делегировать titleIndex.FindByPrefix(prefix).
        (void)prefix;
        return {};
    }

    std::vector<Movie*> IndexManager::FindByStudioPrefix(const std::string& prefix) const
    {
        // TODO: Делегировать studioIndex.FindByPrefix(prefix).
        (void)prefix;
        return {};
    }

    std::vector<Movie*> IndexManager::FindByYear(int year) const
    {
        // TODO: Делегировать yearIndex.Find(year).
        (void)year;
        return {};
    }

    std::vector<Movie*> IndexManager::FilterByRatingRange(double low, double high) const
    {
        // TODO: Использовать BinarySearch::FindInRange() по sortedByRating.
        (void)low;
        (void)high;
        return {};
    }

    std::vector<Movie*> IndexManager::FilterByLengthRange(int low, int high) const
    {
        // TODO: Использовать BinarySearch::FindInRange() по sortedByLength.
        (void)low;
        (void)high;
        return {};
    }

    std::vector<Movie*> IndexManager::SearchByDescription(const std::vector<std::unique_ptr<Movie>>& movies, const std::string& pattern) const
    {
        // TODO: Перебрать все фильмы, для каждого вызвать
        //       RabinKarp::Contains(movie->description, pattern).

        (void)movies;
        (void)pattern;
        return {};
    }

    std::vector<Movie*> IndexManager::FilterByGenre(const std::vector<std::unique_ptr<Movie>>& movies, const std::string& genre) const
    {
        // TODO: Перебрать все фильмы, для каждого проверить наличие genre в movie->genres.

        (void)movies;
        (void)genre;
        return {};
    }

    void IndexManager::RebuildSortedVectors(const std::vector<std::unique_ptr<Movie>>& movies)
    {
        // TODO: Заполнить sortedByRating и sortedByLength сырыми указателями,
        //       затем отсортировать каждый вектор через QuickSort::Sort().

        (void)movies;
    }
}
