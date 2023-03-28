cmake_minimum_required(VERSION 3.4)
project(mujocotasks)

include(ExternalProject)

find_library(MJC_LIB NAMES mujoco.lib PATHS extern/mujoco/lib)
find_file(MJC_DLL NAMES mujoco.dll PATHS extern/mujoco/bin)

if(NOT MJC_LIB OR NOT MJC_DLL)
ExternalProject_Add(mujoco 
    URL https://github.com/deepmind/mujoco/releases/download/2.3.3/mujoco-2.3.3-windows-x86_64.zip
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/mujoco
    URL_HASH MD5=bcef9fb0481063793cb3c3a0f59682be
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
endif()

find_library(GLFW_LIB NAMES glfw3.lib PATHS extern/glfw/lib-vc2019)
find_library(GLFW_LIBDLL NAMES glfw3dll.lib PATHS extern/glfw/lib-vc2019)
find_library(GLFW_LIBMT NAMES glfw3_mt.lib PATHS extern/glfw/lib-vc2019)

if(NOT GLFW_LIB OR NOT GLFW_LIBDLL OR NOT GLFW_LIBMT)
ExternalProject_Add(glfw
    URL https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.WIN64.zip
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/glfw
    URL_HASH MD5=00ae95c16b81ae736c98c2c102409263
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
endif()

add_subdirectory(extern/pybind11)
pybind11_add_module(mujocotasks MuJoCoTasks/binder.cpp)
target_include_directories(mujocotasks PRIVATE mujocotasks)
target_include_directories(mujocotasks PRIVATE extern/mujoco/include)
target_include_directories(mujocotasks PRIVATE extern/glfw/include)

#disabled for now, not necessary...
#add_dependencies(mujocotasks mujoco glfw)

target_link_directories(mujocotasks PRIVATE ${CMAKE_SOURCE_DIR}/extern/mujoco/lib)
target_link_libraries(mujocotasks PRIVATE mujoco.lib)

target_link_directories(mujocotasks PRIVATE ${CMAKE_SOURCE_DIR}/extern/glfw/lib-vc2019)
target_link_libraries(mujocotasks PRIVATE glfw3.lib glfw3dll.lib glfw3_mt.lib)


# Copy the XML and DLL files to the mjct directory
add_custom_command(TARGET mujocotasks POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/mujoco/bin/mujoco.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/mjct/mujoco.dll
)

