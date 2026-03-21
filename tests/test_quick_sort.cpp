/// @file test_quick_sort.cpp
/// @brief Юнит-тесты для быстрой сортировки.

#include "TestFramework.h"
#include "core/algorithms/QuickSort.h"

#include <vector>

TEST(QuickSort_RandomArray)
{  
    std::vector<int> data = {8, 1, 9, 3, 5};
    FilmLibrary::QuickSort::Sort(data, [](int a, int b) { return a < b; }); 
    ASSERT_EQ(data.size(), static_cast<std::size_t>(5));
    ASSERT_EQ(data[0], 1);
    ASSERT_EQ(data[1], 3);
    ASSERT_EQ(data[2], 5);
    ASSERT_EQ(data[3], 8);
    ASSERT_EQ(data[4], 9);   
}

TEST(QuickSort_AlreadySorted)
{
    std::vector<int> data = {1, 3, 5, 8, 9};
    FilmLibrary::QuickSort::Sort(data, [](int a, int b) { return a < b; }); 
    ASSERT_EQ(data.size(), static_cast<std::size_t>(5));
    ASSERT_EQ(data[0], 1);
    ASSERT_EQ(data[1], 3);
    ASSERT_EQ(data[2], 5);
    ASSERT_EQ(data[3], 8);
    ASSERT_EQ(data[4], 9);
}

TEST(QuickSort_ReverseSorted)
{
    std::vector<int> data = {9, 8, 5, 3, 1};
    FilmLibrary::QuickSort::Sort(data, [](int a, int b) { return a < b; }); 
    ASSERT_EQ(data.size(), static_cast<std::size_t>(5));
    ASSERT_EQ(data[0], 1);
    ASSERT_EQ(data[1], 3);
    ASSERT_EQ(data[2], 5);
    ASSERT_EQ(data[3], 8);
    ASSERT_EQ(data[4], 9);
}

TEST(QuickSort_EmptyArray)
{
    std::vector<int> data;
    FilmLibrary::QuickSort::Sort(data, [](int a, int b) { return a < b; }); 
    ASSERT_EQ(data.size(), static_cast<std::size_t>(0));
}

TEST(QuickSort_SingleElement)
{
    std::vector<int> data = {1};
    FilmLibrary::QuickSort::Sort(data, [](int a, int b) { return a < b; }); 
    ASSERT_EQ(data.size(), static_cast<std::size_t>(1));
    ASSERT_EQ(data[0], 1);
}

TEST(QuickSort_Duplicates)
{
    std::vector<int> data = {9, 8, 1, 1, 1};
    FilmLibrary::QuickSort::Sort(data, [](int a, int b) { return a < b; }); 
    ASSERT_EQ(data.size(), static_cast<std::size_t>(5));
    ASSERT_EQ(data[0], 1);
    ASSERT_EQ(data[1], 1);
    ASSERT_EQ(data[2], 1);
    ASSERT_EQ(data[3], 8);
    ASSERT_EQ(data[4], 9);
}
