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
    float m_aspect;

    GLint colorLoc;
    GLint modelLoc;
    float m_crankLen = 0.17f;

    float angle = 0.0f;

    float crankRadius;
    float rodLen;
    float crankCenterY;

    int flashIndex = -1;

    Rectangle cylinderBlock{0.28f, 0.55f};
    Rectangle piston{0.28f, 0.2f};
    Circle flywheel{0.225f};

    Circle flywheelBack{m_crankLen + 0.1f};
    Rectangle cylinderBlockBack{0.37f, 0.65f};

    LineStrip connectingRod;
    LineStrip crankshaft;

    Rectangle valveLeft{0.06f, 0.02f};
    Rectangle valveRight{0.06f, 0.02f};
    Rectangle valveLeftCylinder{0.06f, 0.1f};
    Rectangle valveRightCylinder{0.06f, 0.1f};

    Rectangle sparkPlug{0.06f, 0.15f};
    Circle sparkFlash{0.04f};

    std::vector<Circle> exhaustParticles;
    std::vector<int> exhaustIndices;
    int particleCount = 4;

public:
    explicit SceneRenderer(Shader *shader, float aspect)
            : shader(shader), m_aspect(aspect),
              connectingRod(std::vector<Point>{
                      {0.0f, 0.0f},
                      {0.0f, -0.45f}
              }),
              crankshaft(std::vector<Point>{
                      {0.0f, 0.0f},
                      {0,    m_crankLen}
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
                                    {0.55f, 0.55f, 0.55f, 1}
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

        // Трубы клапанов
        m_objects.push_back({
                                    &valveLeftCylinder,
                                    Mat3::translation(-0.1f, 0.17f),
                                    {0.8f, 0.6f, 0.8f, 1}
                            });
        m_objects.push_back({
                                    &valveRightCylinder,
                                    Mat3::translation(0.1f, 0.17f),
                                    {0.6f, 0.8f, 0.8f, 1}
                            });

        // клапаны
        m_objects.push_back({
                                    &valveLeft,
                                    Mat3::translation(-0.1f, 0.12f),
                                    {0.2f, 0.2f, 0.5f, 1}
                            });
        m_objects.push_back({
                                    &valveRight,
                                    Mat3::translation(0.1f, 0.12f),
                                    {0.2f, 0.2f, 0.5f, 1}
                            });

        // вых газ
        exhaustParticles.resize(particleCount, Circle(0.03f));
        for (int i = 0; i < particleCount; ++i)
        {
            m_objects.push_back({
                                        &exhaustParticles[i],
                                        Mat3::translation(0.1f, 0.15f) * Mat3::scale(0.0f, 0.0f),
                                        {0.5f, 0.5f, 0.5f, 1.0f}
                                });
            exhaustIndices.push_back(m_objects.size() - 1);
        }

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

        int uni_loc = glGetUniformLocation(program, "scr_aspect");
        glUniform1f(uni_loc, m_aspect); // пропорции картинки относительно окна

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

        UpdatePistonAndCrank();

        UpdateValves();

        UpdateExhaustParticles();
        UpdateFlash();
    }

    void SetAspect(float aspect)
    { m_aspect = aspect; }

private:
    void UpdatePistonAndCrank()
    {

        float crankX = crankRadius * -std::sin(angle); // x = R·sin(angle)
        float crankY = crankCenterY + crankRadius * std::cos(angle); // y = y0 + R·cos(angle)
        float pistonY = crankY + std::sqrt(rodLen * rodLen - crankX * crankX);

        // поршень
        m_objects[4].model = Mat3::translation(0.0f, pistonY);

        float rodAngle = std::atan2(crankX, pistonY - crankY);
        // шатун
        m_objects[5].model = Mat3::translation(0.0f, pistonY) * Mat3::rotation(rodAngle);

        // коленвал
        m_objects[6].model = Mat3::translation(0.0f, crankCenterY) * Mat3::rotation(angle);
    }

    void UpdateValves()
    {
        float maxLift = 0.04f;
        float revolutions = angle / (2.0f * M_PI);
        int turnNumber = static_cast<int>(std::floor(revolutions));

        bool leftActive = (turnNumber % 2 == 0);
        bool rightActive = !leftActive;

        float liftLeft = 0.0f;
        float liftRight = 0.0f;

        if (leftActive)
        {
            liftLeft = std::max(0.0f, std::sin(angle));
        }
        if (rightActive)
        {
            liftRight = std::max(0.0f, std::sin(angle + float(M_PI) * 1.15f));
        }

        m_objects[10].model = Mat3::translation(-0.1f, 0.12f - liftLeft * maxLift);
        m_objects[11].model = Mat3::translation(0.1f, 0.12f - liftRight * maxLift);
    }

    void UpdateExhaustParticles()
    {
        float baseX = 0.1f;
        float baseY = 0.15f;
        float globalPhaseShift = -0.7f;
        float revolutions = angle / (2.0f * M_PI);
        int turnNumber = static_cast<int>(std::floor(revolutions));
        bool rightActive = (turnNumber % 2 != 0);

        for (int i = 0; i < particleCount; ++i)
        {
            float intensity = 0.0f;
            if (rightActive)
            {
                float phaseShift = globalPhaseShift + i * 0.3f;
                float shiftedLift = std::max(0.0f, std::sin(angle + float(M_PI) + phaseShift));
                float threshold = 0.93f;
                if (shiftedLift > threshold)
                {
                    intensity = (shiftedLift - threshold) / (1.0f - threshold);
                }
            }
            if (exhaustIndices[i] >= 0)
            {
                float yOffset = i * 0.03f;
                m_objects[exhaustIndices[i]].model =
                        Mat3::translation(baseX, baseY + 0.17f - yOffset) *
                        Mat3::scale(intensity, intensity);
            }
        }
    }

    void UpdateFlash()
    {
        float flashIntensity = 0.0f;
        float revolutions = angle / (2.0f * M_PI);
        float nearestInt = std::round(revolutions);
        float distance = std::abs(angle - nearestInt * 2.0f * M_PI);
        float epsilon = 0.4f;

        if (distance < epsilon && static_cast<int>(nearestInt) % 2 == 1)
        {
            flashIntensity = 1.0f;
        }

        if (flashIndex >= 0)
        {
            m_objects[flashIndex].model =
                    Mat3::translation(0.0f, 0.1f) *
                    Mat3::scale(flashIntensity, flashIntensity);
        }
    }
};
