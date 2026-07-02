cmake_minimum_required(VERSION 3.15)

project(MopCode VERSION 0.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(mopc
    src/main.cpp
    src/Cli.cpp
    src/FileResolver.cpp
    src/Interpreter.cpp
    src/Lexer.cpp
)

target_include_directories(mopc PRIVATE src)
