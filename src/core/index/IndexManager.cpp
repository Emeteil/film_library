/// @file IndexManager.cpp
/// @brief Реализация менеджера индексов.

#include "core/index/IndexManager.h"
#include "core/algorithms/BinarySearch.h"
#include "core/algorithms/QuickSort.h"
#include "core/algorithms/RabinKarp.h"

#include <algorithm>

namespace FilmLibrary
{
    IndexManager::IndexManager() 
        : idIndex([](Movie* const& m) -> int { return m->id; })
        , titleIndex([](Movie* const& m) -> std::string { return m->title; })
        , studioIndex([](Movie* const& m) -> std::string { return m->studio; })
        , yearIndex([](Movie* const& m) -> int { return m->year; })
    {
    }

    IndexManager::~IndexManager() = default;

    void IndexManager::Rebuild(const std::vector<std::unique_ptr<Movie>>& movies)
    {
        std::vector<Movie*> pointers;
        pointers.reserve(movies.size());
        for (const auto& m : movies)
        {
            pointers.push_back(m.get());
        }

        idIndex.BuildTree(pointers);
        titleIndex.BuildTree(pointers);
        studioIndex.BuildTree(pointers);
        yearIndex.BuildTree(pointers);

        RebuildSortedVectors(movies);
    }

    std::vector<Movie*> IndexManager::FindByTitlePrefix(const std::string& prefix) const
    {
        return titleIndex.FindByPrefix(prefix);
    }

    std::vector<Movie*> IndexManager::FindByStudioPrefix(const std::string& prefix) const
    {
        return studioIndex.FindByPrefix(prefix);
    }

    std::vector<Movie*> IndexManager::FindByYear(int year) const
    {
        return yearIndex.Find(year);
    }

    std::vector<Movie*> IndexManager::FilterByRatingRange(double low, double high) const
    {
        return BinarySearch::FindInRange(sortedByRating, low, high, [](Movie* m) { return m->rating; });
    }

    std::vector<Movie*> IndexManager::FilterByLengthRange(int low, int high) const
    {
        return BinarySearch::FindInRange(sortedByLength, low, high, [](Movie* m) { return m->length; });
    }

    std::vector<Movie*> IndexManager::SearchByDescription(const std::vector<std::unique_ptr<Movie>>& movies, const std::string& pattern) const
    {
        std::vector<Movie*> result;
        if (pattern.empty())
            return result;

        for (const auto& m : movies)
        {
            if (RabinKarp::Contains(m->description, pattern))
                result.push_back(m.get());
        }
        return result;
    }

    std::vector<Movie*> IndexManager::FilterByGenre(const std::vector<std::unique_ptr<Movie>>& movies, const std::string& genre) const
    {
        std::vector<Movie*> result;
        for (const auto& m : movies)
        {
            if (std::find(m->genres.begin(), m->genres.end(), genre) != m->genres.end())
                result.push_back(m.get());
        }
        return result;
    }

    Movie* IndexManager::GetMovieById(int id) const
    {
        auto results = idIndex.Find(id);
        return results.empty() ? nullptr : results[0];
    }

    void IndexManager::RebuildSortedVectors(const std::vector<std::unique_ptr<Movie>>& movies)
    {
        sortedByRating.clear();
        sortedByLength.clear();
        sortedByRating.reserve(movies.size());
        sortedByLength.reserve(movies.size());

        for (const auto& m : movies)
        {
            sortedByRating.push_back(m.get());
            sortedByLength.push_back(m.get());
        }

        QuickSort::Sort(sortedByRating, [](Movie* a, Movie* b) {
            return a->rating < b->rating;
        });

        QuickSort::Sort(sortedByLength, [](Movie* a, Movie* b) {
            return a->length < b->length;
        });
    }
}