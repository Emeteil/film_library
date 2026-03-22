/// @file test_binary_search.cpp
/// @brief Юнит-тесты для бинарного поиска.
#include "TestFramework.h"
#include "core/algorithms/BinarySearch.h"
#include "core/models/Movie.h"
#include <vector>
#include <memory>

using namespace FilmLibrary;

// ---------------------------------------------------------------------------
// Вспомогательные функции
// ---------------------------------------------------------------------------

// Создаёт Movie с заданным рейтингом (остальные поля по умолчанию).
static Movie MakeMovieRating(double rating)
{
    Movie m;
    m.rating = rating;
    return m;
}

// Создаёт Movie с заданной длительностью в секундах.
static Movie MakeMovieLength(int length)
{
    Movie m;
    m.length = length;
    return m;
}

// Геттеры для полей
static auto ratingGetter = [](Movie* m) { return m->rating; };
static auto lengthGetter = [](Movie* m) { return m->length; };

// ---------------------------------------------------------------------------
// LowerBound
// ---------------------------------------------------------------------------

TEST(BinarySearch_LowerBound)
{
    std::vector<Movie> movies = {
        MakeMovieRating(1.0),
        MakeMovieRating(3.0),
        MakeMovieRating(5.0),
        MakeMovieRating(7.0),
        MakeMovieRating(9.0),
    };
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    // Точное совпадение - первый элемент с этим значением
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 5.0, ratingGetter), std::size_t(2));

    // Между значениями - возвращает индекс следующего большего
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 4.0, ratingGetter), std::size_t(2));

    // Меньше всех - первый элемент
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 0.0, ratingGetter), std::size_t(0));

    // Больше всех - возвращает size (элемент не найден)
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 10.0, ratingGetter), std::size_t(5));

    // Ровно первый элемент
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 1.0, ratingGetter), std::size_t(0));

    // Ровно последний элемент
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 9.0, ratingGetter), std::size_t(4));
}

TEST(BinarySearch_LowerBound_Empty)
{
    std::vector<Movie*> ptrs;
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 5.0, ratingGetter), std::size_t(0));
}

TEST(BinarySearch_LowerBound_SingleElement)
{
    Movie m = MakeMovieRating(5.0);
    std::vector<Movie*> ptrs = {&m};

    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 5.0, ratingGetter), std::size_t(0));
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 4.0, ratingGetter), std::size_t(0));
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 6.0, ratingGetter), std::size_t(1));
}

TEST(BinarySearch_LowerBound_Duplicates)
{
    std::vector<Movie> movies = {
        MakeMovieRating(3.0), MakeMovieRating(3.0), MakeMovieRating(3.0),
        MakeMovieRating(7.0), MakeMovieRating(7.0),
    };
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    // Возвращает индекс ПЕРВОГО вхождения 3.0
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 3.0, ratingGetter), std::size_t(0));
    // Возвращает индекс ПЕРВОГО вхождения 7.0
    ASSERT_EQ(BinarySearch::LowerBound(ptrs, 7.0, ratingGetter), std::size_t(3));
}

// ---------------------------------------------------------------------------
// UpperBound
// ---------------------------------------------------------------------------

TEST(BinarySearch_UpperBound)
{
    std::vector<Movie> movies = {
        MakeMovieRating(1.0),
        MakeMovieRating(3.0),
        MakeMovieRating(5.0),
        MakeMovieRating(7.0),
        MakeMovieRating(9.0),
    };
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    // Первый элемент > 5.0 - это 7.0 на индексе 3
    ASSERT_EQ(BinarySearch::UpperBound(ptrs, 5.0, ratingGetter), std::size_t(3));

    // Между значениями - первый элемент > 4.0 это 5.0 на индексе 2
    ASSERT_EQ(BinarySearch::UpperBound(ptrs, 4.0, ratingGetter), std::size_t(2));

    // Меньше всех - первый элемент > 0 это 1.0 на индексе 0
    ASSERT_EQ(BinarySearch::UpperBound(ptrs, 0.0, ratingGetter), std::size_t(0));

    // Последний элемент - первый > 9.0 не существует, возвращает size
    ASSERT_EQ(BinarySearch::UpperBound(ptrs, 9.0, ratingGetter), std::size_t(5));

    // Больше всех - возвращает size
    ASSERT_EQ(BinarySearch::UpperBound(ptrs, 10.0, ratingGetter), std::size_t(5));
}

