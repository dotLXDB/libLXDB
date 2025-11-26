cmake_minimum_required(VERSION 3.10)

project(dotLXDB)

include(FetchContent)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.2  # or whatever the latest version is
)

FetchContent_MakeAvailable(json)

target_include_directories(your_target PRIVATE /home/github/actions/_work/libLXDB/libLXDB/build/_deps/json-src/include/)

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