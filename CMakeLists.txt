cmake_minimum_required(VERSION 4.0)
project(dotLXDB)

set(CMAKE_CXX_STANDARD 23)

add_library(dotLXDB SHARED
        dotlxdb.cpp
        dotlxdb.h
)

add_executable(dotlxdbapp
        dotlxdb.cpp
        dotlxdb.h
)

link_libraries(dotlxdbapp PRIVATE dotLXDB)