cmake_minimum_required(VERSION 3.28.3)
project(JC8E LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# My libraries
add_library(
    Chip8Core
    src/CHIP_8/CPU.cpp
    src/CHIP_8/RAM.cpp
    src/CHIP_8/display.cpp
    src/CHIP_8/controller.cpp
    src/emulation/emulator.cpp
)
target_include_directories(Chip8Core PUBLIC include)

include(FetchContent)
# SFML 2.6.2
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.2
)

set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)

FetchContent_MakeAvailable(SFML)

# GoogleTest for unit testing
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip 
)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
    tests/test_ram.cpp
    tests/test_cpu.cpp
    tests/test_display.cpp
    tests/test_controller.cpp
    # Add other test sources here
)

target_link_libraries(unit_tests
    PRIVATE
        Chip8Core
        gmock
        gtest_main
)

add_executable(JC8E
    src/main.cpp
)

target_link_libraries(JC8E
    Chip8Core
    sfml-graphics
)