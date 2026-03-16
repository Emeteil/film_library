#pragma once

/// @file RabinKarp.h
/// @brief Алгоритм Рабина-Карпа для полнотекстового поиска подстроки в описании фильма.

#include <string>
#include <vector>

namespace FilmLibrary
{
    class RabinKarp
    {
        public:
            /// @brief Найти все позиции вхождения паттерна в тексте.
            /// @param text    Текст для поиска.
            /// @param pattern Искомая подстрока.
            /// @return Вектор позиций (индексов) всех вхождений.
            static std::vector<std::size_t> Search(const std::string& text, const std::string& pattern);

            /// @brief Проверить, содержит ли текст заданную подстроку.
            /// @param text    Текст для поиска.
            /// @param pattern Искомая подстрока.
            /// @return true, если подстрока найдена.
            static bool Contains(const std::string& text, const std::string& pattern);

        private:
            static constexpr std::size_t BASE = 256;
            static constexpr std::size_t MOD = 1000000007;

            /// @brief Вычислить хэш строки.
            static std::size_t ComputeHash(const std::string& str, std::size_t start, std::size_t length);
    };
}