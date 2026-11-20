cmake_minimum_required(VERSION 3.30)
project(task3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        main.cpp
        Shaders/Shader.h
        BaseWindow.h
        GLFWInitializer.h
        pch.h
        Presenter.h
        Window.h
        Window.cpp
        RenderObject.h
        ShapeFactory.h
)

file(COPY Shaders/shader.vs Shaders/shader.fs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#add_custom_command(TARGET task1 POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${CMAKE_CURRENT_SOURCE_DIR}/Textures"
#        "$<TARGET_FILE_DIR:task1>/Textures"
#)

target_sources(${PROJECT_NAME} PRIVATE ${GLAD_SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
if (WIN32)
    target_link_libraries(task1 PRIVATE opengl32)
endif()
