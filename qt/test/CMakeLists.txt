cmake_minimum_required(VERSION 3.11)

set(PROJECT qt-test)

project(${PROJECT})

set(CMAKE_CXX_STANDARD)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Widgets Script Svg XmlPatterns WebEngine REQUIRED)
include_directories(${Qt5_INCLUDES})

add_executable(${PROJECT} main.cpp)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Script Qt5::Svg Qt5::XmlPatterns Qt5::WebEngine)
