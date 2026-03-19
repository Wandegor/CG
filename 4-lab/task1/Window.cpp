#include "pch.h"
#include "Window.h"

namespace
{
// Угол обзора по вертикали
constexpr double FIELD_OF_VIEW = 60 * M_PI / 180.0;
// Размер стороны куба
constexpr double STAR_SIZE = 1;

constexpr double Z_NEAR = 0.1;
constexpr double Z_FAR = 10;

// Ортонормируем матрицу 4*4 (это должна быть аффинная матрица)
glm::dmat4x4 Orthonormalize(const glm::dmat4x4& m)
{
	// Извлекаем подматрицу 3*3 из матрицы m и ортонормируем её
	const auto normalizedMatrix = glm::orthonormalize(glm::dmat3x3{ m });
	// Заменяем 3 столбца исходной матрицы
	return {
		glm::dvec4{ normalizedMatrix[0], 0.0 },
		glm::dvec4{ normalizedMatrix[1], 0.0 },
		glm::dvec4{ normalizedMatrix[2], 0.0 },
		m[3]
	};
}

} // namespace

Window::Window(int w, int h, const char* title)
	: BaseWindow(w, h, title)
	, m_star(STAR_SIZE)
{
	m_star.SetSideColor(0, 255, 0, 0);      // Красный
	m_star.SetSideColor(1, 0, 255, 0);      // Зеленый
	m_star.SetSideColor(2, 0, 0, 255);      // Синий
	m_star.SetSideColor(3, 255, 255, 0);    // Желтый
	m_star.SetSideColor(4, 255, 0, 255);    // Пурпурный
	m_star.SetSideColor(5, 0, 255, 255);    // Циан
	m_star.SetSideColor(6, 255, 128, 0);    // Оранжевый
	m_star.SetSideColor(7, 128, 0, 255);    // Фиолетовый
	m_star.SetSideColor(8, 0, 128, 128);    // Морская волна
	m_star.SetSideColor(9, 128, 128, 0);    // Оливковый
	m_star.SetSideColor(10, 255, 192, 203); // Розовый
	m_star.SetSideColor(11, 128, 128, 128); // Серый
}

void Window::OnMouseButton(int button, int action, int mods)  {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		m_leftMouseButtonPressed = (action == GLFW_PRESS);
		// Запоминаем позицию в момент нажатия, чтобы не было прыжка
		glfwGetCursorPos(GetWindow(), &m_lastMouseX, &m_lastMouseY);
	}
}

void Window::OnMouseMove(double x, double y)  {
	if (m_leftMouseButtonPressed) {
		// Вычисляем, насколько сместилась мышь
		m_rotateY += static_cast<float>(x - m_lastMouseX) * 0.5f; // Чувствительность
		m_rotateX += static_cast<float>(y - m_lastMouseY) * 0.5f;
	}
	m_lastMouseX = x;
	m_lastMouseY = y;
}

void Window::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	const auto proj = glm::perspective(FIELD_OF_VIEW, aspect, Z_NEAR, Z_FAR);
	glLoadMatrixd(&proj[0][0]);
	glMatrixMode(GL_MODELVIEW);
}

void Window::OnRunStart()
{
	// // Включаем режим отбраковки граней
	// glEnable(GL_CULL_FACE);
	// // Отбраковываться будут нелицевые стороны граней
	// glCullFace(GL_FRONT);
	// // Сторона примитива считается лицевой, если при ее рисовании
	// // обход верших осуществляется против часовой стрелки
	// glFrontFace(GL_CCW);
	//
	// // Включаем тест глубины для удаления невидимых линий и поверхностей
	glEnable(GL_DEPTH_TEST);
}

void Window::Draw(int width, int height)
{
	glClearColor(1,1,1,255);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glm::dmat4 mat = glm::lookAt(glm::dvec3{ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });

	mat = glm::rotate(mat, glm::radians(m_rotateX), glm::dvec3{ 1, 0, 0 });
	mat = glm::rotate(mat, glm::radians(m_rotateY), glm::dvec3{ 0, 1, 0 });

	glLoadMatrixd(&mat[0][0]);

	m_star.Draw();
}

void Window::SetupProjectionMatrix(int width, int height)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	const auto projMat = glm::perspective(60.0 * M_PI / 180.0, aspect, 0.1, 10.0);
	glLoadMatrixd(&projMat[0][0]);
}

