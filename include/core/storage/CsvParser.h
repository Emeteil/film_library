#pragma once

/// @file CsvParser.h
/// @brief Парсер и генератор CSV-файлов для сериализации данных о фильмах.
///
/// Поддерживает экранирование полей (кавычки, запятые, переносы строк).
/// Не зависит от сторонних библиотек.

#include "core/models/Movie.h"

#include <memory>
#include <string>
#include <vector>

namespace FilmLibrary
{
    /// @brief Результат парсинга: список фильмов + список ошибок.
    struct CsvParseResult
    {
        std::vector<std::unique_ptr<Movie>> movies;
        std::vector<std::string> errors; ///< Описание ошибочных строк.
    };

    /// @brief Класс для чтения и записи CSV-файлов с данными о фильмах.
    class CsvParser
    {
        public:
            /// @brief Загрузить фильмы из CSV-файла.
            /// @param filePath Путь к CSV-файлу.
            /// @return Результат парсинга с фильмами и ошибками.
            static CsvParseResult LoadFromFile(const std::string& filePath);

            /// @brief Сохранить фильмы в CSV-файл.
            /// @param filePath  Путь к CSV-файлу (будет перезаписан).
            /// @param movies    Вектор unique_ptr на фильмы.
            /// @return true при успехе, false при ошибке записи.
            static bool SaveToFile(const std::string& filePath, const std::vector<std::unique_ptr<Movie>>& movies);

        private:
            /// @brief Разобрать одну строку CSV и создать объект Movie.
            /// @param line        Строка CSV.
            /// @param lineNumber  Номер строки (для диагностики).
            /// @return unique_ptr на Movie или nullptr при ошибке.
            static std::unique_ptr<Movie> ParseLine(const std::string& line, int lineNumber);

            /// @brief Разбить CSV-строку на поля с учётом экранирования.
            static std::vector<std::string> SplitCsvLine(const std::string& line);

            /// @brief Экранировать поле для записи в CSV.
            static std::string EscapeField(const std::string& field);

            /// @brief Сериализовать объект Movie в CSV-строку.
            static std::string MovieToCsvLine(const Movie& movie);
    };
}