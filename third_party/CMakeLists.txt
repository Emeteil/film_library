cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

if(WIN32 OR MINGW OR CYGWIN)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WIN32 ON CACHE BOOL "" FORCE)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw/CMakeLists.txt")
    if(WIN32 OR MINGW OR CYGWIN)
        set(CMAKE_DL_LIBS "")
    endif()
    add_subdirectory(glfw)
else()
    message(WARNING "Run: git submodule update --init --recursive")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h")
    set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

    add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )

    target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )

    target_compile_definitions(imgui PUBLIC IMGUI_USE_WCHAR32)

    target_link_libraries(imgui PUBLIC glfw)

    if(WIN32 OR MINGW OR CYGWIN)
        target_link_libraries(imgui PUBLIC opengl32)
    else()
        find_package(OpenGL REQUIRED)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7l|aarch64|arm64")
            target_link_libraries(imgui PUBLIC OpenGL::GLES2)
        else()
            target_link_libraries(imgui PUBLIC OpenGL::GL)
        endif()
    endif()

    set(IMGUI_EXT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui_extensions")
    if(EXISTS "${IMGUI_EXT_DIR}/ImAnim/im_anim.cpp")
        target_sources(imgui PRIVATE
            ${IMGUI_EXT_DIR}/ImAnim/im_anim.cpp
        )
        target_include_directories(imgui PUBLIC
            ${IMGUI_EXT_DIR}/ImAnim
        )
    endif()
else()
    message(WARNING "Run: git submodule update --init --recursive")
endif()