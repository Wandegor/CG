#pragma once
#include "BaseWindow.h"
#include "Presenter.h"

class Window : public BaseWindow
{
public:
	Window(int w, int h, const char* title, Presenter& presenter);

private:
	GLuint m_wallDisplayList = 0;
	void BuildBoardDisplayList(const Model& model);
	void RenderBoard(const Model& model);
	void DrawTile(float width, float depth, float height);

    void OnKey(int key, int scancode, int action, int mods) override;
	void OnMouseButton(int button, int action, [[maybe_unused]] int mods) override;

	void OnMouseMove(double x, double y) override;

	void OnResize(int width, int height) override;

	void OnRunStart() override;

	void Draw(int width, int height) override;
	void SetupLighting();

	void SetupCameraMatrix();

    std::pair<glm::dvec3, glm::dvec3> GetMouseRay(double mouseX, double mouseY);

	bool m_leftButtonPressed = false;
	glm::dvec2 m_mousePos = {};

    Presenter& m_presenter;
	double m_lastTime;

	std::vector<GLuint> m_wallTextures;
	GLuint LoadTexture(const char* path);
};
