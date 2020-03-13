cmake_minimum_required(VERSION 3.15)
project(language)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(language
        src/lexer.cpp
        src/parser.cpp
        src/main.cpp src/parser.cpp src/syntax_tree.cpp)

