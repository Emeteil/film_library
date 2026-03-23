/// @file RabinKarp.cpp
/// @brief Реализация алгоритма Рабина-Карпа для полнотекстового поиска.

#include "core/algorithms/RabinKarp.h"
#include <functional>
#include <algorithm>
#include <cctype>

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
                bool match = true;
                for (std::size_t j = 0; j < m; ++j)
                {
                    if (std::tolower(static_cast<unsigned char>(text[i + j])) != std::tolower(static_cast<unsigned char>(pattern[j])))
                    {
                        match = false;
                        break;
                    }
                }
                
                if (match) result.push_back(i);
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
        std::size_t h = 0;
        for (std::size_t i = 0; i < length; ++i)
        {
            h = h * 31 + std::tolower(static_cast<unsigned char>(str[start + i]));
        }
        return h;
    }
}