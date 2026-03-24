cmake_minimum_required(VERSION 3.28.3)
project(JC8E LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# My libraries
add_library(
    Chip8Core
    src/CHIP_8/CPU.cpp
    src/CHIP_8/RAM.cpp
)
target_include_directories(Chip8Core PUBLIC include)

# GoogleTest for unit testing
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip 
)
FetchContent_MakeAvailable(googletest)

add_executable(cpu_tests
    tests/test_cpu.cpp
)

target_link_libraries(cpu_tests
    PRIVATE
        Chip8Core
        gtest_main
)

add_executable(ram_tests
    tests/test_ram.cpp
)

target_link_libraries(ram_tests
    PRIVATE
        Chip8Core
        gtest_main
)

# include_directories(include)

# set(SOURCES
#     src/main.cpp
# )

# add_executable(JC8E ${SOURCES})

# find_package(SFML 2.6 COMPONENTS window graphics REQUIRED)
# target_link_libraries(JC8E sfml-graphics sfml-window)