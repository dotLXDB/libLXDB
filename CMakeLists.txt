cmake_minimum_required(VERSION 3.10)

project(dotLXDB)

add_library(dotLXDB SHARED
        dotlxdb.cpp
        dotlxdb.h
)
add_executable(dotLXDBapp
        dotlxdb.h
        dotlxdb.cpp
)