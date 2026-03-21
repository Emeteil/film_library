/// @file QuickSort.cpp
/// @brief Реализация быстрой сортировки (Хоара).

#include "core/algorithms/QuickSort.h"

namespace FilmLibrary
{
    // NOTE: QuickSort - шаблонный класс. Все методы определены в заголовочном файле.
    // если потребуется сократить время компиляции.
    
    // TODO: Реализовать быструю сортировку.
    // template void QuickSort::Sort<Movie*, ...>(std::vector<Movie*>&, ...);
    template <typename T, typename Comparator>
    void QuickSort::Sort(std::vector<T>& data, Comparator comparator)
    {
    }

    template <typename T, typename Comparator>
    void QuickSort::SortRange(std::vector<T>& data, int low, int high, Comparator& comp)
    {
    }

    template <typename T, typename Comparator>
    int QuickSort::Partition(std::vector<T>& data, int low, int high, Comparator& comp)
    {
        return 0;
    }

    template <typename T, typename Comparator>
    void QuickSort::InsertionSort(std::vector<T>& data, int low, int high, Comparator& comp)
    {
    }

    template <typename T>
    int QuickSort::MedianOfThree(std::vector<T>& data, int low, int high)
    {
        return 0;
    }
}