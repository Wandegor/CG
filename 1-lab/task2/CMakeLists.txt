cmake_minimum_required(VERSION 3.30)
project(task2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task2 main.cpp
        Common/Color.h
        Common/Position.h
        PrimitiveShapes/RectangleAdapter.h
        PrimitiveShapes/CircleAdapter.h
        PrimitiveShapes/ConvexAdapter.h
        IShape.h
)

add_custom_command(TARGET task2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "D:/SFML-3.0.0/bin/"
        $<TARGET_FILE_DIR:task2>
        COMMENT "Copying ALL DLLs from SFML"
)

target_link_libraries(task2
        SFML::Graphics
        SFML::Window
        SFML::System
)