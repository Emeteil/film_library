set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw/CMakeLists.txt")
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

    target_link_libraries(imgui PUBLIC glfw)

    find_package(OpenGL REQUIRED)
    target_link_libraries(imgui PUBLIC OpenGL::GL)

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