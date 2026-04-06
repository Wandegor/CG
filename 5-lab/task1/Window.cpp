#include "pch.h"
#include "Window.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace
{
    // Угол обзора по вертикали
    constexpr double FIELD_OF_VIEW = 60 * M_PI / 180.0;

    constexpr double Z_NEAR = 0.05;
    constexpr double Z_FAR = 50;

    // Ортонормируем матрицу 4*4 (это должна быть аффинная матрица)
    glm::dmat4x4 Orthonormalize(const glm::dmat4x4& m)
    {
        // Извлекаем подматрицу 3*3 из матрицы m и ортонормируем её
        const auto normalizedMatrix = glm::orthonormalize(glm::dmat3x3{m});
        // Заменяем 3 столбца исходной матрицы
        return {
            glm::dvec4{normalizedMatrix[0], 0.0},
            glm::dvec4{normalizedMatrix[1], 0.0},
            glm::dvec4{normalizedMatrix[2], 0.0},
            m[3]
        };
    }
} // namespace

Window::Window(int w, int h, const char* title, Presenter& presenter)
    : BaseWindow(w, h, title), m_presenter(presenter),
      m_lastTime(glfwGetTime()) {}

void Window::BuildMazeDisplayList(const MazeModel& model)
{
    if (m_wallDisplayList != 0)
    {
        glDeleteLists(m_wallDisplayList, 1);
    }

    m_wallDisplayList = glGenLists(1);
    glNewList(m_wallDisplayList, GL_COMPILE);

    int width = model.GetWidth();
    int height = model.GetHeight();

    // Пол
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.6f, 0.8f);

    glNormal3f(0.0f, 1.0f, 0.0f);

    glVertex3f(0.0f,  0.0f, 0.0f);
    glVertex3f(0.0f,  0.0f, height);
    glVertex3f(width, 0.0f, height);
    glVertex3f(width, 0.0f, 0.0f);
    glEnd();

    // Включение текстур
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_wallTexture);

    glBegin(GL_QUADS);
    for (int x = 0; x < width; ++x)
    {
        for (int z = 0; z < height; ++z)
        {
            if (!model.IsWall(x, z)) continue;

            // Приводим индексы к float для координат
            auto fx = static_cast<float>(x);
            auto fz = static_cast<float>(z);

            // Передняя грань (Z-)
            if (!model.IsWall(x, z - 1)) {
                glNormal3f(0, 0, -1);
                glTexCoord2f(0.0f, 0.0f); glVertex3f(fx,     0, fz); // Лево-низ
                glTexCoord2f(0.0f, 1.0f); glVertex3f(fx,     1, fz); // Лево-верх
                glTexCoord2f(1.0f, 1.0f); glVertex3f(fx + 1, 1, fz); // Право-верх
                glTexCoord2f(1.0f, 0.0f); glVertex3f(fx + 1, 0, fz); // Право-низ
            }

            // Задняя (Z+)
            if (!model.IsWall(x, z + 1)) {
                glNormal3f(0, 0, 1);
                glTexCoord2f(0.0f, 0.0f); glVertex3f(fx + 1, 0, fz + 1); // Лево-низ
                glTexCoord2f(0.0f, 1.0f); glVertex3f(fx + 1, 1, fz + 1); // Лево-верх
                glTexCoord2f(1.0f, 1.0f); glVertex3f(fx,     1, fz + 1); // Право-верх
                glTexCoord2f(1.0f, 0.0f); glVertex3f(fx,     0, fz + 1); // Право-низ
            }

            // Левая (X-)
            if (!model.IsWall(x - 1, z)) {
                glNormal3f(-1, 0, 0);
                glTexCoord2f(0.0f, 0.0f); glVertex3f(fx, 0, fz + 1); // Лево-низ
                glTexCoord2f(0.0f, 1.0f); glVertex3f(fx, 1, fz + 1); // Лево-верх
                glTexCoord2f(1.0f, 1.0f); glVertex3f(fx, 1, fz);     // Право-верх
                glTexCoord2f(1.0f, 0.0f); glVertex3f(fx, 0, fz);     // Право-низ
            }

            // Правая (X+)
            if (!model.IsWall(x + 1, z)) {
                glNormal3f(1, 0, 0);
                glTexCoord2f(0.0f, 0.0f); glVertex3f(fx + 1, 0, fz);     // Лево-низ
                glTexCoord2f(0.0f, 1.0f); glVertex3f(fx + 1, 1, fz);     // Лево-верх
                glTexCoord2f(1.0f, 1.0f); glVertex3f(fx + 1, 1, fz + 1); // Право-верх
                glTexCoord2f(1.0f, 0.0f); glVertex3f(fx + 1, 0, fz + 1); // Право-низ
            }
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEndList();
}

void Window::RenderMaze(const MazeModel& model)
{
    if (m_wallDisplayList == 0)
    {
        BuildMazeDisplayList(model);
    }

    glColor3f(0.6f, 0.2f, 0.6f);
    glCallList(m_wallDisplayList);
}

void Window::OnKey(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
    }

    m_presenter.OnKey(key, action);
}

void Window::OnMouseButton(int button, int action, int mods)
{
    m_presenter.OnMouseButton(button, action);
}

void Window::OnMouseMove(double x, double y)
{
    m_presenter.OnMouseMove(x, y);
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
    // Включаем режим отбраковки граней
    // glEnable(GL_CULL_FACE);
    // Отбраковываться будут нелицевые стороны граней
    // glCullFace(GL_BACK);
    // Сторона примитива считается лицевой, если при ее рисовании
    // обход верших осуществляется против часовой стрелки
    glFrontFace(GL_CCW);

    // Включаем тест глубины для удаления невидимых линий и поверхностей
    glEnable(GL_DEPTH_TEST);

    SetupLighting();

    m_wallTexture = LoadTexture("Textures/wall2.jpg");
}

void Window::SetupLighting()
{
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Расчет света для обоих сторон полигона
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.15f); // Чем больше число, тем быстрее гаснет свет
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05f);

    const GLfloat globalAmbient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    const GLfloat lightAmbient[] = {0.15f, 0.15f, 0.15f, 1.0f};
    const GLfloat lightDiffuse[] = {0.60f, 0.60f, 0.60f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

GLuint Window::LoadTexture(const char* path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Загрузка через stb_image
    int width, height, channels;
    // Флипаем текстуру по вертикали, так как в OpenGL координата Y идет снизу вверх
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

    if (data)
    {
        // Если картинка без альфа-канала (RGB), используем GL_RGB. Если PNG с прозрачностью - GL_RGBA
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        // Можно выбросить std::runtime_error, чтобы сразу заметить ошибку пути
        throw std::runtime_error("Failed to load texture!");
    }

    return textureID;
}

void Window::Draw(int width, int height)
{
    double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - m_lastTime);
    m_lastTime = currentTime;

    m_presenter.UpdateMovement(deltaTime);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetupCameraMatrix();

    glm::dvec3 pos = m_presenter.GetCameraPos();
    const GLfloat lightPosition[] = {
        static_cast<GLfloat>(pos.x),
        static_cast<GLfloat>(pos.y),
        static_cast<GLfloat>(pos.z),
        1.0f
    };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    RenderMaze(m_presenter.GetModel());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Window::SetupCameraMatrix()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glm::dvec3 pos = m_presenter.GetCameraPos();
    glm::dvec3 front = m_presenter.GetCameraFront();
    glm::dvec3 up = m_presenter.GetCameraUp();

    glm::dmat4 view = glm::lookAt(pos, pos + front, up);
    glLoadMatrixd(&view[0][0]);
}
