add_library(film_library_core STATIC
    algorithms/QuickSort.cpp
    algorithms/BinarySearch.cpp
    algorithms/RabinKarp.cpp

    storage/CsvParser.cpp

    index/IndexManager.cpp
    index/SortCache.cpp

    DataManager.cpp

    utils/Logger.cpp
)

target_include_directories(film_library_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
