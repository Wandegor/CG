cmake_minimum_required(VERSION 3.30)
project(task3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        main.cpp
        pch.h
        BaseWindow.h
        GLFWInitializer.h
        Window.cpp
        Window.h
        Vertex.h
        MoebiusStrip.h
        Surface.h
)

target_sources(${PROJECT_NAME} PRIVATE ${GLAD_SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
if (WIN32)
    target_link_libraries(task1 PRIVATE opengl32)
endif()
