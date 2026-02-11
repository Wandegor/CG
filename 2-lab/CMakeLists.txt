cmake_minimum_required(VERSION 3.30)
project(2_lab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "D:/SFML-3.0.0/lib/cmake/SFML")
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

add_subdirectory(task1)
add_subdirectory(task2)
#add_subdirectory(task3)