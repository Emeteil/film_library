/// @file test_optimal_bst.cpp
/// @brief Юнит-тесты для дерева оптимального поиска.

#include "TestFramework.h"
#include "core/algorithms/OptimalBST.h"

#include <algorithm>
#include <string>
#include <vector>

TEST(OptimalBST_Empty)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    ASSERT_TRUE(tree.Find(1).empty());
}

TEST(OptimalBST_SingleElement)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    tree.BuildTree({42});

    auto result = tree.Find(42);
    ASSERT_EQ(result.size(), static_cast<std::size_t>(1));
    ASSERT_EQ(result[0], 42);
}

TEST(OptimalBST_Find_FromValues)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> values = {30, 10, 50, 20, 40};
    tree.BuildTree(values);

    auto r10 = tree.Find(10);
    ASSERT_EQ(r10.size(), static_cast<std::size_t>(1));
    ASSERT_EQ(r10[0], 10);

    auto r50 = tree.Find(50);
    ASSERT_EQ(r50.size(), static_cast<std::size_t>(1));
    ASSERT_EQ(r50[0], 50);

    auto r99 = tree.Find(99);
    ASSERT_TRUE(r99.empty());
}

TEST(OptimalBST_Find_DuplicateKeys)
{
    //Значения с одинаковыми ключами группируются
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v % 10; });

    std::vector<int> values = {10, 20, 30, 11, 21};
    //ключи: 0, 0, 0, 1, 1
    //группировка: key=0 → {10,20,30}, key=1 → {11,21}
    tree.BuildTree(values);

    auto r0 = tree.Find(0);
    ASSERT_EQ(r0.size(), static_cast<std::size_t>(3));

    auto r1 = tree.Find(1);
    ASSERT_EQ(r1.size(), static_cast<std::size_t>(2));
}

TEST(OptimalBST_FindByPrefix)
{
    FilmLibrary::OptimalBST<std::string, std::string> tree([](const std::string& v) { return v; });

    std::vector<std::string> values = {"Apple", "Application", "Banana", "Band", "Cat"};
    tree.BuildTree(values);

    auto ap = tree.FindByPrefix("App");
    ASSERT_EQ(ap.size(), static_cast<std::size_t>(2));

    std::sort(ap.begin(), ap.end());
    ASSERT_EQ(ap[0], "Apple");
    ASSERT_EQ(ap[1], "Application");

    auto ban = tree.FindByPrefix("Ban");
    ASSERT_EQ(ban.size(), static_cast<std::size_t>(2));

    auto c = tree.FindByPrefix("C");
    ASSERT_EQ(c.size(), static_cast<std::size_t>(1));
    ASSERT_EQ(c[0], "Cat");

    auto none = tree.FindByPrefix("нету");
    ASSERT_TRUE(none.empty());
}

TEST(OptimalBST_ClearAndRebuild)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> values1 = {10, 20, 30};
    tree.BuildTree(values1);
    ASSERT_EQ(tree.Find(10).size(), static_cast<std::size_t>(1));

    tree.Clear();
    ASSERT_TRUE(tree.Find(10).empty());

    std::vector<int> values2 = {100, 200, 300, 400, 500};
    tree.BuildTree(values2);
    ASSERT_EQ(tree.Find(100).size(), static_cast<std::size_t>(1));
}

TEST(OptimalBST_MoveSemantics)
{
    FilmLibrary::OptimalBST<int, int> tree1([](const int& v) { return v; });

    std::vector<int> values = {10, 20, 30};
    tree1.BuildTree(values);

    FilmLibrary::OptimalBST<int, int> tree2(std::move(tree1));
    ASSERT_EQ(tree2.Find(10).size(), static_cast<std::size_t>(1));
    ASSERT_TRUE(tree1.Find(10).empty());

    FilmLibrary::OptimalBST<int, int> tree3([](const int& v) { return v; });
    tree3 = std::move(tree2);
    ASSERT_EQ(tree3.Find(10).size(), static_cast<std::size_t>(1));
    ASSERT_TRUE(tree2.Find(10).empty());
}