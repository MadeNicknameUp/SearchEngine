cmake_minimum_required(VERSION 3.10)
project(SearchEngine VERSION 1.0 LANGUAGES CXX)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory(extern/json)

if (TARGET nlohmann_json::nlohmann_json)
    message(STATUS "nlohmann/json found in submodule")
else()
    message(FATAL_ERROR "nlohmann/json not found!")
endif()

set(SOURCES
    src/ConverterJSON.cpp
)

set(HEADERS
    include/ConverterJSON.h
)

# Копируем JSON-файлы в папку сборки (build/)
configure_file(
    ${CMAKE_SOURCE_DIR}/requests.json
    ${CMAKE_BINARY_DIR}/requests.json
    COPYONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/config.json
    ${CMAKE_BINARY_DIR}/config.json
    COPYONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/answers.json
    ${CMAKE_BINARY_DIR}/answers.json
    COPYONLY
)

add_executable(SearchEngine ${SOURCES} ${HEADERS})

target_include_directories(SearchEngine PRIVATE include)
target_link_libraries(SearchEngine PRIVATE nlohmann_json::nlohmann_json)
