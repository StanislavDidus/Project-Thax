cmake_minimum_required(VERSION 4.2)

project(ThaxGUI)

set(CMAKE_CXX_STANDARD 23)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Don't build shared libraries." FORCE)

add_subdirectory(externals)

set(HEADER_FILES
        src/Graphics/Graphics.hpp
        src/UI/UI.hpp
        src/Input/Input.hpp
)

set(SOURCE_FILES
        src/main.cpp
)

set(GLAD_FILES
        externals/glad/src/glad.c
)

set(ALL_FILES
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${GLAD_FILES}
)

add_executable(ThaxGUI ${ALL_FILES})


add_custom_target(copy_command
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/copy_files.cmake
)
add_dependencies(ThaxGUI copy_command)

target_include_directories(ThaxGUI PRIVATE
        src
        externals/glad/include
)

#target_link_libraries(GameBoyGUI PUBLIC spdlog::spdlog glfw imgui nlohmann_json::nlohmann_json)
target_link_libraries(ThaxGUI PUBLIC ThaxCore glfw imgui)
