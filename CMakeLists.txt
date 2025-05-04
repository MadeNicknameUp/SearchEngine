cmake_minimum_required(VERSION 3.10)
project(SearchEngine VERSION 1.0 LANGUAGES CXX)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory(extern/json)
if (NOT TARGET nlohmann_json::nlohmann_json)
    message(FATAL_ERROR "nlohmann/json not found!")
endif()

set(MAIN_SOURCES
    src/main.cpp
    src/ConverterJSON.cpp
    src/InvertedIndex.cpp
    src/SearchServer.cpp

    include/ConverterJSON.h
    include/InvertedIndex.h
    include/SearchServer.h
)

set(TEST_SOURCES
    tests/test_ConverterJSON.cpp
    tests/test_InvertedIndex.cpp
    tests/test_SearchServer.cpp

    src/ConverterJSON.cpp
    src/InvertedIndex.cpp
    src/SearchServer.cpp
)

configure_file(${CMAKE_SOURCE_DIR}/requests.json ${CMAKE_BINARY_DIR}/requests.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/answers.json ${CMAKE_BINARY_DIR}/answers.json COPYONLY)

add_executable(SearchEngine ${MAIN_SOURCES})
target_include_directories(SearchEngine PRIVATE include)
target_link_libraries(SearchEngine PRIVATE nlohmann_json::nlohmann_json)

# ===== Google Test =====
if(EXISTS ${CMAKE_SOURCE_DIR}/extern/googletest/CMakeLists.txt)
    message(STATUS "Found Google Test submodule")
    add_subdirectory(extern/googletest)

    if(TARGET gtest AND TARGET gtest_main)
        message(STATUS "Google Test targets available")

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
        message(WARNING "Google Test targets not found")
    endif()
else()
    message(WARNING "Google Test submodule not found at extern/googletest")
endif()
