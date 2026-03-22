/// @file MovieCsvMapper.cpp
/// @brief Реализация маппера Movie ↔ CSV.

#include "core/storage/MovieCsvMapper.h"
#include "core/storage/CsvParser.h"
#include "core/utils/Logger.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace FilmLibrary
{
    std::string MovieCsvMapper::Header()
    {
        return "id,title,studio,description,year,length,rating,cover,streamLink,genres,actorIds";
    }

    template <typename T>
    std::vector<T>  Parse(const std::string& string, char delimiter) {
        std::vector<T> tokens;
        std::string token;
        std::stringstream ss(string);
        while (std::getline(ss, token, delimiter))
        {
            if (!token.empty())
            {
                if constexpr (std::is_same_v<T, std::string>) 
                {
                    tokens.push_back(token);
                } 
                else if constexpr (std::is_arithmetic_v<T>)
                {
                    tokens.push_back(static_cast<T>(std::stod(token)));
                }
            }   
        }
        return tokens;
    }

    std::unique_ptr<Movie> MovieCsvMapper::FromFields(const std::vector<std::string>& fields, int)
    {
        if (fields.size() < 11) return nullptr;
        auto movie = std::make_unique<Movie>();
        try
        {
            movie->id = std::stoi(fields[0]);
            movie->title = fields[1];
            movie->studio = fields[2];
            movie->description = fields[3];
            movie->year = std::stoi(fields[4]);
            movie->length = std::stoi(fields[5]);
            movie->rating = std::stod(fields[6]);
            movie->cover = fields[7];
            movie->streamLink = fields[8];
            movie->genres = Parse<std::string>(fields[9], '|');
            movie->actorIds = Parse<int>(fields[10], '|');
        }
        catch(const std::exception& e)
        {
            return nullptr;
        }
        return movie;
    }

    template <typename T>
    std::string Unparse(const std::vector<T>& data) {
        if (data.empty()) return "";
        std::string result;
        for (auto d : data) {
            if constexpr (std::is_same_v<T, std::string>) 
            {
                result += d + '|';
            } 
            else if constexpr (std::is_arithmetic_v<T>)
            {
                result += std::to_string(d) + '|';
            }
        }
        result.pop_back();
        return result;
    }

    std::string MovieCsvMapper::ToLine(const Movie& movie)
    {
        std::string result;
        try
        {
            result = std::to_string(movie.id) + "," + 
                    CsvParser::EscapeField(movie.title) + "," + 
                    CsvParser::EscapeField(movie.studio) + "," + 
                    CsvParser::EscapeField(movie.description) + "," + 
                    std::to_string(movie.year) + "," + 
                    std::to_string(movie.length) + "," + 
                    std::to_string(movie.rating) + "," + 
                    CsvParser::EscapeField(movie.cover) + "," + 
                    CsvParser::EscapeField(movie.streamLink) + "," + 
                    CsvParser::EscapeField(Unparse(movie.genres)) + "," + 
                    CsvParser::EscapeField(Unparse(movie.actorIds));
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return "";
        }
        return result;
    }
}