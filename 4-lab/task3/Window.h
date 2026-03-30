#pragma once
#include "BaseWindow.h"
#include "MoebiusStrip.h"

class Window : public BaseWindow
{
public:
	Window(int w, int h, const char* title);

private:
    bool IsKeyPressed(int key) const;
    void UpdateMovement(float deltaTime);

	void OnMouseButton(int button, int action, [[maybe_unused]] int mods) override;

	void OnMouseMove(double x, double y) override;

	void RotateCamera(double xAngleRadians, double yAngleRadians);

	void OnResize(int width, int height) override;

	void OnRunStart() override;

	void Draw(int width, int height) override;
	void SetupLighting();

	void SetupCameraMatrix();

	MoebiusStrip m_strip;


	bool m_leftButtonPressed = false;
	glm::dvec2 m_mousePos = {};
	glm::dmat4x4 m_cameraMatrix;

    double m_lastTime = 0.0;
    float m_moveSpeed = 3.0f;
};
