cmake_minimum_required(VERSION 3.14)
project(SearchEngine VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

# ===== nlohmann/json =====
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

if (NOT TARGET nlohmann_json::nlohmann_json)
    message(FATAL_ERROR "nlohmann/json not found!")
endif()

# ===== Main Project =====
set(MAIN_SOURCES
    src/main.cpp
    src/ConverterJSON.cpp
    src/InvertedIndex.cpp
    src/SearchServer.cpp

    include/ConverterJSON.h
    include/InvertedIndex.h
    include/SearchServer.h
)

foreach(json_file requests config answers)
    if(EXISTS "${CMAKE_SOURCE_DIR}/${json_file}.json")
        configure_file(
            "${CMAKE_SOURCE_DIR}/${json_file}.json"
            "${CMAKE_BINARY_DIR}/${json_file}.json"
            COPYONLY
        )
        message(STATUS "Copied ${json_file}.json to build directory")
    else()
        message(WARNING "File ${json_file}.json not found - skipping")
    endif()
endforeach()

add_executable(SearchEngine ${MAIN_SOURCES})
target_include_directories(SearchEngine PRIVATE include)
target_link_libraries(SearchEngine PRIVATE nlohmann_json::nlohmann_json)

# ===== Google Test =====
set(TEST_SOURCES
    tests/test_ConverterJSON.cpp
    tests/test_InvertedIndex.cpp
    tests/test_SearchServer.cpp
    tests/other_tests.cpp

    src/ConverterJSON.cpp
    src/InvertedIndex.cpp
    src/SearchServer.cpp
)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

if(TARGET gtest AND TARGET gtest_main)
    message(STATUS "Google Test is available")
    add_executable(tests ${TEST_SOURCES})
    target_link_libraries(tests
        PRIVATE
        gtest_main
        nlohmann_json::nlohmann_json
    )
    target_include_directories(tests PRIVATE include)
    enable_testing()
    add_test(NAME AllTests COMMAND tests)
else()
    message(WARNING "Google Test could not be initialized")
endif()
