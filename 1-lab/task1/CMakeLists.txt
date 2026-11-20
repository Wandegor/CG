cmake_minimum_required(VERSION 3.30)
project(task1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task1
        main.cpp
        Letter/Color.h
        Letter/Letter.h
        Letter/Position.h
        Letter/DrawStrategy/IDrawStrategy.h
        Letter/DrawStrategy/DrawT.h
        Letter/DrawStrategy/DrawE.h
        Letter/DrawStrategy/DrawA.h
        Letter/JumpStrategy/IJumpStrategy.h
        Letter/JumpStrategy/UniformJump.h)

add_custom_command(TARGET task1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "D:/SFML-3.0.0/bin/"
        $<TARGET_FILE_DIR:task1>
        COMMENT "Copying ALL DLLs from SFML"
)

target_link_libraries(task1
        SFML::Graphics
        SFML::Window
        SFML::System
)