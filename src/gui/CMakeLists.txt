cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
    add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

add_library(film_library_gui STATIC
    AppWindow.cpp
    AppController.cpp
    widgets/MainWidget.cpp
    widgets/MovieTableWidget.cpp
    widgets/SearchPanel.cpp
    widgets/MovieFormDialog.cpp
    widgets/ConfirmDialog.cpp
    widgets/NotificationManager.cpp
)

target_compile_options(film_library_gui PRIVATE -Wno-unused-variable -Wno-unused-parameter)

target_include_directories(film_library_gui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/imgui_extensions
    ${PROJECT_SOURCE_DIR}/imgui_extensions/ImAnim
)

target_link_libraries(film_library_gui PUBLIC
    film_library_core
    imgui
)

if(WIN32)
    target_link_libraries(film_library_gui PUBLIC urlmon)
endif()

if(APPLE)
    target_compile_definitions(film_library_gui PRIVATE GL_SILENCE_DEPRECATION)
endif()