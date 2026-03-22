/// @file CsvParser.cpp
/// @brief Реализация обобщённого CSV-парсера.

#include "core/storage/CsvParser.h"
#include "core/storage/MovieCsvMapper.h"
#include "core/storage/ActorCsvMapper.h"
#include "core/utils/Logger.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace FilmLibrary
{
    std::vector<std::string> CsvParser::SplitCsvLine(const std::string& line)
    {
        std::vector<std::string> words;
        std::string s = "";
        bool inQuotes = false;
        for (size_t i = 0; i < line.size(); ++i) 
        {
            char c = line[i];
            if (c == '"') 
            {
                if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') 
                {
                    s += '"';
                    i++;
                }
                else inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes) 
            {
                words.push_back(s);
                s = "";
            } else
                s += c;
        }
        words.push_back(s);
        return words;
    }

    std::string CsvParser::EscapeField(const std::string& field)
    {
        std::string result;
        bool isNeed = false;
        for (size_t i = 0; i < field.size(); i++)
        {
            char c = field[i];
            if (c == '"') 
            {
                result += "\"\"";
                isNeed = true;
            }            
            else 
            {
                if (c == ',' || c == '\n') isNeed = true;
                result += c;
            }
        }
        if (isNeed) 
        {
            return "\"" + result + "\"";
        }
        return result;
    }

    // --- Явные инстанциации шаблонных методов ---

    template <>
    CsvParseResult<Movie> CsvParser::LoadFromFile<Movie, MovieCsvMapper>(const std::string& filePath)
    {
        CsvParseResult<Movie> result;
        // TODO: Реализовать загрузку CSV.
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            result.errors.push_back("Error opening file: " + filePath); 
            return result;
        }
        std::string line;
        std::getline(file, line);        
        int i = 0;
        while (std::getline(file, line))
        {
            if (line.empty()) continue;
            i++;
            auto fields = SplitCsvLine(line);
            try
            {
                auto movie = MovieCsvMapper::FromFields(fields, i);
                result.records.push_back(move(movie));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        file.close();
        return result;
    }

    template <>
    bool CsvParser::SaveToFile<Movie, MovieCsvMapper>(const std::string& filePath, const std::vector<std::unique_ptr<Movie>>& records)
    {
        // TODO: Реализовать сохранение в CSV.
        std::ofstream file(filePath);
        if (!file.is_open()) 
        {
            return false;
        }
        file << MovieCsvMapper::Header() << std::endl;
        for (const auto& r : records)
        {
            if (r)
            {
                file << MovieCsvMapper::ToLine(*r) << std::endl;   
            }
        }
        return true;
    }

    template <>
    CsvParseResult<Actor> CsvParser::LoadFromFile<Actor, ActorCsvMapper>(const std::string& filePath)
    {
        // TODO: Реализовать загрузку CSV актёров.
        CsvParseResult<Actor> result;
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            result.errors.push_back("Error opening file: " + filePath); 
            return result;
        }
        std::string line;
        std::getline(file, line);        
        int i = 1;
        while (std::getline(file, line))
        {
            if (line.empty()) continue;
            i++;
            auto fields = SplitCsvLine(line);
            try
            {
                auto actor = ActorCsvMapper::FromFields(fields, i);
                result.records.push_back(move(actor));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        file.close();
        return result;
    }

    template <>
    bool CsvParser::SaveToFile<Actor, ActorCsvMapper>(const std::string& filePath, const std::vector<std::unique_ptr<Actor>>& records)
    {
        // TODO: Реализовать сохранение актёров в CSV.
        std::ofstream file(filePath);
        if (!file.is_open()) 
        {
            return false;
        }
        file << ActorCsvMapper::Header() << std::endl;
        for (const auto& r : records)
        {
            if (r)
            {
                file << ActorCsvMapper::ToLine(*r) << std::endl;   
            }
        }
        return true;
    }
}
