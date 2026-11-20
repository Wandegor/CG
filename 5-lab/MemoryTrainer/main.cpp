#include "GLFWInitializer.h"
#include "Window.h"
#include "Presenter.h"

int main()
{
	GLFWInitializer initGLFW;

	auto window = std::make_shared<Window>(800, 600, "Memory Game");

	auto presenter = std::make_shared<Presenter>(4, 4, *window);

	window->SetPresenter(presenter);

	window->Run();
}
