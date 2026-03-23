#ifndef CORE_ALGORITHMS_OPTIMALBST_CPP
#define CORE_ALGORITHMS_OPTIMALBST_CPP

/// @file OptimalBST.cpp
/// @brief Реализация дерева оптимального поиска.

#include "core/algorithms/OptimalBST.h"

namespace FilmLibrary
{
    inline std::size_t sz(std::size_t base, int offset = 0)
    {
        return static_cast<std::size_t>(static_cast<int>(base) + offset);
    }

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

        n = sortedKeys.size();
        ComputeMatrices(weights);
        BuildTreeRecursive(&root, sortedKeys, sortedValues, weights, 0, n);
        nodeCount = n;
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::ComputeMatrices(const std::vector<int>& weights)
    {
        matrixAW.assign(n + 1, std::vector<int>(n + 1, 0));
        matrixAP.assign(n + 1, std::vector<int>(n + 1, 0));
        matrixAR.assign(n + 1, std::vector<std::size_t>(n + 1, 0));

        for (std::size_t i = 0; i <= n; i++)
        {
            matrixAW[i][i] = 0;
            for (std::size_t j = i + 1; j <= n; j++)
            {
                matrixAW[i][j] = matrixAW[i][sz(j, -1)] + weights[sz(j, -1)];
            }
        }

        for (std::size_t i = 0; i < n; i++)
        {
            std::size_t j = i + 1;
            matrixAP[i][j] = matrixAW[i][j];
            matrixAR[i][j] = j;
        }

        for (std::size_t h = 2; h <= n; h++)
        {
            for (std::size_t i = 0; i <= n - h; i++)
            {
                std::size_t j = i + h;
                std::size_t m = matrixAR[i][sz(j, -1)];
                int minVal = matrixAP[i][sz(m, -1)] + matrixAP[m][j];

                for (std::size_t k = m + 1; k <= matrixAR[i + 1][j]; k++)
                {
                    int x = matrixAP[i][sz(k, -1)] + matrixAP[k][j];
                    if (x < minVal)
                    {
                        m = k;
                        minVal = x;
                    }
                }

                matrixAP[i][j] = minVal + matrixAW[i][j];
                matrixAR[i][j] = m;
            }
        }
    }

    template <typename T, typename Key>
    void OptimalBST<T, Key>::BuildTreeRecursive(Node** p, const std::vector<Key>& keys, const std::vector<std::vector<T>>& allValues, const std::vector<int>& weights, std::size_t L, std::size_t R)
    {
        if (L < R)
        {
            std::size_t k = matrixAR[L][R];
            *p = new Node();
            (*p)->key = keys[sz(k, -1)];
            (*p)->values = allValues[sz(k, -1)];
            (*p)->weight = weights[sz(k, -1)];
            (*p)->left = nullptr;
            (*p)->right = nullptr;

            BuildTreeRecursive(&(*p)->left, keys, allValues, weights, L, sz(k, -1));
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
}

#endif