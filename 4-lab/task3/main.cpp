#include "GLFWInitializer.h"
#include "Window.h"

int main()
{
	GLFWInitializer initGLFW;
	Presenter presenter(10, 10);
	Window window{ 800, 600, "3D Cube", presenter };
	window.Run();
}
