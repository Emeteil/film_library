/// @file ActorCsvMapper.cpp
/// @brief Реализация маппера Actor ↔ CSV.

#include "core/storage/ActorCsvMapper.h"
#include "core/storage/CsvParser.h"
#include "core/utils/Logger.h"

#include <iostream>
#include <sstream>
#include <set>

namespace FilmLibrary
{
    std::string ActorCsvMapper::Header()
    {
        return "id,name,description,birthdate,filmIds,photo";
    }

    std::set<int>  ParseFilms(const std::string& string, char delimiter) {
        std::set<int> tokens;
        std::string token;
        std::stringstream ss(string);
        while (std::getline(ss, token, delimiter))
        {
            if (!token.empty())
            {
                tokens.insert(stoi(token));
            }
        }
        return tokens;
    }

    std::unique_ptr<Actor> ActorCsvMapper::FromFields(const std::vector<std::string>& fields, int)
    {
        if (fields.size() < 6) return nullptr;
        auto actor = std::make_unique<Actor>();
        try
        {
            actor->id = std::stoi(fields[0]);
            actor->name = fields[1];
            actor->description = fields[2];
            actor->birthdate = std::stoll(fields[3]);
            actor->filmIds = ParseFilms(fields[4], '|');
            actor->photo = fields[5];
        }
        catch(const std::exception& e)
        {
            return nullptr;
        }
        return actor;
    }

    std::string UnparseFilms(const std::set<int>& films)
    {
        if (films.empty()) return "";
        std::string result;
        for (auto id : films)
        {
            result += std::to_string(id) + '|';
        }
        result.pop_back();
        return result;
    }

    std::string ActorCsvMapper::ToLine(const Actor& actor)
    {
        std::string result;
        try
        {
            result = std::to_string(actor.id) + "," + 
                    CsvParser::EscapeField(actor.name) + "," + 
                    CsvParser::EscapeField(actor.description) + "," + 
                    std::to_string(actor.birthdate) + "," + 
                    CsvParser::EscapeField(UnparseFilms(actor.filmIds)) + "," + 
                    CsvParser::EscapeField(actor.photo);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return "";
        }
        return result;
    }
}