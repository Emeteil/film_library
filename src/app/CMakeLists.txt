add_executable(FilmLibrary main.cpp)

target_link_libraries(FilmLibrary PRIVATE
    film_library_gui
    film_library_core
)
