#pragma once

/// @file AvlTree.h
/// @brief Самописное АВЛ-дерево для индексации фильмов по произвольному ключу.
///
/// @tparam T Тип хранимого значения (обычно Movie*).
/// @tparam Key Тип ключа (std::string, int, double и т.д.).
///
/// Дерево хранит СЫРЫЕ указатели - время жизни объектов управляется
/// вектором std::unique_ptr<Movie> в DataManager.

#include <functional>
#include <vector>

namespace FilmLibrary
{
    template <typename T, typename Key>
    class AvlTree
    {
        public:
            /// @brief Функция-экстрактор ключа из значения.
            using KeyExtractor = std::function<Key(const T&)>;

            /// @param keyExtractor  Функция, извлекающая ключ из значения.
            AvlTree(KeyExtractor keyExtractor);
            ~AvlTree();

            AvlTree(const AvlTree&) = delete;
            AvlTree& operator=(const AvlTree&) = delete;

            AvlTree(AvlTree&& other);
            AvlTree& operator=(AvlTree&& other);

            /// @brief Вставить значение в дерево.
            void Insert(const T& value);

            /// @brief Удалить значение по ключу.
            /// @return true, если элемент был найден и удалён.
            bool Remove(const Key& key);

            /// @brief Очистить дерево (удалить все узлы).
            void Clear();

            /// @brief Найти все значения с заданным ключом.
            std::vector<T> Find(const Key& key) const;

            /// @brief Найти все значения, ключ которых начинается с заданного префикса.
            ///        Применимо только для строковых ключей.
            std::vector<T> FindByPrefix(const Key& prefix) const;

            /// @brief Найти все значения с ключом в диапазоне [low, high].
            std::vector<T> FindInRange(const Key& low, const Key& high) const;

            /// @brief Получить все значения при обходе in-order.
            std::vector<T> InOrderTraversal() const;

            /// @return Количество элементов в дереве.
            std::size_t Size() const;

            /// @return true, если дерево пустое.
            bool Empty() const;

        private:
            struct Node
            {
                Key key;
                T value;
                int height = 1;
                Node* left = nullptr;
                Node* right = nullptr;
            };

            Node* root = nullptr;
            std::size_t size = 0;
            KeyExtractor keyExtractor;

            int Height(Node* node) const;
            int BalanceFactor(Node* node) const;
            void UpdateHeight(Node* node);

            Node* RotateRight(Node* node);
            Node* RotateLeft(Node* node);
            Node* Balance(Node* node);

            Node* InsertNode(Node* node, const Key& key, const T& value);
            Node* RemoveNode(Node* node, const Key& key, bool& removed);
            Node* FindMin(Node* node) const;
            Node* RemoveMin(Node* node);

            void FindNode(Node* node, const Key& key, std::vector<T>& result) const;
            void FindByPrefixNode(Node* node, const Key& prefix, std::vector<T>& result) const;
            void FindInRangeNode(Node* node, const Key& low, const Key& high, std::vector<T>& result) const;
            void InOrderNode(Node* node, std::vector<T>& result) const;
            void DestroyTree(Node* node);
    };

    template <typename T, typename Key>
    AvlTree<T, Key>::AvlTree(KeyExtractor keyExtractor) : keyExtractor(std::move(keyExtractor))
    {
    }

    template <typename T, typename Key>
    AvlTree<T, Key>::~AvlTree()
    {
        Clear();
    }

    template <typename T, typename Key>
    AvlTree<T, Key>::AvlTree(AvlTree&& other) : root(other.root), size(other.size), keyExtractor(std::move(other.keyExtractor))
    {
        other.root = nullptr;
        other.size = 0;
    }

    template <typename T, typename Key>
    AvlTree<T, Key>& AvlTree<T, Key>::operator=(AvlTree&& other)
    {
        if (this != &other)
        {
            Clear();
            root = other.root;
            size = other.size;
            keyExtractor = std::move(other.keyExtractor);
            other.root = nullptr;
            other.size = 0;
        }
        return *this;
    }

    template <typename T, typename Key>
    void AvlTree<T, Key>::Insert(const T& value)
    {
        // TODO: Реализовать вставку.
        Key key = keyExtractor(value);
        root = InsertNode(root, key, value);
        ++size;
    }

    template <typename T, typename Key>
    bool AvlTree<T, Key>::Remove(const Key& key)
    {
        // TODO: Реализовать удаление.
        bool removed = false;
        root = RemoveNode(root, key, removed);
        if (removed) --size;
        return removed;
    }

    template <typename T, typename Key>
    void AvlTree<T, Key>::Clear()
    {
        DestroyTree(root);
        root = nullptr;
        size = 0;
    }

    template <typename T, typename Key>
    std::vector<T> AvlTree<T, Key>::Find(const Key& key) const
    {
        // TODO: Реализовать поиск.
        std::vector<T> result;
        FindNode(root, key, result);
        return result;
    }

