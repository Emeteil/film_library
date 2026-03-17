/// @file RabinKarp.cpp
/// @brief Реализация алгоритма Рабина-Карпа для полнотекстового поиска.

#include "core/algorithms/RabinKarp.h"
#include <functional>

namespace FilmLibrary
{
    std::vector<std::size_t> RabinKarp::Search(const std::string& text, const std::string& pattern)
    {
        std::vector<std::size_t> result;
        
        if (pattern.empty() || text.length() < pattern.length())
            return result;

        std::size_t n = text.length();
        std::size_t m = pattern.length();
        std::size_t patternHash = ComputeHash(pattern, 0, m);

        for (std::size_t i = 0; i <= n - m; i++)
        {
            if (ComputeHash(text, i, m) == patternHash)
            {
                if (text.compare(i, m, pattern) == 0)
                    result.push_back(i);
            }
        }

        return result;
    }

    bool RabinKarp::Contains(const std::string& text, const std::string& pattern)
    {
        if (pattern.empty())
            return true;
        if (text.length() < pattern.length())
            return false;
        
        return !Search(text, pattern).empty();
    }

    std::size_t RabinKarp::ComputeHash(const std::string& str, std::size_t start, std::size_t length)
    {
        return std::hash<std::string>{}(str.substr(start, length));
    }
}