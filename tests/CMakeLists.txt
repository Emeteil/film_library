cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(film_library_tests
    main_test.cpp
    test_optimal_bst.cpp
    test_quick_sort.cpp
    test_binary_search.cpp
    test_rabin_karp.cpp
    test_csv_parser.cpp
    test_sort_cache.cpp
    test_data_manager.cpp
    test_index_manager.cpp
    test_app_controller.cpp
)

target_link_libraries(film_library_tests PRIVATE
    film_library_core
)

target_include_directories(film_library_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

add_test(NAME FilmLibraryTests COMMAND film_library_tests)
