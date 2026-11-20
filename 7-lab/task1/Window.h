#pragma once
#include "BaseWindow.h"
#include "Presenter.h"
#include "RenderObject.h"
#include "Shaders/Shader.h"

class Window : public BaseWindow
{
public:
    Window(int w, int h, const char* title, Presenter& presenter);

private:
    void OnKey(int key, int scancode, int action, int mods) override;
    void OnMouseButton(int button, int action, [[maybe_unused]] int mods) override;

    void OnMouseMove(double x, double y) override;


    void OnResize(int width, int height) override;

    void OnRunStart() override;

    void Draw(int width, int height) override;
    void SetupLighting();

    glm::dvec2 m_mousePos = {};

    Presenter& m_presenter;
    double m_lastTime;

    std::vector<GLuint> m_wallTextures;

    std::unique_ptr<Shader> m_shader;

    std::unique_ptr<RenderObject> m_canabola;
};
