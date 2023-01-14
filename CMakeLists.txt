cmake_minimum_required(VERSION 3.25)

project(ig-dler VERSION 0.1.0 LANGUAGES CXX)

add_executable(ig-dler src/main.cpp)

install(TARGETS ig-dler
        RUNTIME
        DESTINATION "${CMAKE_SOURCE_DIR}/dist")
