cmake_minimum_required(VERSION 3.4)
project(mjct)

include(ExternalProject)

ExternalProject_Add(mujoco 
    URL https://github.com/deepmind/mujoco/releases/download/2.3.3/mujoco-2.3.3-windows-x86_64.zip
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/mujoco
    URL_HASH MD5=bcef9fb0481063793cb3c3a0f59682be
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

ExternalProject_Add(glfw
    URL https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.WIN64.zip
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/glfw
    URL_HASH MD5=00ae95c16b81ae736c98c2c102409263
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

add_subdirectory(extern/pybind11)
pybind11_add_module(mjct MuJoCoTasks/binder.cpp)
target_include_directories(mjct PRIVATE mujocotasks)
target_include_directories(mjct PRIVATE extern/mujoco/include)
target_include_directories(mjct PRIVATE extern/glfw/include)

add_dependencies(mjct mujoco glfw)

target_link_directories(mjct PRIVATE ${CMAKE_SOURCE_DIR}/extern/mujoco/lib)
target_link_libraries(mjct PRIVATE mujoco.lib)

target_link_directories(mjct PRIVATE ${CMAKE_SOURCE_DIR}/extern/glfw/lib-vc2019)
target_link_libraries(mjct PRIVATE glfw3.lib glfw3dll.lib glfw3_mt.lib)


# Copy the XML and DLL files to the build/Debug directory
add_custom_command(TARGET mjct POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/mujocotasks/mjct_models/tosser.xml
        ${CMAKE_CURRENT_BINARY_DIR}/Debug/tosser.xml
)

add_custom_command(TARGET mjct POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/mujoco/bin/mujoco.dll
        ${CMAKE_CURRENT_BINARY_DIR}/Debug/mujoco.dll
)

