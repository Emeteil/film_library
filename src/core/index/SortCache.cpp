/// @file SortCache.cpp
/// @brief Реализация кэша сортировки.

#include "core/index/SortCache.h"
#include "core/algorithms/QuickSort.h"

namespace FilmLibrary
{
    SortCache::SortCache() = default;
    SortCache::~SortCache() = default;

    const std::vector<Movie*>& SortCache::GetSorted(const std::string& key, const std::vector<std::unique_ptr<Movie>>& movies, MovieComparator comp)
    {
        // TODO: Реализовать ленивую инициализацию.
        //
        // 1. Проверить, есть ли ключ в cache.
        // 2. Если нет - создать вектор Movie* из movies,
        //    отсортировать через QuickSort::Sort() с comp,
        //    сохранить в cache[key].
        // 3. Вернуть ссылку на cache[key].

        (void)movies;
        (void)comp;
        return cache[key];
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
