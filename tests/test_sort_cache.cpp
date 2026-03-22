#include "TestFramework.h"
#include "core/index/SortCache.h"
#include "core/models/Movie.h"

#include <memory>
#include <vector>

using namespace FilmLibrary;

static std::unique_ptr<Movie> MakeMovie(int year)
{
    auto m = std::make_unique<Movie>();
    m->year = year;
    return m;
}

TEST(SortCache_GetSorted)
{
    std::vector<std::unique_ptr<Movie>> movies;
    movies.push_back(MakeMovie(2005));
    movies.push_back(MakeMovie(1999));
    movies.push_back(MakeMovie(2020));

    SortCache cache;
    auto comp = [](const Movie* a, const Movie* b) { return a->year < b->year; };

    const auto& sorted1 = cache.GetSorted("year_asc", movies, comp);

    ASSERT_EQ(sorted1.size(), std::size_t(3));
    ASSERT_EQ(sorted1[0]->year, 1999);
    ASSERT_EQ(sorted1[1]->year, 2005);
    ASSERT_EQ(sorted1[2]->year, 2020);

    const auto& sorted2 = cache.GetSorted("year_asc", movies, comp);
    ASSERT_EQ(sorted1.data(), sorted2.data());
}

TEST(SortCache_InvalidateAll)
{
    std::vector<std::unique_ptr<Movie>> movies;
    movies.push_back(MakeMovie(1));

    SortCache cache;
    auto comp = [](const Movie* a, const Movie* b) { return a->year < b->year; };

    const auto& sorted1 = cache.GetSorted("test", movies, comp);
    ASSERT_EQ(sorted1.size(), std::size_t(1));

    movies.push_back(MakeMovie(2));
    cache.Invalidate();

    const auto& sorted2 = cache.GetSorted("test", movies, comp);
    ASSERT_EQ(sorted2.size(), std::size_t(2));
}

TEST(SortCache_InvalidateKey)
{
    std::vector<std::unique_ptr<Movie>> movies;
    movies.push_back(MakeMovie(1));

    SortCache cache;
    auto comp = [](const Movie* a, const Movie* b) { return a->year < b->year; };

    const auto& sorted1 = cache.GetSorted("key1", movies, comp);
    const auto& sorted2 = cache.GetSorted("key2", movies, comp);
    ASSERT_EQ(sorted1.size(), std::size_t(1));
    ASSERT_EQ(sorted2.size(), std::size_t(1));

    movies.push_back(MakeMovie(2));
    cache.Invalidate("key1");

    const auto& new_sorted1 = cache.GetSorted("key1", movies, comp);
    const auto& new_sorted2 = cache.GetSorted("key2", movies, comp);

    ASSERT_EQ(new_sorted1.size(), std::size_t(2));
    ASSERT_EQ(new_sorted2.size(), std::size_t(1));
}
