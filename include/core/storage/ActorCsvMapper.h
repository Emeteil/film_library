#pragma once

/// @file ActorCsvMapper.h
/// @brief Маппер для сериализации/десериализации Actor в CSV.
///
/// Определяет CSV-формат для таблицы актёров:
/// id,name,description,birthdate,filmIds,photo

#include "core/models/Actor.h"

#include <memory>
#include <string>
#include <vector>

namespace FilmLibrary
{
    class ActorCsvMapper
    {
        public:
            /// @brief Строка заголовка CSV.
            static std::string Header();

            /// @brief Создать Actor из полей CSV-строки.
            /// @param fields     Массив полей, полученный из CsvParser::SplitCsvLine.
            /// @param lineNumber Номер строки (для диагностики ошибок).
            /// @return unique_ptr на Actor или nullptr при ошибке.
            static std::unique_ptr<Actor> FromFields(const std::vector<std::string>& fields, int lineNumber);

            /// @brief Сериализовать Actor в CSV-строку.
            static std::string ToLine(const Actor& actor);
    };
}