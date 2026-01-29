cmake_minimum_required(VERSION 3.10)

project(process_runner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(CMAKE_CONFIGURATION_TYPES)
    foreach(OUTPUTCONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
        string(TOUPPER "${OUTPUTCONFIG}" OUTPUTCONFIG_UPPER)
        set("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER}"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endforeach()
endif()

add_library(process_runner STATIC
    src/process_runner.cpp
)

target_include_directories(process_runner
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

if (WIN32)
    target_compile_definitions(process_runner PRIVATE _WIN32_WINNT=0x0601)
endif()

add_executable(test_runner
    test/test_runner.cpp
)

target_link_libraries(test_runner
    PRIVATE
        process_runner
)
