cmake_minimum_required(VERSION 3.10)

project(dotLXDB)

add_library(dotLXDB SHARED
        src/dotlxdb.cpp
        src/dotlxdb.h
        src/exceptions.h
)
add_executable(dotLXDBapp
        src/dotlxdb.h
        src/dotlxdb.cpp
        src/exceptions.h
)