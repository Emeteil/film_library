/// @file SortCache.cpp
/// @brief Реализация кэша сортировки.
#include "core/index/SortCache.h"
#include "core/algorithms/QuickSort.h"
namespace FilmLibrary
{
    SortCache::SortCache() = default;
    SortCache::~SortCache() = default;

    const std::vector<Movie*>& SortCache::GetSorted(
        const std::string& key,
        const std::vector<std::unique_ptr<Movie>>& movies,
        MovieComparator comp)
    {
        // 1. Проверяем, есть ли готовый результат в кэше.
        auto it = cache.find(key);
        if (it != cache.end())
            return it->second;

        // 2. Кэша нет — строим вектор сырых указателей из хранилища.
        std::vector<Movie*> sorted;
        sorted.reserve(movies.size());
        for (const auto& movie : movies)
            sorted.push_back(movie.get());

        // 3. Сортируем через QuickSort проекта.
        QuickSort::Sort(sorted, comp);

        // 4. Сохраняем в кэш и возвращаем стабильную ссылку.
        auto result = cache.emplace(key, std::move(sorted));
        return result.first->second;
    }

    void SortCache::Invalidate()
    {
        cache.clear();
    }

    void SortCache::Invalidate(const std::string& key)
    {
        cache.erase(key);
    }
}