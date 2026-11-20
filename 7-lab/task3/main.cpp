#include "GLFWInitializer.h"
#include "Window.h"
int main()
{
	GLFWInitializer initGLFW;
	Presenter presenter{};
	Window window{ 800, 600, "3D Cube", presenter };
	window.Run();
}
