/// @file ActorCsvMapper.cpp
/// @brief Реализация маппера Actor ↔ CSV.

#include "core/storage/ActorCsvMapper.h"
#include "core/storage/CsvParser.h"
#include "core/utils/Logger.h"

#include <sstream>

namespace FilmLibrary
{
    std::string ActorCsvMapper::Header()
    {
        return "id,name,description,birthdate,filmIds,photo";
    }

    std::unique_ptr<Actor> ActorCsvMapper::FromFields(const std::vector<std::string>& fields, int lineNumber)
    {
        // TODO: Реализовать парсинг полей в объект Actor.
        //
        // Ожидаемые поля (6 шт):
        //   0: id, 1: name, 2: description, 3: birthdate (timestamp),
        //   4: filmIds (разделитель |), 5: photo

        (void)fields;
        (void)lineNumber;
        return nullptr;
    }

    std::string ActorCsvMapper::ToLine(const Actor& actor)
    {
        // TODO: Сериализовать Actor в CSV-строку.
        //
        // Формат: id,name,description,birthdate,filmIds,photo
        // filmIds: id фильмов через | (1|2|3)

        (void)actor;
        return "";
    }
}