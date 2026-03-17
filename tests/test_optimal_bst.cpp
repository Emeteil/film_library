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

    ASSERT_TRUE(tree.Empty());
    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(0));
    ASSERT_EQ(tree.Height(), 0);
    ASSERT_EQ(tree.CheckSum(), 0);
    ASSERT_TRUE(tree.Find(1).empty());
    ASSERT_TRUE(tree.InOrderTraversal().empty());
    ASSERT_TRUE(tree.VerifyAlgorithm());
}

TEST(OptimalBST_SingleElement)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> keys = {42};
    std::vector<int> weights = {5};
    tree.BuildTree(keys, weights);

    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(1));
    ASSERT_EQ(tree.Height(), 1);
    ASSERT_EQ(tree.CheckSum(), 5);
    ASSERT_FALSE(tree.Empty());
    ASSERT_TRUE(tree.VerifyAlgorithm());
}

TEST(OptimalBST_EqualWeights_3Nodes)
{
    //keys={1,2,3}, weights={1,1,1}
    //root=2, left=1, right=3 (сбалансированное)
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> keys = {1, 2, 3};
    std::vector<int> weights = {1, 1, 1};
    tree.BuildTree(keys, weights);

    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(3));
    ASSERT_EQ(tree.Height(), 2);
    ASSERT_EQ(tree.CheckSum(), 3);
    ASSERT_TRUE(tree.VerifyAlgorithm());

    // AP[0,3]/AW[0,3] = 5/3
    const auto& aw = tree.GetAW();
    const auto& ap = tree.GetAP();
    ASSERT_EQ(aw[0][3], 3);
    ASSERT_EQ(ap[0][3], 5);
}

TEST(OptimalBST_SkewedWeights_HighFirst)
{
    //keys={1,2,3}, weights={10,1,1}
    //ожидание root=1 (самый частый), правое поддерево {2, 3}
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> keys = {1, 2, 3};
    std::vector<int> weights = {10, 1, 1};
    tree.BuildTree(keys, weights);

    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(3));
    ASSERT_TRUE(tree.VerifyAlgorithm());

    const auto& ar = tree.GetAR();
    ASSERT_EQ(ar[0][3], 1);

    const auto& aw = tree.GetAW();
    const auto& ap = tree.GetAP();
    ASSERT_EQ(aw[0][3], 12);
    ASSERT_EQ(ap[0][3], 15);
}

TEST(OptimalBST_SkewedWeights_HighMiddle)
{
    //keys={1,2,3}, weights={1,10,1}
    //ожидание root=2 (самый частый)
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> keys = {1, 2, 3};
    std::vector<int> weights = {1, 10, 1};
    tree.BuildTree(keys, weights);

    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(3));
    ASSERT_TRUE(tree.VerifyAlgorithm());

    const auto& ar = tree.GetAR();
    ASSERT_EQ(ar[0][3], 2);
}

TEST(OptimalBST_FiveNodes)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> keys = {10, 20, 30, 40, 50};
    std::vector<int> weights = {4, 2, 6, 3, 1};
    tree.BuildTree(keys, weights);

    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(5));
    ASSERT_TRUE(tree.VerifyAlgorithm());

    // AW[0,5] = 4+2+6+3+1 = 16
    const auto& aw = tree.GetAW();
    ASSERT_EQ(aw[0][5], 16);

    // сумма весов = 16
    ASSERT_EQ(tree.CheckSum(), 16);
}

TEST(OptimalBST_LargerTree)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> weights = {1, 5, 2, 8, 3, 7, 4, 6, 9, 1};
    tree.BuildTree(keys, weights);

    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(10));
    ASSERT_TRUE(tree.VerifyAlgorithm());

    const auto& aw = tree.GetAW();
    int totalWeight = 1 + 5 + 2 + 8 + 3 + 7 + 4 + 6 + 9 + 1;
    ASSERT_EQ(aw[0][10], totalWeight);
}

TEST(OptimalBST_AverageWeightedHeight)
{
    //keys={1,2,3}, weights={1,1,1}
    //root=2 (depth 1), left=1 (depth 2), right=3 (depth 2)
    //AWH = (1*2 + 1*1 + 1*2) / (1+1+1) = 5/3 ≈ 1.6667
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> keys = {1, 2, 3};
    std::vector<int> weights = {1, 1, 1};
    tree.BuildTree(keys, weights);

    double awh = tree.AverageWeightedHeight();
    double expected = 5.0 / 3.0;
    ASSERT_TRUE(std::abs(awh - expected) < 0.001);
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

    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(2));

    auto r0 = tree.Find(0);
    ASSERT_EQ(r0.size(), static_cast<std::size_t>(3));

    auto r1 = tree.Find(1);
    ASSERT_EQ(r1.size(), static_cast<std::size_t>(2));
}

