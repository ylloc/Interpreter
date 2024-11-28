cmake_minimum_required(VERSION 3.28)

project(Interpreter)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD 23)

include_directories(lib)
include_directories(src)

add_executable(Interpreter main.cpp
        lib/node.h
        lib/error.h
        lib/scheme.h
        lib/scheme_list.h
        lib/evaluate.h
        lib/parser.h
        src/parser.cpp
        src/evaluate.cpp
        src/scheme.cpp)