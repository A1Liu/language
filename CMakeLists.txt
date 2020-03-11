cmake_minimum_required(VERSION 3.15)
project(language)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(language
        src/lexer.cpp
        src/lexer.h
        src/main.cpp)
