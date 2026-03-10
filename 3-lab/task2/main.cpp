#include <glad/glad.h>
#include <cmath>
#include <iostream>
#include "Mat3.h"
#include "../task1/Window.h"
#include "Shaders/Shader.h"
#include "Shapes/Circle.h"
#include "Shapes/Drawable.h"
#include "Shapes/LineStrip.h"
#include "Shapes/Rectangle.h"

struct GameObject
{
    Drawable *drawable;
    Mat3 model;
    float color[4];
};

const char *pVSFileName = "shader.vs";
const char *pFSFileName = "shader.fs";

float lastTime = (float) glfwGetTime();
float angularSpeed = 2.0f;

int main()
{
    Window window(1000, 1000, "Engine Cutaway (Static)");

    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader(pVSFileName, pFSFileName);
    GLuint shaderProgram = shader.GetProgram();

    GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");

    glViewport(0, 0, window.GetWidth(), window.GetHeight());
    glEnable(GL_MULTISAMPLE);

    float crankLen = 0.17f;

    Rectangle cylinderBlock(0.3f, 0.55f); // блок цилиндров
    Rectangle piston(0.25f, 0.2f); // поршень
    Circle flywheel(crankLen); // маховик

    Circle flywheelBack(crankLen + 0.1f);
    Rectangle cylinderBlockBack(0.4f, 0.65f);

    // Шатун
    std::vector<Point> rodLocal = {
            {0.0f, 0.0f},
            {0.0f, -0.45f}
    };
    LineStrip connectingRod(rodLocal);

    // Коленвал
    std::vector<Point> crankLocal = {
            {0.0f,     0.0f},
            {crankLen, 0}
    };
    LineStrip crankshaft(crankLocal);

    // Свеча
    Rectangle sparkPlug(0.08f, 0.15f);

    std::vector<GameObject> objects;

    // Блок цилиндра ФОН
    objects.push_back({
                              &cylinderBlockBack,
                              Mat3::translation(0.0f, -0.15f),
                              {0.25f, 0.25f, 0.25f, 1}
                      });

    // Маховик ФОН
    objects.push_back({
                              &flywheelBack,
                              Mat3::translation(0.0f, -0.6f),
                              {0.25f, 0.25f, 0.25f, 1}
                      });
    // Блок цилиндра
    objects.push_back({
                              &cylinderBlock,
                              Mat3::translation(0.0f, -0.15f),
                              {0.55f, 0.55f, 0.55f, 1}
                      });

    // Маховик
    objects.push_back({
                              &flywheel,
                              Mat3::translation(0.0f, -0.6f),
                              {0.2f, 0.2f, 0.8f, 1}
                      });

    // Поршень
    objects.push_back({
                              &piston,
                              Mat3::translation(0.0f, 0.0f),
                              {0.8f, 0.2f, 0.2f, 1}
                      });

    // Шатун
    objects.push_back({
                              &connectingRod,
                              Mat3::translation(0.0f, 0.0f),
                              {0, 0, 0, 1}
                      });

    // Коленвал
    objects.push_back({
                              &crankshaft,
                              Mat3::translation(0.0f, -0.6f),
                              {0, 0, 0, 1}
                      });

    // Свеча
    objects.push_back({
                              &sparkPlug,
                              Mat3::translation(0.0f, 0.16f),
                              {0, 0, 0, 1}
                      });

    float angle = 0.0f;

    float crankRadius = crankLen; // радиус коленвала
    float rodLen = 0.45f; // длина шатуна
    float crankCenterY = -0.6f;

    glLineWidth(6.0f);
    while (!window.ShouldClose())
    {
        window.ProcessInput();

        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        angle += angularSpeed * deltaTime;
        float crankX = crankRadius * std::cos(angle);
        float crankY = crankCenterY + crankRadius * std::sin(angle);
        float pistonY = crankY + std::sqrt(rodLen * rodLen - crankX * crankX);

        objects[2 + 2].model = Mat3::translation(0.0f, pistonY);        // piston
        objects[3 + 2].model = Mat3::translation(0.0f, pistonY);        // rod
        objects[4 + 2].model = Mat3::translation(0.0f, crankCenterY) *
                               Mat3::rotation(angle);

        float rodAngle = std::atan2(crankX, pistonY - crankY);

        objects[3 + 2].model =
                Mat3::translation(0.0f, pistonY) *
                Mat3::rotation(rodAngle);
        for (const auto &obj: objects)
        {
            glUniformMatrix3fv(modelLoc, 1, GL_TRUE, obj.model.data);
            glUniform4fv(colorLoc, 1, obj.color);
            obj.drawable->Draw();
        }

        glBindVertexArray(0);
        window.SwapBuffers();
        window.PollEvents();
    }
    return 0;
}
