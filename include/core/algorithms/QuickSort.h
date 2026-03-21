#pragma once

/// @file QuickSort.h
/// @brief Самописная быстрая сортировка (Хоара) для вектора указателей.
///
/// Поддерживает произвольный компаратор.
/// Оптимизации: выбор медианы из трёх, переход на InsertionSort для малых подмассивов.

#include <functional>
#include <vector>

namespace FilmLibrary
{
    class QuickSort
    {
        public:
            /// @brief Сортирует вектор in-place по заданному компаратору.
            /// @tparam T           Тип элемента вектора (обычно Movie*).
            /// @tparam Comparator  Функтор/лямбда: bool(const T&, const T&).
            /// @param data         Вектор для сортировки.
            /// @param comparator   Компаратор "меньше".
            template <typename T, typename Comparator>
            static void Sort(std::vector<T>& /*data*/, Comparator /*comparator*/) {}

        private:
            template <typename T, typename Comparator>
            static void SortRange(std::vector<T>& /*data*/, int /*low*/, int /*high*/, Comparator& /*comp*/) {}

            template <typename T, typename Comparator>
            static std::pair<int, int> Partition(std::vector<T>& /*data*/, int /*low*/, int /*high*/, Comparator& /*comp*/);

            template <typename T>
            static int MedianOfThree(std::vector<T>& /*data*/, int /*low*/, int /*high*/) { return 0; }
    };
}
