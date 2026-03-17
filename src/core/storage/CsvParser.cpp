/// @file CsvParser.cpp
/// @brief Реализация обобщённого CSV-парсера.

#include "core/storage/CsvParser.h"
#include "core/storage/MovieCsvMapper.h"
#include "core/storage/ActorCsvMapper.h"
#include "core/utils/Logger.h"

#include <fstream>
#include <sstream>

namespace FilmLibrary
{
    std::vector<std::string> CsvParser::SplitCsvLine(const std::string& line)
    {
        // TODO: Реализовать разбор CSV с учётом кавычек и экранирования.
        //
        // Обработать:
        // - Поля в кавычках: "value with, comma"
        // - Экранирование кавычек: "value with ""quotes"""
        // - Пустые поля: ,,

        (void)line;
        return {};
    }

    std::string CsvParser::EscapeField(const std::string& field)
    {
        // TODO: Экранировать поле для CSV.
        //
        // Если поле содержит запятые, кавычки или переносы строк -
        // обернуть в кавычки, удвоив внутренние кавычки.

        (void)field;
        return field;
    }

    // --- Явные инстанциации шаблонных методов ---

    template <>
    CsvParseResult<Movie> CsvParser::LoadFromFile<Movie, MovieCsvMapper>(const std::string& filePath)
    {
        CsvParseResult<Movie> result;

        // TODO: Реализовать загрузку CSV.
        //
        // 1. Открыть файл std::ifstream.
        // 2. Пропустить заголовочную строку.
        // 3. Для каждой строки: SplitCsvLine → MovieCsvMapper::FromFields.
        // 4. Успешные записи в result.records, ошибки в result.errors.

        (void)filePath;
        return result;
    }

    template <>
    bool CsvParser::SaveToFile<Movie, MovieCsvMapper>(const std::string& filePath, const std::vector<std::unique_ptr<Movie>>& records)
    {
        // TODO: Реализовать сохранение в CSV.
        //
        // 1. Открыть файл std::ofstream.
        // 2. Записать MovieCsvMapper::Header().
        // 3. Для каждой записи: MovieCsvMapper::ToLine().

        (void)filePath;
        (void)records;
        return false;
    }

    template <>
    CsvParseResult<Actor> CsvParser::LoadFromFile<Actor, ActorCsvMapper>(const std::string& filePath)
    {
        CsvParseResult<Actor> result;

        // TODO: Реализовать загрузку CSV актёров.

        (void)filePath;
        return result;
    }

    template <>
    bool CsvParser::SaveToFile<Actor, ActorCsvMapper>(const std::string& filePath, const std::vector<std::unique_ptr<Actor>>& records)
    {
        // TODO: Реализовать сохранение актёров в CSV.

        (void)filePath;
        (void)records;
        return false;
    }
}