TEST(BinarySearch_UpperBound_Duplicates)
{
    std::vector<Movie> movies = {
        MakeMovieRating(3.0), MakeMovieRating(3.0), MakeMovieRating(3.0),
        MakeMovieRating(7.0), MakeMovieRating(7.0),
    };
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    // Первый индекс после всех 3.0 - индекс 3
    ASSERT_EQ(BinarySearch::UpperBound(ptrs, 3.0, ratingGetter), std::size_t(3));
    // Первый индекс после всех 7.0 - size = 5
    ASSERT_EQ(BinarySearch::UpperBound(ptrs, 7.0, ratingGetter), std::size_t(5));
}

// ---------------------------------------------------------------------------
// FindInRange
// ---------------------------------------------------------------------------

TEST(BinarySearch_FindInRange)
{
    std::vector<Movie> movies = {
        MakeMovieRating(1.0),
        MakeMovieRating(3.0),
        MakeMovieRating(5.0),
        MakeMovieRating(7.0),
        MakeMovieRating(9.0),
    };
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    auto result = BinarySearch::FindInRange(ptrs, 3.0, 7.0, ratingGetter);
    ASSERT_EQ(result.size(), std::size_t(3));
    ASSERT_EQ(result[0]->rating, 3.0);
    ASSERT_EQ(result[1]->rating, 5.0);
    ASSERT_EQ(result[2]->rating, 7.0);
}

TEST(BinarySearch_EmptyArray)
{
    std::vector<Movie*> ptrs;
    auto result = BinarySearch::FindInRange(ptrs, 1.0, 9.0, ratingGetter);
    ASSERT_TRUE(result.empty());
}

TEST(BinarySearch_AllElementsInRange)
{
    std::vector<Movie> movies = {
        MakeMovieRating(1.0),
        MakeMovieRating(3.0),
        MakeMovieRating(5.0),
        MakeMovieRating(7.0),
        MakeMovieRating(9.0),
    };
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    // Диапазон покрывает весь массив
    auto result = BinarySearch::FindInRange(ptrs, 0.0, 10.0, ratingGetter);
    ASSERT_EQ(result.size(), std::size_t(5));
}

TEST(BinarySearch_NoElementsInRange)
{
    std::vector<Movie> movies = {
        MakeMovieRating(1.0),
        MakeMovieRating(3.0),
        MakeMovieRating(5.0),
    };
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    // Диапазон полностью вне массива
    auto result = BinarySearch::FindInRange(ptrs, 6.0, 9.0, ratingGetter);
    ASSERT_TRUE(result.empty());

    // Между двух значений - ни один элемент не попадает
    auto result2 = BinarySearch::FindInRange(ptrs, 2.0, 2.9, ratingGetter);
    ASSERT_TRUE(result2.empty());
}

TEST(BinarySearch_FindInRange_SingleMatch)
{
    std::vector<Movie> movies = {
        MakeMovieRating(1.0),
        MakeMovieRating(3.0),
        MakeMovieRating(5.0),
        MakeMovieRating(7.0),
        MakeMovieRating(9.0),
    };
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    auto result = BinarySearch::FindInRange(ptrs, 5.0, 5.0, ratingGetter);
    ASSERT_EQ(result.size(), std::size_t(1));
    ASSERT_EQ(result[0]->rating, 5.0);
}

TEST(BinarySearch_FindInRange_InvertedRange)
{
    std::vector<Movie> movies = {MakeMovieRating(3.0), MakeMovieRating(7.0)};
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    // low > high - результат должен быть пустым
    auto result = BinarySearch::FindInRange(ptrs, 7.0, 3.0, ratingGetter);
    ASSERT_TRUE(result.empty());
}

TEST(BinarySearch_FindInRange_Duplicates)
{
    std::vector<Movie> movies = {
        MakeMovieRating(3.0), MakeMovieRating(3.0), MakeMovieRating(3.0),
        MakeMovieRating(7.0),
    };
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    auto result = BinarySearch::FindInRange(ptrs, 3.0, 3.0, ratingGetter);
    ASSERT_EQ(result.size(), std::size_t(3));
    for (auto* p : result)
        ASSERT_EQ(p->rating, 3.0);
}

TEST(BinarySearch_FindInRange_LengthField)
{
    // Проверяем что работает и для поля length (int), не только rating (double)
    std::vector<Movie> movies = {
        MakeMovieLength(60),
        MakeMovieLength(90),
        MakeMovieLength(120),
        MakeMovieLength(150),
        MakeMovieLength(180),
    };
    std::vector<Movie*> ptrs;
    for (auto& m : movies) ptrs.push_back(&m);

    auto result = BinarySearch::FindInRange(ptrs, 90, 150, lengthGetter);
    ASSERT_EQ(result.size(), std::size_t(3));
    ASSERT_EQ(result[0]->length, 90);
    ASSERT_EQ(result[1]->length, 120);
    ASSERT_EQ(result[2]->length, 150);
}