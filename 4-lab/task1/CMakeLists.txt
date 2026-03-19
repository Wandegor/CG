cmake_minimum_required(VERSION 3.30)
project(task1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        main.cpp
        Common/Point.h
        Common/Util.h
        pch.cpp
        pch.h
        BaseWindow.cpp
        BaseWindow.h
        StarPolyhedron.cpp
        StarPolyhedron.h
        GLFWInitializer.cpp
        GLFWInitializer.h
        Window.cpp
)

file(COPY Shaders/shader2.vs Shaders/shader2.fs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_sources(${PROJECT_NAME} PRIVATE ${GLAD_SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
if (WIN32)
    target_link_libraries(task1 PRIVATE opengl32)
endif()
