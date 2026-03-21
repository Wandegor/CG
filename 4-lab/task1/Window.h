#pragma once
#include "BaseWindow.h"
#include "StarPolyhedron.h"

class Window : public BaseWindow
{
public:
	Window(int w, int h, const char* title);

private:
	void OnMouseButton(int button, int action, int mods) override;

	void OnMouseMove(double x, double y) override;

	void OnResize(int width, int height) override;

	void OnRunStart() override;

	void Draw(int width, int height) override;

	void RotateCamera(double xAngleRadians, double yAngleRadians);
	void SetupLighting();

	StarPolyhedron m_star;

	bool m_leftMouseButtonPressed = false;
	glm::dvec2 m_lastMousePos{};

	static constexpr double DISTANCE_TO_ORIGIN = 3;
	glm::dmat4x4 m_cameraMatrix;
};
