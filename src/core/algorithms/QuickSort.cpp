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
        T pivot = data[MedianOfThree(data, low, high, comparator)];
        int i = low;
        int j = high;
        
        while (i <= j)
        {
            while (comparator(data[i], pivot)) i++;
            while (comparator(pivot, data[j])) j--;
            
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

    template <typename T, typename Comparator>
    int QuickSort::MedianOfThree(std::vector<T>& data, int low, int high, Comparator& comparator)
    {
        int mid = low + (high - low) / 2;
        const T& a = data[low];
        const T& b = data[mid];
        const T& c = data[high];
        if (comparator(a, b)) {
            if (comparator(b, c)) return mid;
            if (comparator(a, c)) return high;
            return low;
        } else {
            if (comparator(a, c)) return low;
            if (comparator(b, c)) return high;
            return mid;
        }
    }   
}