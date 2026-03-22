#ifndef CORE_ALGORITHMS_BINARYSEARCH_CPP
#define CORE_ALGORITHMS_BINARYSEARCH_CPP

/// @file BinarySearch.cpp
/// @brief Реализация бинарного поиска для фильтрации по диапазонам.

#include "core/algorithms/BinarySearch.h"

namespace FilmLibrary
{
    template <typename T, typename Value, typename ValueGetter>
    std::vector<T> BinarySearch::FindInRange(const std::vector<T>& sortedData, const Value& low, const Value& high, ValueGetter getter)
    {
        std::vector<T> result;

        if (low > high || sortedData.empty())
            return result;

        std::size_t first = LowerBound(sortedData, low, getter);
        std::size_t last  = UpperBound(sortedData, high, getter);

        result.reserve(last - first);
        for (std::size_t i = first; i < last; ++i)
            result.push_back(sortedData[i]);

        return result;
    }

    template <typename T, typename Value, typename ValueGetter>
    std::size_t BinarySearch::LowerBound(const std::vector<T>& sortedData, const Value& value, ValueGetter getter)
    {
        std::size_t lo = 0;
        std::size_t hi = sortedData.size();

        while (lo < hi)
        {
            std::size_t mid = lo + (hi - lo) / 2;
            if (getter(sortedData[mid]) < value)
                lo = mid + 1;
            else
                hi = mid;
        }

        return lo;
    }

    template <typename T, typename Value, typename ValueGetter>
    std::size_t BinarySearch::UpperBound(const std::vector<T>& sortedData, const Value& value, ValueGetter getter)
    {
        std::size_t lo = 0;
        std::size_t hi = sortedData.size();

        while (lo < hi)
        {
            std::size_t mid = lo + (hi - lo) / 2;
            if (getter(sortedData[mid]) <= value)
                lo = mid + 1;
            else
                hi = mid;
        }

        return lo;
    }
}

#endif