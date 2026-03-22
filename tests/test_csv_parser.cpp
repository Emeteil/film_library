/// @file test_csv_parser.cpp
/// @brief Интеграционные тесты для CSV-парсера.

#include "TestFramework.h"
#include "core/storage/CsvParser.h"
#include "core/storage/MovieCsvMapper.h"
#include "core/storage/ActorCsvMapper.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

// TODO: Реализовать тесты после имплементации CsvParser.

TEST(CsvParser_SplitCsvLine)
{
    std::string string1 = "1997,Ford,E350,\"ac, abs, moon\",3000.00";
    std::string string2 = "1999,Chevy,\"Venture \"\"Extended Edition\"\"\", ,4900.00";
    std::string string3 = "1996,Jeep,Grand Cherokee,\"MUST SELL! air, moon roof, loaded\",4799.00";
    std::vector<std::string> expected1 = {"1997", "Ford", "E350", "ac, abs, moon", "3000.00"};
    std::vector<std::string> expected2 = {"1999", "Chevy", "Venture \"Extended Edition\"", " ","4900.00"};
    std::vector<std::string> expected3 = {"1996", "Jeep", "Grand Cherokee", "MUST SELL! air, moon roof, loaded", "4799.00"};
    ASSERT_EQ(FilmLibrary::CsvParser::SplitCsvLine(string1), expected1);
    ASSERT_EQ(FilmLibrary::CsvParser::SplitCsvLine(string2), expected2);
    ASSERT_EQ(FilmLibrary::CsvParser::SplitCsvLine(string3), expected3);
}

TEST(CsvParser_EscapedFields)
{
    std::string test1 = "ac, abs, moon";
    std::string test2 = "Venture \"Extended Edition\"";
    std::string test3 = "MUST SELL! air, moon roof, loaded";
    std::string expected1 = "\"ac, abs, moon\"";
    std::string expected2 = "\"Venture \"\"Extended Edition\"\"\"";
    std::string expected3 = "\"MUST SELL! air, moon roof, loaded\"";
    ASSERT_EQ(FilmLibrary::CsvParser::EscapeField(test1), expected1);
    ASSERT_EQ(FilmLibrary::CsvParser::EscapeField(test2), expected2);
    ASSERT_EQ(FilmLibrary::CsvParser::EscapeField(test3), expected3);
}

void CreateTestFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    file << content;
}

TEST(CsvParser_LoadAndSaveRoundTrip)
{
    // TODO: Создать вектор Movie, сохранить в CSV, загрузить обратно.
    //       Сравнить данные - они должны совпадать.
    std::string testFile = "roundTrip.csv";

    std::vector<std::unique_ptr<FilmLibrary::Movie>> OriginalData;
    auto movie = std::make_unique<FilmLibrary::Movie>();
    movie->id = 1;
    movie->title = "Fight Club";
    movie->year = 1999;
    OriginalData.push_back(std::move(movie));

    bool isSave = FilmLibrary::CsvParser::SaveToFile<FilmLibrary::Movie, FilmLibrary::MovieCsvMapper>(testFile, OriginalData);
    ASSERT_TRUE(isSave);
    auto result = FilmLibrary::CsvParser::LoadFromFile<FilmLibrary::Movie, FilmLibrary::MovieCsvMapper>(testFile);  
    ASSERT_EQ(result.records.size(), 1);
    ASSERT_EQ(result.records[0]->title, "Fight Club");
    ASSERT_EQ(result.records[0]->year, 1999);
    ASSERT_TRUE(result.errors.empty());
    std::filesystem::remove(testFile);
}

TEST(CsvParser_HandleMalformedLines)
{
    // TODO: Файл с битыми строками.
    //       Проверить, что хорошие строки загружены, ошибки в result.errors.
    std::string testFile = "badData.csv";
    CreateTestFile(testFile, "id,title,year\n badid,SomeTitle,badyear");
    auto result = FilmLibrary::CsvParser::LoadFromFile<FilmLibrary::Movie, FilmLibrary::MovieCsvMapper>(testFile);  
    ASSERT_EQ(result.records.size(), 1); 
    ASSERT_EQ(result.errors.size(), 0);
    std::filesystem::remove(testFile);
}

TEST(CsvParser_EmptyFile)
{
    std::string testFile = "empty.csv";
    CreateTestFile(testFile, "");
    auto result = FilmLibrary::CsvParser::LoadFromFile<FilmLibrary::Movie, FilmLibrary::MovieCsvMapper>(testFile);  
    ASSERT_EQ(result.records.size(), 0);
    ASSERT_EQ(result.errors.size(), 0);
    std::filesystem::remove(testFile);
}

TEST(CsvParser_FileNotFound)
{
    auto result = FilmLibrary::CsvParser::LoadFromFile<FilmLibrary::Movie, FilmLibrary::MovieCsvMapper>("mega_file.csv");  
    ASSERT_EQ(result.records.size(), 0);
    ASSERT_EQ(result.errors.size(), 1);
}
