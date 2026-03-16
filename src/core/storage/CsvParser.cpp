/// @file CsvParser.cpp
/// @brief Реализация парсера и генератора CSV для объектов Movie.

#include "core/storage/CsvParser.h"
#include "core/utils/Logger.h"

#include <fstream>
#include <sstream>

namespace FilmLibrary
{
    CsvParseResult CsvParser::LoadFromFile(const std::string& filePath)
    {
        CsvParseResult result;

        // TODO: Реализовать загрузку CSV.
        //
        // Алгоритм:
        // 1. Открыть файл std::ifstream.
        // 2. Пропустить заголовочную строку (если есть).
        // 3. Для каждой строки вызвать ParseLine().
        // 4. Успешные Movie добавить в result.movies.
        // 5. Ошибки добавить в result.errors.

        (void)filePath;
        return result;
    }

    bool CsvParser::SaveToFile(const std::string& filePath,
                               const std::vector<std::unique_ptr<Movie>>& movies)
    {
        // TODO: Реализовать сохранение в CSV.
        //
        // 1. Открыть файл std::ofstream (перезапись).
        // 2. Записать заголовочную строку.
        // 3. Для каждого фильма вызвать MovieToCsvLine() и записать в файл.
        // 4. Вернуть true при успехе, false при ошибке.

        (void)filePath;
        (void)movies;
        return false;
    }

    std::unique_ptr<Movie> CsvParser::ParseLine(const std::string& line, int lineNumber)
    {
        // TODO: Реализовать парсинг одной CSV-строки.
        //
        // 1. Разбить строку через SplitCsvLine().
        // 2. Проверить количество полей.
        // 3. Преобразовать типы (stoi, stod).
        // 4. При ошибке вернуть nullptr и залогировать.

        (void)line;
        (void)lineNumber;
        return nullptr;
    }

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

    std::string CsvParser::MovieToCsvLine(const Movie& movie)
    {
        // TODO: Сериализовать Movie в CSV-строку.
        //
        // Формат: title,studio,description,year,length,rating,cover,streamLink
        // (id не сохраняем)

        (void)movie;
        return "";
    }
}
