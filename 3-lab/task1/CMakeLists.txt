cmake_minimum_required(VERSION 3.30)
project(task1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        main.cpp
        Common/Point.h
        Common/Util.h
        Application.h
        Bezier.h
        Shaders/Shader.h
        Window.h
        BezierRenderer.h
)

file(COPY Shaders/shader.vs Shaders/shader.fs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_sources(${PROJECT_NAME} PRIVATE ${GLAD_SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

if (WIN32)
    target_link_libraries(task1 PRIVATE opengl32)
endif()
