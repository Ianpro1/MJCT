cmake_minimum_required(VERSION 3.4)

project(mjct)

include(ExternalProject)

ExternalProject_Add(mujoco URL https://github.com/deepmind/mujoco/releases/download/2.3.3/mujoco-2.3.3-windows-x86_64.zip
SOURCE_DIR ../extern/mujoco
URL_HASH MD5=bcef9fb0481063793cb3c3a0f59682be
CONFIGURE_COMMAND "" 
BUILD_COMMAND ""
INSTALL_COMMAND ""
)

ExternalProject_Add(glfw URL https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.WIN64.zip
SOURCE_DIR ../extern/glfw
URL_HASH MD5=00ae95c16b81ae736c98c2c102409263
CONFIGURE_COMMAND "" 
BUILD_COMMAND ""
INSTALL_COMMAND ""
)

add_subdirectory(extern/pybind11)

pybind11_add_module(mjct MuJoCoTasks/binder.cpp)

target_include_directories(mjct PRIVATE mujocotasks)
target_include_directories(mjct PRIVATE extern/mujoco/include)
target_include_directories(mjct PRIVATE extern/glfw/include)

find_library(MUJOCO_LIBRARY
    NAMES mujoco.lib
    HINTS extern/mujoco/lib
)

if(MUJOCO_LIBRARY)
message("found mujoco.lib at " ${MUJOCO_LIBRARY})
target_link_libraries(mjct PRIVATE ${MUJOCO_LIBRARY})
else()
message(USERWARNING " couldn't find mujoco.lib starting from (but not limited to) /extern/mujoco/lib")
endif()

find_library(GLFW_LIBRARY
    NAMES glfw3.lib
    HINTS extern/glfw/lib-vc2019
)

if(GLFW_LIBRARY)
message("found glfw3.lib at " ${GLFW_LIBRARY})
target_link_libraries(mjct PRIVATE ${GLFW_LIBRARY})
else()
message(USERWARNING " couldn't find glfw3.lib starting from (but not limited to) /extern/lib-vc2019")
endif()

find_library(GLFWDLL_LIBRARY
    NAMES glfw3dll.lib
    HINTS extern/glfw/lib-vc2019
)

if(GLFWDLL_LIBRARY)
message("found glfw3dll.lib at " ${GLFWDLL_LIBRARY})
target_link_libraries(mjct PRIVATE ${GLFWDLL_LIBRARY})
else()
message(USERWARNING " couldn't find glfw3dll.lib starting from (but not limited to) /extern/lib-vc2019")
endif()

find_library(GLFW_MT_LIBRARY
    NAMES glfw3_mt.lib
    HINTS extern/glfw/lib-vc2019
)

if(GLFW_MT_LIBRARY)
message("found glfw3_mt.lib at " ${GLFW_MT_LIBRARY})
target_link_libraries(mjct PRIVATE ${GLFW_MT_LIBRARY})
else()
message(USERWARNING " couldn't find glfw3_mt.lib starting from (but not limited to) /extern/lib-vc2019")
endif()

