#pragma once

/// @file TestFramework.h
/// @brief Базовый шаблон/фреймворк для юнит-тестов с гитхаба (без внешних зависимостей).
///
/// Использование:
///   TEST(TestName)
///   {
///       ASSERT_TRUE(condition);
///       ASSERT_EQ(a, b);
///   }
///
/// В main_test.cpp вызвать RunAllTests().

#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace TestFramework
{

class NotImplementedException : public std::exception
{
public:
    const char* what() const noexcept override { return "Not implemented"; }
};

struct TestCase
{
    std::string name;
    std::function<void()> func;
};

inline std::vector<TestCase>& GetTestRegistry()
{
    static std::vector<TestCase> tests;
    return tests;
}

inline int RegisterTest(const std::string& name, std::function<void()> func)
{
    GetTestRegistry().push_back({name, std::move(func)});
    return 0;
}

inline int RunAllTests()
{
    int passed = 0;
    int failed = 0;
    int not_impl = 0;

    for (const auto& test : GetTestRegistry())
    {
        try
        {
            test.func();
            std::cout << "[PASS] " << test.name << std::endl;
            ++passed;
        }
        catch (const NotImplementedException&)
        {
            std::cout << "[SKIP] " << test.name << " (Not Implemented)" << std::endl;
            ++not_impl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "[FAIL] " << test.name << ": " << e.what() << std::endl;
            ++failed;
        }
    }

    std::cout << "\n=== Результат: " << passed << " пройдено, " << failed << " провалено, " << not_impl << " не реализовано ===" << std::endl;

    return failed > 0 ? 1 : 0;
}

}

#define TEST(name)                                                          \
    void Test_##name();                                                     \
    static int test_reg_##name =                                            \
        TestFramework::RegisterTest(#name, Test_##name);                    \
    void Test_##name()

#define ASSERT_TRUE(expr)                                                   \
    do {                                                                    \
        if (!(expr))                                                        \
        {                                                                   \
            throw std::runtime_error(                                       \
                std::string("ASSERT_TRUE failed: ") + #expr +              \
                " at " + __FILE__ + ":" + std::to_string(__LINE__));        \
        }                                                                   \
    } while (false)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))

#define ASSERT_EQ(a, b)                                                     \
    do {                                                                    \
        if ((a) != (b))                                                     \
        {                                                                   \
            throw std::runtime_error(                                       \
                std::string("ASSERT_EQ failed: ") + #a + " != " + #b +    \
                " at " + __FILE__ + ":" + std::to_string(__LINE__));        \
        }                                                                   \
    } while (false)

#define ASSERT_NE(a, b)                                                     \
    do {                                                                    \
        if ((a) == (b))                                                     \
        {                                                                   \
            throw std::runtime_error(                                       \
                std::string("ASSERT_NE failed: ") + #a + " == " + #b +    \
                " at " + __FILE__ + ":" + std::to_string(__LINE__));        \
        }                                                                   \
    } while (false)

#define NOT_IMPLEMENTED() throw TestFramework::NotImplementedException()