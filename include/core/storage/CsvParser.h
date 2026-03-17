#pragma once

/// @file CsvParser.h
/// @brief Обобщённый парсер и генератор CSV-файлов.
///
/// Предоставляет общую утилитарную логику для работы с CSV:
/// разбор строк, экранирование полей. Специфика каждой таблицы
/// (Movie, Actor) реализуется в соответствующих Mapper-классах.

#include <memory>
#include <string>
#include <vector>

namespace FilmLibrary
{
    /// @brief Обобщённый результат парсинга CSV.
    /// @tparam T Тип записи (Movie, Actor и т.д.).
    template <typename T>
    struct CsvParseResult
    {
        std::vector<std::unique_ptr<T>> records;
        std::vector<std::string> errors;
    };

    /// @brief Утилитарный класс для работы с CSV-файлами.
    class CsvParser
    {
        public:
            /// @brief Разбить CSV-строку на поля с учётом экранирования.
            static std::vector<std::string> SplitCsvLine(const std::string& line);

            /// @brief Экранировать поле для записи в CSV.
            static std::string EscapeField(const std::string& field);

            /// @brief Загрузить записи из CSV, используя mapper для преобразования.
            ///
            /// @tparam T       Тип записи.
            /// @tparam Mapper  Класс с методами:
            ///                 - static std::string Header()
            ///                 - static std::unique_ptr<T> FromFields(const std::vector<std::string>&, int lineNum)
            ///                 - static std::string ToLine(const T&)
            ///
            /// @param filePath Путь к CSV-файлу.
            /// @return Результат парсинга с записями и ошибками.
            template <typename T, typename Mapper>
            static CsvParseResult<T> LoadFromFile(const std::string& filePath);

            /// @brief Сохранить записи в CSV, используя mapper для сериализации.
            template <typename T, typename Mapper>
            static bool SaveToFile(const std::string& filePath, const std::vector<std::unique_ptr<T>>& records);
    };
}