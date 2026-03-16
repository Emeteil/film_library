cmake_minimum_required(VERSION 3.16)
project(FilmLibrary LANGUAGES CXX)

option(FILM_LIBRARY_BUILD_TESTS "Build unit tests" ON)

add_subdirectory(third_party)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_subdirectory(src/core)
add_subdirectory(src/gui)
add_subdirectory(src/app)

if(FILM_LIBRARY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()