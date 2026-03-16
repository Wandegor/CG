#include <glad/gl.h>
#include "Window.h"
#include "SceneRenderer.h"
#include "Shaders/Shader.h"

int main()
{

    int SCR_WIDTH = 1000;
    int SCR_HEIGHT = 1000;

    Window window(SCR_WIDTH, SCR_HEIGHT, "Engine");

    if (!gladLoadGL(glfwGetProcAddress))
        return -1;

    Shader shader("shader2.vs", "shader2.fs");

    SceneRenderer renderer(&shader, static_cast<float>(SCR_HEIGHT)/static_cast<float>(SCR_WIDTH));

    float lastTime = glfwGetTime();

    while (!window.ShouldClose())
    {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        window.ProcessInput();

        float aspect = static_cast<float>(window.GetHeight()) / static_cast<float>(window.GetWidth());
        renderer.SetAspect(aspect);

        glClearColor(0.9f,0.9f,0.9f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.Update(deltaTime);
        renderer.DrawScene();

        window.SwapBuffers();
        window.PollEvents();
    }
}
