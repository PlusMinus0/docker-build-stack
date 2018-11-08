cmake_minimum_required(VERSION 3.11)

set(PROJECT gcc-test)

project(${PROJECT})

set(CMAKE_CXX_STANDARD)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT} main.cpp)
