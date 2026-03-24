cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(FilmLibrary main.cpp)

target_link_libraries(FilmLibrary PRIVATE
    film_library_gui
    film_library_core
)
