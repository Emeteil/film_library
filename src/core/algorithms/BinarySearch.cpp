/// @file BinarySearch.cpp
/// @brief Реализация бинарного поиска для фильтрации по диапазонам.

#include "core/algorithms/BinarySearch.h"

namespace FilmLibrary
{
    // NOTE: BinarySearch - шаблонный класс. Все методы определены в заголовочном файле.

    // TODO: Реализовать бинарный поиск.
    template <typename T, typename Value, typename ValueGetter>
    std::vector<T> BinarySearch::FindInRange(const std::vector<T>& sortedData, const Value& low, const Value& high, ValueGetter getter)
    {
        return {};
    }

    template <typename T, typename Value, typename ValueGetter>
    std::size_t BinarySearch::LowerBound(const std::vector<T>& sortedData, const Value& value, ValueGetter getter)
    {
        return 0;
    }

    template <typename T, typename Value, typename ValueGetter>
    std::size_t BinarySearch::UpperBound(const std::vector<T>& sortedData, const Value& value, ValueGetter getter)
    {
        return 0;
    }
}