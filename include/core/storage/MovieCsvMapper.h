#pragma once

/// @file MovieCsvMapper.h
/// @brief Маппер для сериализации/десериализации Movie в CSV.
///
/// Определяет CSV-формат для таблицы фильмов:
/// id,title,studio,description,year,length,rating,cover,streamLink,genres,actorIds

#include "core/models/Movie.h"

#include <memory>
#include <string>
#include <vector>

namespace FilmLibrary
{
    class MovieCsvMapper
    {
        public:
            /// @brief Строка заголовка CSV.
            static std::string Header();

            /// @brief Создать Movie из полей CSV-строки.
            /// @param fields     Массив полей, полученный из CsvParser::SplitCsvLine.
            /// @param lineNumber Номер строки (для диагностики ошибок).
            /// @return unique_ptr на Movie или nullptr при ошибке.
            static std::unique_ptr<Movie> FromFields(const std::vector<std::string>& fields, int lineNumber);

            /// @brief Сериализовать Movie в CSV-строку.
            static std::string ToLine(const Movie& movie);
    };
}