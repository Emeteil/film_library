add_library(film_library_core STATIC
    algorithms/QuickSort.cpp
    algorithms/BinarySearch.cpp
    algorithms/RabinKarp.cpp

    storage/CsvParser.cpp
    storage/MovieCsvMapper.cpp
    storage/ActorCsvMapper.cpp

    index/IndexManager.cpp
    index/SortCache.cpp

    DataManager.cpp
    ActorManager.cpp

    utils/Logger.cpp
)

target_include_directories(film_library_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
