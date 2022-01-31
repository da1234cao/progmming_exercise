cmake_minimum_required(VERSION 3.16)

SET(CMAKE_BUILD_TYPE "Debug")

project(test)

add_executable(${PROJECT_NAME} test.cpp)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} pthread)