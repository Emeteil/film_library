#pragma once

/// @file BinarySearch.h
/// @brief Бинарный поиск для фильтрации по диапазонам (рейтинг, длительность).
///
/// Работает с предварительно отсортированными векторами сырых указателей Movie*.

#include <functional>
#include <vector>

namespace FilmLibrary
{
    class BinarySearch
    {
        public:
            /// @brief Найти все элементы, значение которых попадает в диапазон [low, high].
            ///
            /// @tparam T           Тип элемента (обычно Movie*).
            /// @tparam Value       Тип сравниваемого значения.
            /// @tparam ValueGetter Функция извлечения сравниваемого значения из T.
            /// @param  sortedData  Отсортированный вектор.
            /// @param  low         Нижняя граница диапазона (включительно).
            /// @param  high        Верхняя граница диапазона (включительно).
            /// @param  getter      Функция получения значения из элемента.
            /// @return Вектор элементов, попадающих в диапазон.
            template <typename T, typename Value, typename ValueGetter>
            static std::vector<T> FindInRange(
                const std::vector<T>& sortedData,
                const Value& low,
                const Value& high,
                ValueGetter getter)
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

            /// @brief Найти индекс первого элемента >= value (lower_bound).
            ///
            /// @tparam T           Тип элемента (обычно Movie*).
            /// @tparam Value       Тип сравниваемого значения.
            /// @tparam ValueGetter Функция извлечения сравниваемого значения из T.
            /// @param  sortedData  Отсортированный вектор.
            /// @param  value       Значение для поиска.
            /// @param  getter      Функция получения значения из элемента.
            /// @return Индекс первого элемента, который не меньше value.
            template <typename T, typename Value, typename ValueGetter>
            static std::size_t LowerBound(
                const std::vector<T>& sortedData,
                const Value& value,
                ValueGetter getter)
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

            /// @brief Найти индекс первого элемента > value (upper_bound).
            ///
            /// @tparam T           Тип элемента (обычно Movie*).
            /// @tparam Value       Тип сравниваемого значения.
            /// @tparam ValueGetter Функция извлечения сравниваемого значения из T.
            /// @param  sortedData  Отсортированный вектор.
            /// @param  value       Значение для поиска.
            /// @param  getter      Функция получения значения из элемента.
            /// @return Индекс первого элемента, который больше value.
            template <typename T, typename Value, typename ValueGetter>
            static std::size_t UpperBound(
                const std::vector<T>& sortedData,
                const Value& value,
                ValueGetter getter)
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
    };
}