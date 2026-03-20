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

	StarPolyhedron m_star;
	bool m_leftMouseButtonPressed = false;
	double m_lastMouseX = 0, m_lastMouseY = 0;
	double m_rotateX = 0, m_rotateY = 0;
};
