#include "GLFWInitializer.h"
#include "Window.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
int main()
{
	GLFWInitializer initGLFW;
	Presenter presenter(16, 16);
	Window window{ 800, 600, "3D Cube", presenter };
	window.Run();
}
