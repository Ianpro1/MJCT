cmake_minimum_required(VERSION 3.4)

project(mjct)

add_subdirectory(extern/pybind11)

# Create the Pybind11 module
pybind11_add_module(mjct MuJoCoTasks/binder.cpp)

# Link any additional libraries if needed
target_include_directories(mjct PRIVATE mujocotasks)
