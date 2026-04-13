#pragma once
#include "BaseWindow.h"
#include "IView.h"
#include "Model.h"

class Presenter;

class Window : public BaseWindow, public IView
{
public:
	Window(int w, int h, const char* title);

	void SetPresenter(std::shared_ptr<Presenter> presenter);

private:
	GLuint m_wallDisplayList = 0;
	void RenderBoard(float dt);
	void DrawTile(float width, float depth, float height, GLuint textureID);

    void OnKey(int key, int scancode, int action, int mods) override;
	void OnMouseButton(int button, int action, [[maybe_unused]] int mods) override;

	void OnMouseMove(double x, double y) override;

	void OnResize(int width, int height) override;

	void OnRunStart() override;

	void Draw(int width, int height) override;
	void SetupLighting();

	void SetupCameraMatrix();

    std::pair<glm::dvec3, glm::dvec3> GetMouseRay(double mouseX, double mouseY);

	void Redraw() override;

	bool m_leftButtonPressed = false;
	glm::dvec2 m_mousePos = {};

	std::shared_ptr<Presenter> m_presenter;
	double m_lastTime;

	std::vector<GLuint> m_wallTextures;
	GLuint LoadTexture(const char* path);

	std::vector<std::vector<float>> m_tileAngles;
	float m_animationSpeed = 500.0f;
};
