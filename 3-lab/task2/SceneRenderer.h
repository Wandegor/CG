#pragma once

#include <vector>
#include "Shapes/Drawable.h"
#include "Mat3.h"
#include "Shapes/Circle.h"
#include "Shapes/Drawable.h"
#include "Shapes/LineStrip.h"
#include "Shapes/Rectangle.h"
#include "Shaders/Shader.h"

struct GameObject
{
    Drawable *drawable;
    Mat3 model;
    float color[4];
};

class SceneRenderer
{
private:
    std::vector<GameObject> m_objects;

    Shader *shader;

    GLint colorLoc;
    GLint modelLoc;
    float m_crankLen = 0.17f;

    float angle = 0.0f;

    float crankRadius;
    float rodLen;
    float crankCenterY;

    int flashIndex = -1;

    Rectangle cylinderBlock{0.3f, 0.55f};
    Rectangle piston{0.25f, 0.2f};
    Circle flywheel{m_crankLen};

    Circle flywheelBack{m_crankLen + 0.1f};
    Rectangle cylinderBlockBack{0.4f, 0.65f};

    LineStrip connectingRod;
    LineStrip crankshaft;

    Rectangle sparkPlug{0.06f, 0.15f};
    Circle sparkFlash{0.04f};

public:
    explicit SceneRenderer(Shader *shader)
            : shader(shader),
              connectingRod(std::vector<Point>{
                      {0.0f, 0.0f},
                      {0.0f, -0.45f}
              }),
              crankshaft(std::vector<Point>{
                      {0.0f,       0.0f},
                      {m_crankLen, 0}
              })
    {
        GLuint program = shader->GetProgram();

        colorLoc = glGetUniformLocation(program, "uColor");
        modelLoc = glGetUniformLocation(program, "model");

        crankRadius = 0.17f;
        rodLen = 0.45f;
        crankCenterY = -0.6f;

        // Блок цилиндра ФОН
        m_objects.push_back({
                                    &cylinderBlockBack,
                                    Mat3::translation(0.0f, -0.15f),
                                    {0.25f, 0.25f, 0.25f, 1}
                            });

        // Маховик ФОН
        m_objects.push_back({
                                    &flywheelBack,
                                    Mat3::translation(0.0f, -0.6f),
                                    {0.25f, 0.25f, 0.25f, 1}
                            });
        // Блок цилиндра
        m_objects.push_back({
                                    &cylinderBlock,
                                    Mat3::translation(0.0f, -0.15f),
                                    {0.55f, 0.55f, 0.55f, 1}
                            });

        // Маховик
        m_objects.push_back({
                                    &flywheel,
                                    Mat3::translation(0.0f, -0.6f),
                                    {0.2f, 0.2f, 0.8f, 1}
                            });

        // Поршень
        m_objects.push_back({
                                    &piston,
                                    Mat3::translation(0.0f, 0.0f),
                                    {0.8f, 0.2f, 0.2f, 1}
                            });

        // Шатун
        m_objects.push_back({
                                    &connectingRod,
                                    Mat3::translation(0.0f, 0.0f),
                                    {0, 0, 0, 1}
                            });

        // Коленвал
        m_objects.push_back({
                                    &crankshaft,
                                    Mat3::translation(0.0f, -0.6f),
                                    {0, 0, 0, 1}
                            });

        // Свеча
        m_objects.push_back({
                                    &sparkPlug,
                                    Mat3::translation(0.0f, 0.16f),
                                    {0, 0, 0, 1}
                            });

        // Вспышка на свече
        m_objects.push_back({
                                    &sparkFlash,
                                    Mat3::translation(0.0f, 0.1f),
                                    {1.0f, 1.0f, 0.0f, 1.0f}
                            });

        flashIndex = m_objects.size() - 1;
    }

    void DrawScene()
    {
        GLuint program = shader->GetProgram();

        glUseProgram(program);

        glLineWidth(6.0f);

        for (const auto &obj: m_objects)
        {
            glUniformMatrix3fv(modelLoc, 1, GL_TRUE, obj.model.data);
            glUniform4fv(colorLoc, 1, obj.color);

            obj.drawable->Draw();
        }
    }

    void Update(float deltaTime)
    {
        float angularSpeed = 2.0f;

        angle += angularSpeed * deltaTime;

        float flashIntensity = std::max(0.0f, std::sin(angle - 0.4f) * 1.2f);
        float flashScale = flashIntensity;

        if (flashIndex >= 0)
        {
            m_objects[flashIndex].model =
                    Mat3::translation(0.0f, 0.1f) *
                    Mat3::scale(flashScale, flashScale);
        }

        float crankX = crankRadius * std::cos(angle);
        float crankY = crankCenterY + crankRadius * std::sin(angle);
        float pistonY = crankY + std::sqrt(rodLen * rodLen - crankX * crankX);

        m_objects[4].model = Mat3::translation(0.0f, pistonY);

        float rodAngle = std::atan2(crankX, pistonY - crankY);

        m_objects[5].model =
                Mat3::translation(0.0f, pistonY) *
                Mat3::rotation(rodAngle);

        m_objects[6].model =
                Mat3::translation(0.0f, crankCenterY) *
                Mat3::rotation(angle);
    }
};
