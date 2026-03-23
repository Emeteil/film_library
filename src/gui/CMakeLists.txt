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

target_include_directories(film_library_gui PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(film_library_gui PUBLIC
    film_library_core
    imgui
)

if(WIN32)
    target_link_libraries(film_library_gui PUBLIC urlmon)
endif()
