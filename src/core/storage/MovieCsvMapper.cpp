/// @file MovieCsvMapper.cpp
/// @brief Реализация маппера Movie ↔ CSV.

#include "core/storage/MovieCsvMapper.h"
#include "core/storage/CsvParser.h"
#include "core/utils/Logger.h"

#include <sstream>

namespace FilmLibrary
{
    std::string MovieCsvMapper::Header()
    {
        return "id,title,studio,description,year,length,rating,cover,streamLink,genres,actorIds";
    }

    std::unique_ptr<Movie> MovieCsvMapper::FromFields(const std::vector<std::string>& fields, int lineNumber)
    {
        // TODO: Реализовать парсинг полей в объект Movie.
        //
        // Ожидаемые поля (11 шт):
        //   0: id, 1: title, 2: studio, 3: description, 4: year,
        //   5: length, 6: rating, 7: cover, 8: streamLink,
        //   9: genres (разделитель |), 10: actorIds (разделитель |)

        (void)fields;
        (void)lineNumber;
        return nullptr;
    }

    std::string MovieCsvMapper::ToLine(const Movie& movie)
    {
        // TODO: Сериализовать Movie в CSV-строку.
        //
        // Формат: id,title,studio,description,year,length,rating,cover,streamLink,genres,actorIds
        // genres: жанры через | (Action|Drama|Comedy)
        // actorIds: id актёров через | (1|2|3)

        (void)movie;
        return "";
    }
}