/// @file test_avl_tree.cpp
/// @brief Юнит-тесты для АВЛ-дерева.

#include "TestFramework.h"
#include "core/algorithms/AvlTree.h"

#include <string>

// TODO: Реализовать тесты после имплементации AvlTree.

TEST(AvlTree_InsertAndFind)
{
    // TODO: Создать AvlTree<int, int> с простым экстрактором.
    //       Вставить несколько значений.
    //       Проверить, что Find() находит все вставленные.
}

TEST(AvlTree_Remove)
{
    // TODO: Вставить значения, удалить часть.
    //       Проверить, что удалённые не находятся, а остальные - находятся.
}

TEST(AvlTree_BalanceAfterInsert)
{
    // TODO: Вставить значения в порядке, требующем ротаций.
    //       Проверить, что Size() корректен и Find() работает.
}

TEST(AvlTree_FindByPrefix)
{
    // TODO: Создать AvlTree<std::string, std::string>.
    //       Вставить строки, проверить FindByPrefix().
}

TEST(AvlTree_FindInRange)
{
    // TODO: Создать AvlTree<int, int>.
    //       Вставить значения, проверить FindInRange(low, high).
}

TEST(AvlTree_Empty)
{
    // TODO: Проверить поведение пустого дерева.
    //       Empty() == true, Size() == 0, Find() == empty.
}
