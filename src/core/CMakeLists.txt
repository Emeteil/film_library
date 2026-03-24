cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(film_library_core STATIC
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
