/// @file RabinKarp.cpp
/// @brief Реализация алгоритма Рабина-Карпа для полнотекстового поиска.

#include "core/algorithms/RabinKarp.h"

namespace FilmLibrary
{
    std::vector<std::size_t> RabinKarp::Search(const std::string& text, const std::string& pattern)
    {
        // TODO: Реализовать алгоритм Рабина-Карпа.
        //
        // Алгоритм:
        // 1. Вычислить хэш паттерна.
        // 2. Вычислить хэш первого окна текста длиной pattern.size().
        // 3. Скользящим окном двигаться по тексту:
        //    a. Если хэши совпали - сравнить строки посимвольно (защита от коллизий).
        //    b. Пересчитать хэш окна за O(1) с помощью rolling hash.
        // 4. Собрать все позиции совпадений.

        std::vector<std::size_t> result;
        (void)text;
        (void)pattern;
        return result;
    }

    bool RabinKarp::Contains(const std::string& text, const std::string& pattern)
    {
        // TODO: Использовать Search() и проверить наличие хотя бы одного результата.
        return !Search(text, pattern).empty();
    }

    std::size_t RabinKarp::ComputeHash(const std::string& str, std::size_t start, std::size_t length)
    {
        // TODO: Реализовать вычисление хэша.
        (void)str;
        (void)start;
        (void)length;
        return 0;
    }
}
