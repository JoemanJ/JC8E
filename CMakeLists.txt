cmake_minimum_required(VERSION 3.28.3)
project(JC8E VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/main.cpp
)

add_executable(JC8E ${SOURCES})

find_package(SFML 2.6 COMPONENTS window graphics REQUIRED)
target_link_libraries(JC8E sfml-graphics sfml-window)