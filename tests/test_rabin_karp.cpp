/// @file test_rabin_karp.cpp
/// @brief Юнит-тесты для алгоритма Рабина-Карпа.

#include "TestFramework.h"
#include "core/algorithms/RabinKarp.h"

#include <string>
#include <vector>

using namespace FilmLibrary;

TEST(RabinKarp_SimpleMatch)
{
    std::string text = "Ak3j7jds4testkghgj";
    std::string pattern = "test";
    
    auto result = RabinKarp::Search(text, pattern);
    
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0], 9);
}

TEST(RabinKarp_MultipleMatches)
{
    std::string text = "abababa";
    std::string pattern = "aba";
    
    auto result = RabinKarp::Search(text, pattern);
    
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result[0], 0);
    ASSERT_EQ(result[1], 2);
    ASSERT_EQ(result[2], 4);
}

TEST(RabinKarp_NoMatch)
{
    std::string text = "Hello World";
    std::string pattern = "xyz";
    
    auto result = RabinKarp::Search(text, pattern);
    
    ASSERT_TRUE(result.empty());
}

TEST(RabinKarp_EmptyPattern)
{
    std::string text = "Empty pattern test";
    std::string pattern = "";
    
    auto result = RabinKarp::Search(text, pattern);
    ASSERT_TRUE(result.empty());
}

TEST(RabinKarp_PatternLongerThanText)
{
    std::string text = "Short";
    std::string pattern = "Longer than text";
    
    auto result = RabinKarp::Search(text, pattern);
    ASSERT_TRUE(result.empty());
}

TEST(RabinKarp_Contains)
{
    std::string text = "Find me if you can";
    
    ASSERT_TRUE(RabinKarp::Contains(text, "me"));
    ASSERT_TRUE(RabinKarp::Contains(text, "can"));
    ASSERT_TRUE(RabinKarp::Contains(text, "Find"));
    ASSERT_FALSE(RabinKarp::Contains(text, "missing"));
    ASSERT_TRUE(RabinKarp::Contains(text, ""));
}

TEST(RabinKarp_UnicodeText)
{
    std::string text = "Привет мир, мир";
    std::string pattern = "мир";
    
    auto result = RabinKarp::Search(text, pattern);
    ASSERT_EQ(result.size(), 2);
}

TEST(RabinKarp_CaseInsensitivity)
{
    std::string text = "Hello World";
    
    auto result1 = RabinKarp::Search(text, "world");
    ASSERT_EQ(result1.size(), 1);
    ASSERT_EQ(result1[0], 6);
    
    auto result2 = RabinKarp::Search(text, "HELLO");
    ASSERT_EQ(result2.size(), 1);
    ASSERT_EQ(result2[0], 0);
}