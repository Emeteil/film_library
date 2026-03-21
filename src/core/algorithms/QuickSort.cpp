/// @file QuickSort.cpp
/// @brief Реализация быстрой сортировки (Хоара).

#include "core/algorithms/QuickSort.h"

namespace FilmLibrary
{
    // NOTE: QuickSort - шаблонный класс. Все методы определены в заголовочном файле.
    // если потребуется сократить время компиляции.
    
    // TODO: Реализовать быструю сортировку.
    // template void QuickSort::Sort<Movie*, ...>(std::vector<Movie*>&, ...);

    template <typename T, typename Comparator>
    void QuickSort::Sort(std::vector<T>& data, Comparator comparator)
    {
        SortRange(data, 0, data.size() - 1, comparator);
    }

    template <typename T, typename Comparator>
    void QuickSort::SortRange(std::vector<T>& data, int low, int high, Comparator& comparator)
    {
        while (low < high)
        {
            std::pair<int, int> p = Partition(data, low, high, comparator);
            int i = p.first;
            int j = p.second;
            if (j - low < high - i)
            {
                SortRange(data, low, j, comparator);
                low = i;
            }
            else
            {
                SortRange(data, i, high, comparator);
                high = j;
            }
        }
        
    }

    template <typename T, typename Comparator>
    std::pair<int, int> QuickSort::Partition(std::vector<T>& data, int low, int high, Comparator& comparator)
    {
        int pivot = data[MedianOfThree(data, low, high)];
        int i = low;
        int j = high;
        
        while (i < j)
        {
            while (i <= high)
            {
                if (!comparator(data[i], pivot)) break;
                i++;
            }
            while (j >= low)
            {
                if (comparator(data[j], pivot)) break;
                j--;
            }
            
            if (i <= j) 
            {
                // int temp = data[i];
                // data[i] = data[j];
                // data[j] = temp;
                std::swap(data[i], data[j]);
            }
            else break;
        }
        return std::pair(i, j);
    }

    template <typename T>
    int QuickSort::MedianOfThree(std::vector<T>& data, int low, int high)
    {
        return low;
    }
}


/*

class QuickSortFuncs
{
    // QuickSort(L, R)
    // x := a[L], i := L, j := R
    // DO (i < j)
    //     DO (a[i] < x) i := i + 1 OD
    //     DO (a[j] > x) j := j - 1 OD
    //     IF (i <= j) a[i] ↔️ a[j], i := i + 1, j := j - 1 FI
    // OD
    // IF (L < j) QuickSort(L, j) FI
    // IF (i < R) QuickSort(i, R) FI

    // QuickSort(L, R) (Первая версия)
    // <Разделение на две части>
    // IF (L < j) QuickSort(L, j) FI
    // IF (i < R) QuickSort(i, R) FI

    // QuickSort(L, R) (Вторая версия)
    // DO (L < R)
    //     <Разделение на две части>
    //     IF (j - L < R - i) QuickSort(L, j), L := i
    //     ELSE QuickSort(i, R), R := j FI
    // OD

    public static (int, int) QuickSortStandard(int[] A)
    {
        int comparisons = 0, swaps = 0;
        int? maxDepth = 0;
        if (A.Length > 0)
            _QuickSortStandard(A, 0, A.Length - 1, ref comparisons, ref swaps, ref maxDepth, 0);
        return (comparisons, swaps);
    }

    public static (int, int) QuickSortImproved(int[] A)
    {
        int comparisons = 0, swaps = 0;
        int? maxDepth = 0;
        if (A.Length > 0)
            _QuickSortImproved(A, 0, A.Length - 1, ref comparisons, ref swaps, ref maxDepth, 0);
        return (comparisons, swaps);
    }

    public static (int, int) QuickSortStandardWithDepth(int[] A, ref int? maxDepth)
    {
        int comparisons = 0, swaps = 0;
        if (A.Length > 0)
            _QuickSortStandard(A, 0, A.Length - 1, ref comparisons, ref swaps, ref maxDepth, 0);
        return (comparisons, swaps);
    }

    public static (int, int) QuickSortImprovedWithDepth(int[] A, ref int? maxDepth)
    {
        int comparisons = 0, swaps = 0;
        if (A.Length > 0)
            _QuickSortImproved(A, 0, A.Length - 1, ref comparisons, ref swaps, ref maxDepth, 0);
        return (comparisons, swaps);
    }

    private static void _QuickSortStandard(int[] A, int L, int R, ref int C, ref int M, ref int? maxDepth, int currentDepth)
    {
        if (currentDepth > maxDepth)
            maxDepth = currentDepth;

        // if (L >= R) return;

        var (i, j) = Partition(A, L, R, ref C, ref M);

        if (L < j)
            _QuickSortStandard(A, L, j, ref C, ref M, ref maxDepth, currentDepth + 1);
        if (i < R)
            _QuickSortStandard(A, i, R, ref C, ref M, ref maxDepth, currentDepth + 1);
    }

    private static void _QuickSortImproved(int[] A, int L, int R, ref int C, ref int M, ref int? maxDepth, int currentDepth)
    {
        if (currentDepth > maxDepth)
            maxDepth = currentDepth;

        while (L < R)
        {
            var (i, j) = Partition(A, L, R, ref C, ref M);

            if (j - L < R - i)
            {
                _QuickSortImproved(A, L, j, ref C, ref M, ref maxDepth, currentDepth + 1);
                L = i;
            }
            else
            {
                _QuickSortImproved(A, i, R, ref C, ref M, ref maxDepth, currentDepth + 1);
                R = j;
            }
        }
    }

    private static (int i, int j) Partition(int[] A, int L, int R, ref int C, ref int M)
    {
        int x = A[L];
        int i = L;
        int j = R;

        while (i < j)
        {
            while (i <= R)
            {
                C++;
                if (A[i] >= x) break;
                i++;
            }

            while (j >= L)
            {
                C++;
                if (A[j] <= x) break;
                j--;
            }

            if (i <= j)
            {
                int temp = A[i];
                A[i] = A[j];
                A[j] = temp;
                M++;
                i++;
                j--;
            }
            else break;
        }

        return (i, j);
    }
}

*/