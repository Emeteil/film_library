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
            /// @tparam ValueGetter Функция извлечения сравниваемого значения из T.
            /// @param  sortedData  Отсортированный вектор.
            /// @param  low         Нижняя граница диапазона.
            /// @param  high        Верхняя граница диапазона.
            /// @param  getter      Функция получения значения из элемента.
            /// @return Вектор элементов, попадающих в диапазон.
            template <typename T, typename Value, typename ValueGetter>
            static std::vector<T> FindInRange(const std::vector<T>& sortedData, const Value& low, const Value& high, ValueGetter getter);

            /// @brief Найти индекс первого элемента >= value (lower_bound).
            template <typename T, typename Value, typename ValueGetter>
            static std::size_t LowerBound(const std::vector<T>& sortedData, const Value& value, ValueGetter getter);

            /// @brief Найти индекс первого элемента > value (upper_bound).
            template <typename T, typename Value, typename ValueGetter>
            static std::size_t UpperBound(const std::vector<T>& sortedData, const Value& value, ValueGetter getter);
    };
}
