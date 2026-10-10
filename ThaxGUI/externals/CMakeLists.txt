cmake_minimum_required(VERSION 4.2)

add_subdirectory(glfw EXCLUDE_FROM_ALL)

set(IMGUI_HEADER_FILES
        imgui/imgui.h
        imgui/imgui_internal.h
        imgui/backends/imgui_impl_glfw.h
        imgui/backends/imgui_impl_opengl3.h
        imgui_memory_editor/imgui_memory_editor.h
)

set(IMGUI_SOURCE_FILES
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

set(IMGUI_ALL_FILES
        ${IMGUI_HEADER_FILES}
        ${IMGUI_SOURCE_FILES}
)
add_library(imgui ${IMGUI_ALL_FILES})
target_include_directories(imgui PUBLIC imgui imgui/backends imgui_memory_editor)
target_link_libraries(imgui PUBLIC glfw)