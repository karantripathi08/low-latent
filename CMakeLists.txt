cmake_minimum_required(VERSION 3.20)

project(Cerebro VERSION 1.0.0 LANGUAGES CXX)

option(CEREBRO_BUILD_TESTS "Build unit tests" ON)
option(CEREBRO_BUILD_BENCHMARKS "Build benchmarks" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

find_package(Threads REQUIRED)

add_library(cerebro_core
    src/cerebro/async_logger.cpp
    src/cerebro/engine.cpp
    src/cerebro/market_data_generator.cpp
    src/cerebro/matching_engine.cpp
    src/cerebro/packet_decoder.cpp
    src/cerebro/order_book.cpp
    src/cerebro/risk_engine.cpp
    src/cerebro/statistics_engine.cpp
)

target_include_directories(cerebro_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(cerebro_core PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

target_link_libraries(cerebro_core PUBLIC Threads::Threads)

add_executable(cerebro
    src/main.cpp
)

target_link_libraries(cerebro PRIVATE cerebro_core)

if(CEREBRO_BUILD_BENCHMARKS)
    add_executable(cerebro_benchmark
        benchmarks/benchmark.cpp
    )
    target_link_libraries(cerebro_benchmark PRIVATE cerebro_core)
endif()

if(CEREBRO_BUILD_TESTS)
    find_package(GTest QUIET)

    if(GTest_FOUND)
        add_executable(cerebro_tests
            tests/test_decoder.cpp
            tests/test_matching_engine.cpp
            tests/test_order_book.cpp
            tests/test_risk_engine.cpp
        )

        target_link_libraries(cerebro_tests PRIVATE cerebro_core GTest::gtest_main)

        include(GoogleTest)
        gtest_discover_tests(cerebro_tests)
    else()
        message(STATUS "GoogleTest not found; skipping unit test target")
    endif()
endif()
