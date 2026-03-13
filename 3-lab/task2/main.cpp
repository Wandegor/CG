#include <glad/gl.h>
#include "Window.h"
#include "SceneRenderer.h"
#include "Shaders/Shader.h"

const char *pVSFileName = "shader.vs";
const char *pFSFileName = "shader.fs";

int main()
{
    Window window(1000, 1000, "Engine Cutaway");

    if (!gladLoadGL(glfwGetProcAddress))
        return -1;

    Shader shader("shader.vs", "shader.fs");

    SceneRenderer renderer(&shader);

    float lastTime = glfwGetTime();

    while (!window.ShouldClose())
    {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        window.ProcessInput();

        glClearColor(0.9f,0.9f,0.9f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.Update(deltaTime);
        renderer.DrawScene();

        window.SwapBuffers();
        window.PollEvents();
    }
}