    template <typename T, typename Key>
    std::vector<T> AvlTree<T, Key>::FindByPrefix(const Key& prefix) const
    {
        // TODO: Реализовать поиск по префиксу.
        std::vector<T> result;
        FindByPrefixNode(root, prefix, result);
        return result;
    }

    template <typename T, typename Key>
    std::vector<T> AvlTree<T, Key>::FindInRange(const Key& low, const Key& high) const
    {
        // TODO: Реализовать поиск в диапазоне.
        std::vector<T> result;
        FindInRangeNode(root, low, high, result);
        return result;
    }

    template <typename T, typename Key>
    std::vector<T> AvlTree<T, Key>::InOrderTraversal() const
    {
        // TODO: Реализовать обход.
        std::vector<T> result;
        InOrderNode(root, result);
        return result;
    }

    template <typename T, typename Key>
    std::size_t AvlTree<T, Key>::Size() const
    {
        return size;
    }

    template <typename T, typename Key>
    bool AvlTree<T, Key>::Empty() const
    {
        return size == 0;
    }

    template <typename T, typename Key>
    int AvlTree<T, Key>::Height(Node* node) const
    {
        return node ? node->height : 0;
    }

    template <typename T, typename Key>
    int AvlTree<T, Key>::BalanceFactor(Node* node) const
    {
        return node ? Height(node->left) - Height(node->right) : 0;
    }

    template <typename T, typename Key>
    void AvlTree<T, Key>::UpdateHeight(Node* node)
    {
        if (node)
        {
            int lh = Height(node->left);
            int rh = Height(node->right);
            node->height = (lh > rh ? lh : rh) + 1;
        }
    }

    template <typename T, typename Key>
    typename AvlTree<T, Key>::Node* AvlTree<T, Key>::RotateRight(Node* node)
    {
        // TODO: Реализовать правый поворот.
        (void)node;
        return node;
    }

    template <typename T, typename Key>
    typename AvlTree<T, Key>::Node* AvlTree<T, Key>::RotateLeft(Node* node)
    {
        // TODO: Реализовать левый поворот.
        (void)node;
        return node;
    }

    template <typename T, typename Key>
    typename AvlTree<T, Key>::Node* AvlTree<T, Key>::Balance(Node* node)
    {
        // TODO: Реализовать балансировку.
        (void)node;
        return node;
    }

    template <typename T, typename Key>
    typename AvlTree<T, Key>::Node* AvlTree<T, Key>::InsertNode(Node* node, const Key& key, const T& value)
    {
        // TODO: Реализовать рекурсивную вставку.
        (void)key;
        (void)value;
        if (!node)
        {
            Node* n = new Node();
            n->key = key;
            n->value = value;
            return n;
        }
        return node;
    }

    template <typename T, typename Key>
    typename AvlTree<T, Key>::Node* AvlTree<T, Key>::RemoveNode(Node* node, const Key& key, bool& removed)
    {
        // TODO: Реализовать рекурсивное удаление.
        (void)key;
        (void)removed;
        return node;
    }

    template <typename T, typename Key>
    typename AvlTree<T, Key>::Node* AvlTree<T, Key>::FindMin(Node* node) const
    {
        if (!node) return nullptr;
        while (node->left) node = node->left;
        return node;
    }

    template <typename T, typename Key>
    typename AvlTree<T, Key>::Node* AvlTree<T, Key>::RemoveMin(Node* node)
    {
        // TODO: Реализовать удаление минимального узла.
        (void)node;
        return node;
    }

    template <typename T, typename Key>
    void AvlTree<T, Key>::FindNode(Node* node, const Key& key, std::vector<T>& result) const
    {
        // TODO: Реализовать поиск по ключу.
        (void)node;
        (void)key;
        (void)result;
    }

    template <typename T, typename Key>
    void AvlTree<T, Key>::FindByPrefixNode(Node* node, const Key& prefix, std::vector<T>& result) const
    {
        // TODO: Реализовать поиск по префиксу.
        (void)node;
        (void)prefix;
        (void)result;
    }

    template <typename T, typename Key>
    void AvlTree<T, Key>::FindInRangeNode(Node* node, const Key& low, const Key& high, std::vector<T>& result) const
    {
        // TODO: Реализовать поиск в диапазоне.
        (void)node;
        (void)low;
        (void)high;
        (void)result;
    }

    template <typename T, typename Key>
    void AvlTree<T, Key>::InOrderNode(Node* node, std::vector<T>& result) const
    {
        // TODO: Реализовать in-order обход.
        (void)node;
        (void)result;
    }

    template <typename T, typename Key>
    void AvlTree<T, Key>::DestroyTree(Node* node)
    {
        if (!node) return;
        DestroyTree(node->left);
        DestroyTree(node->right);
        delete node;
    }
}