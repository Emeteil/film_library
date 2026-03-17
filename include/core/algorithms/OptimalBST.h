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

            /// @brief Построить оптимальное дерево из явных ключей и весов (аналог C#).
            ///
            /// Ключи ДОЛЖНЫ быть отсортированы по возрастанию.
            /// Значения (T) не сохраняются — только структура дерева.
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

            /// @brief Вычислить матрицы AW, AP, AR (алгоритм Кнута).
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

    template <typename T, typename Key>
    OptimalBST<T, Key>::OptimalBST(KeyExtractor keyExtractor) : keyExtractor(keyExtractor)
    {
    }

    template <typename T, typename Key>
    OptimalBST<T, Key>::~OptimalBST()
    {
        Clear();
    }

    template <typename T, typename Key>
    OptimalBST<T, Key>::OptimalBST(OptimalBST&& other) : root(other.root), nodeCount(other.nodeCount), n(other.n), keyExtractor(other.keyExtractor), matrixAW(other.matrixAW), matrixAP(other.matrixAP), matrixAR(other.matrixAR)
    {
        other.root = nullptr;
        other.nodeCount = 0;
        other.n = 0;
    }

    template <typename T, typename Key>
    OptimalBST<T, Key>& OptimalBST<T, Key>::operator=(OptimalBST&& other)
    {
        if (this != &other)
        {
            Clear();
            root = other.root;
            nodeCount = other.nodeCount;
            n = other.n;
            keyExtractor = other.keyExtractor;
            matrixAW = other.matrixAW;
            matrixAP = other.matrixAP;
            matrixAR = other.matrixAR;
            other.root = nullptr;
            other.nodeCount = 0;
            other.n = 0;
        }
        return *this;
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::BuildTree(const std::vector<T>& values)
    {
        Clear();
        if (values.empty())
            return;

        std::map<Key, std::vector<T>> grouped;
        for (const auto& v : values)
        {
            grouped[keyExtractor(v)].push_back(v);
        }

        std::vector<Key> sortedKeys;
        std::vector<std::vector<T>> sortedValues;
        std::vector<int> weights;

        for (auto& pair : grouped)
        {
            sortedKeys.push_back(pair.first);
            weights.push_back(static_cast<int>(pair.second.size()));
            sortedValues.push_back(pair.second);
        }

        n = static_cast<int>(sortedKeys.size());
        ComputeMatrices(weights);
        BuildTreeRecursive(&root, sortedKeys, sortedValues, weights, 0, n);
        nodeCount = static_cast<std::size_t>(n);
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::BuildTree(const std::vector<Key>& keys, const std::vector<int>& weights)
    {
        Clear();
        if (keys.empty())
            return;

        n = static_cast<int>(keys.size());

        std::vector<std::vector<T>> emptyValues(static_cast<std::size_t>(n));

        ComputeMatrices(weights);
        BuildTreeRecursive(&root, keys, emptyValues, weights, 0, n);
        nodeCount = static_cast<std::size_t>(n);
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::ComputeMatrices(const std::vector<int>& weights)
    {
        matrixAW.assign(static_cast<std::size_t>(n + 1), std::vector<int>(static_cast<std::size_t>(n + 1), 0));
        matrixAP.assign(static_cast<std::size_t>(n + 1), std::vector<int>(static_cast<std::size_t>(n + 1), 0));
        matrixAR.assign(static_cast<std::size_t>(n + 1), std::vector<int>(static_cast<std::size_t>(n + 1), 0));

        for (int i = 0; i <= n; i++)
        {
            matrixAW[static_cast<std::size_t>(i)][static_cast<std::size_t>(i)] = 0;
            for (int j = i + 1; j <= n; j++)
            {
                matrixAW[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] = matrixAW[static_cast<std::size_t>(i)][static_cast<std::size_t>(j - 1)] + weights[static_cast<std::size_t>(j - 1)];
            }
        }

        for (int i = 0; i < n; i++)
        {
            int j = i + 1;
            matrixAP[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] = matrixAW[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)];
            matrixAR[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] = j;
        }

        for (int h = 2; h <= n; h++)
        {
            for (int i = 0; i <= n - h; i++)
            {
                int j = i + h;
                int m = matrixAR[static_cast<std::size_t>(i)][static_cast<std::size_t>(j - 1)];
                int minVal = matrixAP[static_cast<std::size_t>(i)][static_cast<std::size_t>(m - 1)] + matrixAP[static_cast<std::size_t>(m)][static_cast<std::size_t>(j)];

                for (int k = m + 1; k <= matrixAR[static_cast<std::size_t>(i + 1)][static_cast<std::size_t>(j)]; k++)
                {
                    int x = matrixAP[static_cast<std::size_t>(i)][static_cast<std::size_t>(k - 1)] + matrixAP[static_cast<std::size_t>(k)][static_cast<std::size_t>(j)];
                    if (x < minVal)
                    {
                        m = k;
                        minVal = x;
                    }
                }

                matrixAP[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] = minVal + matrixAW[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)];
                matrixAR[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] = m;
            }
        }
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::BuildTreeRecursive(Node** p, const std::vector<Key>& keys, const std::vector<std::vector<T>>& allValues, const std::vector<int>& weights, int L, int R)
    {
        if (L < R)
        {
            int k = matrixAR[static_cast<std::size_t>(L)][static_cast<std::size_t>(R)];
            *p = new Node();
            (*p)->key = keys[static_cast<std::size_t>(k - 1)];
            (*p)->values = allValues[static_cast<std::size_t>(k - 1)];
            (*p)->weight = weights[static_cast<std::size_t>(k - 1)];
            (*p)->left = nullptr;
            (*p)->right = nullptr;

            BuildTreeRecursive(&(*p)->left, keys, allValues, weights, L, k - 1);
            BuildTreeRecursive(&(*p)->right, keys, allValues, weights, k, R);
        }
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::Clear()
    {
        DestroyTree(root);
        root = nullptr;
        nodeCount = 0;
        n = 0;
        matrixAW.clear();
        matrixAP.clear();
        matrixAR.clear();
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::DestroyTree(Node* node)
    {
        if (!node) return;
        DestroyTree(node->left);
        DestroyTree(node->right);
        delete node;
    }

    template <typename T, typename Key>
    std::vector<T> OptimalBST<T, Key>::Find(const Key& key) const
    {
        std::vector<T> result;
        FindNode(root, key, result);
        return result;
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::FindNode(Node* node, const Key& key, std::vector<T>& result) const
    {
        if (!node)
            return;

        if (key < node->key)
            FindNode(node->left, key, result);
        else if (node->key < key)
            FindNode(node->right, key, result);
        else
            result.insert(result.end(), node->values.begin(), node->values.end());
    }

    template <typename T, typename Key>
    std::vector<T> OptimalBST<T, Key>::FindByPrefix(const Key& prefix) const
    {
        std::vector<T> result;
        FindByPrefixNode(root, prefix, result);
        return result;
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::FindByPrefixNode(Node* node, const Key& prefix, std::vector<T>& result) const
    {
        if (!node)
            return;

        bool matches = (node->key.size() >= prefix.size() && node->key.compare(0, prefix.size(), prefix) == 0);

        if (matches)
        {
            result.insert(result.end(), node->values.begin(), node->values.end());
            FindByPrefixNode(node->left, prefix, result);
            FindByPrefixNode(node->right, prefix, result);
        }
        else if (prefix < node->key)
        {
            FindByPrefixNode(node->left, prefix, result);
        }
        else
        {
            FindByPrefixNode(node->right, prefix, result);
        }
    }

    template <typename T, typename Key>
    std::vector<T> OptimalBST<T, Key>::FindInRange(const Key& low, const Key& high) const
    {
        std::vector<T> result;
        FindInRangeNode(root, low, high, result);
        return result;
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::FindInRangeNode(Node* node, const Key& low, const Key& high, std::vector<T>& result) const
    {
        if (!node)
            return;

        if (node->key < low)
        {
            FindInRangeNode(node->right, low, high, result);
        }
        else if (high < node->key)
        {
            FindInRangeNode(node->left, low, high, result);
        }
        else
        {
            result.insert(result.end(), node->values.begin(), node->values.end());
            FindInRangeNode(node->left, low, high, result);
            FindInRangeNode(node->right, low, high, result);
        }
    }

    template <typename T, typename Key>
    std::vector<T> OptimalBST<T, Key>::InOrderTraversal() const
    {
        std::vector<T> result;
        InOrderNode(root, result);
        return result;
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::InOrderNode(Node* node, std::vector<T>& result) const
    {
        if (!node) return;
        InOrderNode(node->left, result);
        result.insert(result.end(), node->values.begin(), node->values.end());
        InOrderNode(node->right, result);
    }

    template <typename T, typename Key>
    std::size_t OptimalBST<T, Key>::Size() const
    {
        return nodeCount;
    }

    template <typename T, typename Key>
    bool OptimalBST<T, Key>::Empty() const
    {
        return nodeCount == 0;
    }

    template <typename T, typename Key>
    int OptimalBST<T, Key>::Height() const
    {
        return HeightRecursive(root);
    }

    template <typename T, typename Key>
    int OptimalBST<T, Key>::HeightRecursive(Node* node) const
    {
        if (!node)
            return 0;

        int lh = HeightRecursive(node->left);
        int rh = HeightRecursive(node->right);
        return 1 + (lh > rh ? lh : rh);
    }

    template <typename T, typename Key>
    int OptimalBST<T, Key>::CheckSum() const
    {
        return CheckSumRecursive(root);
    }

    template <typename T, typename Key>
    int OptimalBST<T, Key>::CheckSumRecursive(Node* node) const
    {
        if (!node)
            return 0;

        return node->weight + CheckSumRecursive(node->left) + CheckSumRecursive(node->right);
    }

    template <typename T, typename Key>
    double OptimalBST<T, Key>::AverageWeightedHeight() const
    {
        int totalWeightedHeight = 0;
        int totalWeight = 0;
        CalculateWeightedHeight(root, 1, totalWeightedHeight, totalWeight);
        return totalWeight == 0 ? 0.0 : static_cast<double>(totalWeightedHeight) / totalWeight;
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::CalculateWeightedHeight(Node* node, int currentHeight, int& totalWeightedHeight, int& totalWeight) const
    {
        if (!node) return;

        totalWeightedHeight += node->weight * currentHeight;
        totalWeight += node->weight;

        CalculateWeightedHeight(node->left, currentHeight + 1, totalWeightedHeight, totalWeight);
        CalculateWeightedHeight(node->right, currentHeight + 1, totalWeightedHeight, totalWeight);
    }

    template <typename T, typename Key>
    bool OptimalBST<T, Key>::VerifyAlgorithm() const
    {
        if (n == 0) return true;

        int aw = matrixAW[0][static_cast<std::size_t>(n)];
        if (aw == 0) return true;

        double ratio = static_cast<double>(matrixAP[0][static_cast<std::size_t>(n)]) / aw;
        double avgHeight = AverageWeightedHeight();

        return std::abs(ratio - avgHeight) < 0.0001;
    }

    template <typename T, typename Key>
    const std::vector<std::vector<int>>& OptimalBST<T, Key>::GetAW() const
    {
        return matrixAW;
    }

    template <typename T, typename Key>
    const std::vector<std::vector<int>>& OptimalBST<T, Key>::GetAP() const
    {
        return matrixAP;
    }

    template <typename T, typename Key>
    const std::vector<std::vector<int>>& OptimalBST<T, Key>::GetAR() const
    {
        return matrixAR;
    }
}
