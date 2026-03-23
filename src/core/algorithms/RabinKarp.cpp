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

        std::string lowerText = text;
        std::string lowerPattern = pattern;
        std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), [](unsigned char c){ return std::tolower(c); });
        std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), [](unsigned char c){ return std::tolower(c); });

        std::size_t n = lowerText.length();
        std::size_t m = lowerPattern.length();
        std::size_t patternHash = ComputeHash(lowerPattern, 0, m);

        for (std::size_t i = 0; i <= n - m; i++)
        {
            if (ComputeHash(lowerText, i, m) == patternHash)
            {
                if (lowerText.compare(i, m, lowerPattern) == 0)
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