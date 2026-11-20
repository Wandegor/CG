cmake_minimum_required(VERSION 3.30)
project(task3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task3 main.cpp
        Circle.h
        Canvas.h
        Common/Color.h
        Common/Position.h)

add_custom_command(TARGET task3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "D:/SFML-3.0.0/bin/"
        $<TARGET_FILE_DIR:task3>
        COMMENT "Copying ALL DLLs from SFML"
)

target_link_libraries(task3
        SFML::Graphics
        SFML::Window
        SFML::System
)