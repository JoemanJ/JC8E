cmake_minimum_required(VERSION 3.28.3)
project(JC8E LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### My libraries ###
add_library(Chip8Core
    src/CHIP_8/CPU.cpp
    src/CHIP_8/RAM.cpp
    src/CHIP_8/display.cpp
    src/CHIP_8/controller.cpp
)
target_include_directories(Chip8Core PUBLIC include)

add_library(Emulation
    src/emulation/emulator.cpp
    src/emulation/renderer.cpp
    src/emulation/inputMapper.cpp
)
target_include_directories(Emulation PUBLIC include)
target_link_libraries(Emulation
    PUBLIC
        Chip8Core
        sfml-system
)

add_library(Application
    src/application.cpp
)
target_include_directories(Application PUBLIC include)
target_link_libraries(Application
    PUBLIC
        Emulation
        ImGui-SFML::ImGui-SFML
)

### External libraries ###
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

# Other external libraries
add_subdirectory(external)

### Test executables ###
file(GLOB TEST_SOURCES "tests/test_*.cpp")

# Unit tests for each module
foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name}
        PRIVATE 
            Chip8Core
            Emulation
            gmock
            gtest_main
            sfml-graphics
            sfml-system
    )
    target_include_directories(${test_name} PUBLIC tests/include)
endforeach()

# Run all unit tests
add_executable(unit_tests
    tests/test_ram.cpp
    tests/test_cpu.cpp
    tests/test_display.cpp
    tests/test_controller.cpp
    tests/test_emulation.cpp
    tests/test_InputMapper.cpp
    tests/test_renderer.cpp
    # Add other test sources here
)

target_include_directories(unit_tests PUBLIC tests/include)

target_link_libraries(unit_tests
    PRIVATE
        Chip8Core
        Emulation
        gmock
        gtest_main
        sfml-graphics
        sfml-system
)

add_executable(test_interface1
    tests/manual/test_interface1.cpp
)

target_link_libraries(test_interface1
    PRIVATE
        Chip8Core
        Emulation
        ImGui-SFML::ImGui-SFML
)

add_executable(test_interface2
    tests/manual/test_interface2.cpp
)

target_link_libraries(test_interface2
    PRIVATE
        Chip8Core
        Emulation
        ImGui-SFML::ImGui-SFML
)

add_executable(test_interface3
    tests/manual/test_interface3.cpp
)

target_link_libraries(test_interface3
    PRIVATE
        Chip8Core
        Emulation
        ImGui-SFML::ImGui-SFML
        Application
)

### Main program ###
add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Chip8Core
    Emulation
    ImGui-SFML::ImGui-SFML
    Application
)