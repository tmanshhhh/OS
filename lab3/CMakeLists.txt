cmake_minimum_required(VERSION 3.10)

project(Lab3 LANGUAGES CXX)

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Output directory ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
if(CMAKE_CONFIGURATION_TYPES)
    foreach(OUTPUTCONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
        string(TOUPPER "${OUTPUTCONFIG}" OUTPUTCONFIG_UPPER)
        set("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endforeach()
endif()

# --- Threads ---
find_package(Threads REQUIRED)

# --- Executable ---
add_executable(lab3
    src/main.cpp
    src/app.cpp
    src/platform.cpp
)

# --- Include directories ---
target_include_directories(lab3 PRIVATE include)

# --- Link libraries ---
if(UNIX)
    target_link_libraries(lab3_main PRIVATE Threads::Threads rt)
else()
    target_link_libraries(lab3_main PRIVATE Threads::Threads)
endif()

# --- Windows-specific definitions ---
if(WIN32)
    target_compile_definitions(lab3 PRIVATE _WIN32_WINNT=0x0601)
endif()
