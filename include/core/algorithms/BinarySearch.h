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
            static std::vector<T> FindInRange(const std::vector<T>& sortedData, const Value& low, const Value& high, ValueGetter getter);

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
            static std::size_t LowerBound(const std::vector<T>& sortedData, const Value& value, ValueGetter getter);

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
            static std::size_t UpperBound(const std::vector<T>& sortedData, const Value& value, ValueGetter getter);
    };
}

#include "../../../src/core/algorithms/BinarySearch.cpp"