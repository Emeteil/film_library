/// @file test_quick_sort.cpp
/// @brief Юнит-тесты для быстрой сортировки.

#include "TestFramework.h"
#include "core/algorithms/QuickSort.h"

#include <vector>
#include <cstdlib>
#include <ctime>

// TODO: Реализовать тесты после имплементации QuickSort.

TEST(QuickSort_RandomArray)
{
    std::vector<int> v(10);
    std::srand(std::time(NULL));
    for (size_t i = 0; i < 10; i++) {
        v[i] = 1 + std::rand() % 10;
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
    
    FilmLibrary::QuickSort::Sort(v, [](int a, int b) { return a < b; });

    for (size_t i = 0; i < 10; i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
    
    // TODO: Создать массив случайных чисел.
    //       Отсортировать. Проверить, что массив отсортирован.
}

TEST(QuickSort_AlreadySorted)
{
    // TODO: Отсортировать уже отсортированный массив.
    //       Проверить, что результат корректен.
}

TEST(QuickSort_ReverseSorted)
{
    // TODO: Отсортировать массив, отсортированный в обратном порядке.
}

TEST(QuickSort_EmptyArray)
{
    // TODO: Пустой вектор - сортировка не должна падать.
}

TEST(QuickSort_SingleElement)
{
    // TODO: Вектор из одного элемента - результат тривиален.
}

TEST(QuickSort_Duplicates)
{
    // TODO: Массив с повторяющимися элементами.
}
