cmake_minimum_required(VERSION 3.20)

project(CerebroX VERSION 1.0.0 LANGUAGES CXX)

option(CEREBROX_BUILD_BENCHMARKS "Build benchmarks" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(cerebrox_core
    src/cerebrox/advisor.cpp
    src/cerebrox/cli.cpp
    src/cerebrox/collector.cpp
    src/cerebrox/controller.cpp
    src/cerebrox/predictor.cpp
    src/cerebrox/system_snapshot.cpp
)

target_include_directories(cerebrox_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(cerebrox_core PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

target_link_libraries(cerebrox_core PUBLIC Threads::Threads)

add_executable(cerebrox
    src/main.cpp
)

target_link_libraries(cerebrox PRIVATE cerebrox_core)

if(CEREBROX_BUILD_BENCHMARKS)
    add_executable(cerebrox_benchmark
        benchmarks/benchmark.cpp
    )
    target_link_libraries(cerebrox_benchmark PRIVATE cerebrox_core)
endif()
