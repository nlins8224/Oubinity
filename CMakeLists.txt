cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(Oubinity CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_subdirectory(Oubinity)

