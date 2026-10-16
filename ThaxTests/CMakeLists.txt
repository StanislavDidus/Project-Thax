cmake_minimum_required(VERSION 4.2)

project(ThaxTests)

set(CMAKE_CXX_STANDARD 23)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Don't build shared libraries." FORCE)

add_subdirectory(externals)

set(HEADER_FILES
)

set(SOURCE_FILES
        src/main.cpp
)

set(ALL_FILES
        ${HEADER_FILES}
        ${SOURCE_FILES}
)

add_executable(ThaxTests ${ALL_FILES})

target_include_directories(ThaxTests PRIVATE
        src
)

target_link_libraries(ThaxTests PUBLIC ThaxCore nlohmann_json::nlohmann_json)
