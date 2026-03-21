/// @file QuickSort.cpp
/// @brief Реализация быстрой сортировки (Хоара).

#include "core/algorithms/QuickSort.h"
#include <utility>
#include <algorithm>

namespace FilmLibrary
{
    template <typename T, typename Comparator>
    void QuickSort::Sort(std::vector<T>& data, Comparator comparator)
    {
        if (data.empty()) return;
        SortRange(data, 0, static_cast<int>(data.size()) - 1, comparator);
    }

    template <typename T, typename Comparator>
    void QuickSort::SortRange(std::vector<T>& data, int low, int high, Comparator& comparator)
    {
        while (low < high)
        {
            std::pair<int, int> p = Partition(data, low, high, comparator);
            int i = p.first;
            int j = p.second;
            if (j - low < high - i)
            {
                SortRange(data, low, j, comparator);
                low = i;
            }
            else
            {
                SortRange(data, i, high, comparator);
                high = j;
            }
        }
    }

    template <typename T, typename Comparator>
    std::pair<int, int> QuickSort::Partition(std::vector<T>& data, int low, int high, Comparator& comparator)
    {
        T pivot = data[MedianOfThree(data, low, high)];
        int i = low;
        int j = high;
        
        while (i < j)
        {
            while (i <= high)
            {
                if (!comparator(data[i], pivot)) break;
                i++;
            }
            while (j >= low)
            {
                if (comparator(data[j], pivot)) break;
                j--;
            }
            
            if (i <= j) 
            {
                std::swap(data[i], data[j]);
                i++;
                j--;
            }
            else break;
        }
        return std::make_pair(i, j);
    }

    template <typename T>
    int QuickSort::MedianOfThree(std::vector<T>&, int low, int)
    {
        return low;
    }
}