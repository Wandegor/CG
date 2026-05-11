cmake_minimum_required(VERSION 3.30)
project(8_lab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLAD_INCLUDE_DIR D:/GLAD/include)
set(GLAD_SOURCE D:/GLAD/src/gl.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stbi)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build tests" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build documentation" FORCE)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glm)

add_subdirectory(task1)
add_subdirectory(task2)