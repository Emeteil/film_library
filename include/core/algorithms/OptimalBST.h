#pragma once

/// @file OptimalBST.h
/// @brief Самописное дерево оптимального поиска (алгоритм Кнута).
///
/// @tparam T Тип хранимого значения (обычно Movie* или Actor*).
/// @tparam Key Тип ключа (std::string, int, double и т.д.).
///
/// Дерево строится методом динамического программирования, обеспечивая
/// минимальное средневзвешенное время поиска. Хранит СЫРЫЕ указатели -
/// время жизни объектов управляется вектором std::unique_ptr в менеджере.

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <vector>

namespace FilmLibrary
{
    template <typename T, typename Key>
    class OptimalBST
    {
        public:
            /// @brief Функция-экстрактор ключа из значения.
            using KeyExtractor = std::function<Key(const T&)>;

            /// @param keyExtractor  Функция, извлекающая ключ из значения.
            OptimalBST(KeyExtractor keyExtractor);
            ~OptimalBST();

            OptimalBST(const OptimalBST&) = delete;
            OptimalBST& operator=(const OptimalBST&) = delete;

            OptimalBST(OptimalBST&& other);
            OptimalBST& operator=(OptimalBST&& other);

            /// @brief Построить оптимальное дерево из коллекции значений.
            ///
            /// Ключи извлекаются через keyExtractor, группируются по уникальным ключам.
            /// Вес каждого уникального ключа = количество значений с этим ключом.
            void BuildTree(const std::vector<T>& values);

            /// @brief Построить оптимальное дерево из явных ключей и весов.
            ///
            /// Ключи ДОЛЖНЫ быть отсортированы по возрастанию.
            /// Значения (T) не сохраняются - только структура дерева.
            void BuildTree(const std::vector<Key>& keys, const std::vector<int>& weights);

            /// @brief Очистить дерево (удалить все узлы и матрицы).
            void Clear();

            /// @brief Найти все значения с заданным ключом.
            std::vector<T> Find(const Key& key) const;

            /// @brief Найти все значения, ключ которых начинается с prefix.
            ///        Применимо только для строковых ключей.
            std::vector<T> FindByPrefix(const Key& prefix) const;

            /// @brief Найти все значения с ключом в диапазоне [low, high].
            std::vector<T> FindInRange(const Key& low, const Key& high) const;

            /// @brief Получить все значения при обходе in-order.
            std::vector<T> InOrderTraversal() const;

            /// @return Количество узлов (уникальных ключей) в дереве.
            std::size_t Size() const;

            /// @return true, если дерево пустое.
            bool Empty() const;

            /// @return Высота дерева.
            int Height() const;

            /// @return Контрольная сумма (сумма всех весов в дереве).
            int CheckSum() const;

            /// @return Средневзвешенная высота дерева.
            double AverageWeightedHeight() const;

            /// @brief Проверить корректность алгоритма:
            ///        AP[0,n] / AW[0,n] должно быть ≈ AverageWeightedHeight().
            /// @return true если значения совпадают (с точностью 0.0001).
            bool VerifyAlgorithm() const;

            /// @brief Доступ к матрице AW (для тестирования).
            const std::vector<std::vector<int>>& GetAW() const;

            /// @brief Доступ к матрице AP (для тестирования).
            const std::vector<std::vector<int>>& GetAP() const;

            /// @brief Доступ к матрице AR (для тестирования).
            const std::vector<std::vector<int>>& GetAR() const;

        private:
            struct Node
            {
                Key key;
                std::vector<T> values;
                int weight = 0;
                Node* left = nullptr;
                Node* right = nullptr;
            };

            Node* root = nullptr;
            std::size_t nodeCount = 0;
            int n = 0;
            KeyExtractor keyExtractor;

            std::vector<std::vector<int>> matrixAW;
            std::vector<std::vector<int>> matrixAP;
            std::vector<std::vector<int>> matrixAR;

            /// @brief Вычислить матрицы AW, AP, AR.
            void ComputeMatrices(const std::vector<int>& weights);

            /// @brief Рекурсивное построение дерева из матрицы AR.
            void BuildTreeRecursive(Node** p, const std::vector<Key>& keys, const std::vector<std::vector<T>>& allValues, const std::vector<int>& weights, int L, int R);

            void DestroyTree(Node* node);
            int HeightRecursive(Node* node) const;
            int CheckSumRecursive(Node* node) const;

            void CalculateWeightedHeight(Node* node, int currentHeight, int& totalWeightedHeight, int& totalWeight) const;

            void FindNode(Node* node, const Key& key, std::vector<T>& result) const;

            void FindByPrefixNode(Node* node, const Key& prefix, std::vector<T>& result) const;

            void FindInRangeNode(Node* node, const Key& low, const Key& high, std::vector<T>& result) const;

            void InOrderNode(Node* node, std::vector<T>& result) const;
    };
}

#include "../../../src/core/algorithms/OptimalBST.cpp"