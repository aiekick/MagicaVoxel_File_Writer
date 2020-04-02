cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0012 NEW)
cmake_policy(SET CMP0023 NEW)

set(PROJECT VoxWriter)
project(${PROJECT} CXX)

enable_language(CXX)

if(MSVC)
    add_definitions(-DMSVC)
endif()

add_definitions(-std=c++11)

add_executable(${PROJECT} main.cpp VoxWriter.cpp VoxWriter.h)

