cmake_minimum_required(VERSION 3.6)
project(study_hours)

set(CMAKE_CXX_STANDARD 20)

add_executable(tracker main.cpp data_class.cpp)

add_executable(setup setup.cpp)