TEST(OptimalBST_InOrderTraversal)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> values = {50, 30, 10, 40, 20};
    tree.BuildTree(values);

    auto inOrder = tree.InOrderTraversal();
    ASSERT_EQ(inOrder.size(), static_cast<std::size_t>(5));

    //обход должен дать отсортированные значения
    ASSERT_EQ(inOrder[0], 10);
    ASSERT_EQ(inOrder[1], 20);
    ASSERT_EQ(inOrder[2], 30);
    ASSERT_EQ(inOrder[3], 40);
    ASSERT_EQ(inOrder[4], 50);
}

TEST(OptimalBST_FindInRange)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> values = {10, 20, 30, 40, 50, 60, 70};
    tree.BuildTree(values);

    auto range = tree.FindInRange(25, 55);
    ASSERT_EQ(range.size(), static_cast<std::size_t>(3));

    std::sort(range.begin(), range.end());
    ASSERT_EQ(range[0], 30);
    ASSERT_EQ(range[1], 40);
    ASSERT_EQ(range[2], 50);

    //граничные значения включаются
    auto exact = tree.FindInRange(30, 30);
    ASSERT_EQ(exact.size(), static_cast<std::size_t>(1));
    ASSERT_EQ(exact[0], 30);

    //пустой диапазон
    auto empty = tree.FindInRange(100, 200);
    ASSERT_TRUE(empty.empty());
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
    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(3));
    ASSERT_TRUE(tree.VerifyAlgorithm());

    tree.Clear();
    ASSERT_TRUE(tree.Empty());
    ASSERT_EQ(tree.Height(), 0);

    std::vector<int> values2 = {100, 200, 300, 400, 500};
    tree.BuildTree(values2);
    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(5));
    ASSERT_TRUE(tree.VerifyAlgorithm());
}

TEST(OptimalBST_RebuildWithoutExplicitClear)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    std::vector<int> values1 = {1, 2, 3};
    tree.BuildTree(values1);
    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(3));

    std::vector<int> values2 = {10, 20};
    tree.BuildTree(values2);
    ASSERT_EQ(tree.Size(), static_cast<std::size_t>(2));
    ASSERT_TRUE(tree.VerifyAlgorithm());
}

TEST(OptimalBST_MoveSemantics)
{
    FilmLibrary::OptimalBST<int, int> tree1([](const int& v) { return v; });

    std::vector<int> values = {10, 20, 30};
    tree1.BuildTree(values);

    FilmLibrary::OptimalBST<int, int> tree2(std::move(tree1));
    ASSERT_EQ(tree2.Size(), static_cast<std::size_t>(3));
    ASSERT_TRUE(tree2.VerifyAlgorithm());
    ASSERT_TRUE(tree1.Empty());

    FilmLibrary::OptimalBST<int, int> tree3([](const int& v) { return v; });
    tree3 = std::move(tree2);
    ASSERT_EQ(tree3.Size(), static_cast<std::size_t>(3));
    ASSERT_TRUE(tree3.VerifyAlgorithm());
    ASSERT_TRUE(tree2.Empty());
}

TEST(OptimalBST_VerifyAlgorithm_AllCases)
{
    FilmLibrary::OptimalBST<int, int> tree([](const int& v) { return v; });

    // все одинаковые веса
    tree.BuildTree({1, 2, 3, 4, 5}, {1, 1, 1, 1, 1});
    ASSERT_TRUE(tree.VerifyAlgorithm());

    // возрастающие веса
    tree.BuildTree({1, 2, 3, 4, 5}, {1, 2, 3, 4, 5});
    ASSERT_TRUE(tree.VerifyAlgorithm());

    // убывающие веса
    tree.BuildTree({1, 2, 3, 4, 5}, {5, 4, 3, 2, 1});
    ASSERT_TRUE(tree.VerifyAlgorithm());

    // один большой вес
    tree.BuildTree({1, 2, 3, 4, 5}, {1, 1, 100, 1, 1});
    ASSERT_TRUE(tree.VerifyAlgorithm());

    // 15 узлов
    tree.BuildTree(
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {5, 3, 8, 1, 7, 2, 9, 4, 6, 10, 3, 5, 2, 8, 1}
    );
    ASSERT_TRUE(tree.VerifyAlgorithm());
}