cmake_minimum_required(VERSION 3.30)
project(task1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        main.cpp
        Button.h
        View/ImageViewer.h
        Model/ImageModel.h
        Presenter/ImagePresenter.h
        Listeners/Document.h
        Listeners/IDocumentListener.h
        MouseData.h
        View/IView.h)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "D:/SFML-3.0.0/bin/"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying ALL DLLs from SFML"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/ArialRegular.ttf
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying font to executable directory"
)

target_link_libraries(${PROJECT_NAME}
        SFML::Graphics
        SFML::Window
        SFML::System
)

