#include "pch.h"
#include "Window.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

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

void Window::DrawTile(float width, float depth, float height)
{
    float hw = width / 2.0f;
    float hd = depth / 2.0f;

    glBegin(GL_QUADS);

    // Верхняя грань (Рубашка)
    glColor3f(0.8f, 0.8f, 0.8f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-hw, height, -hd);
    glVertex3f(-hw, height,  hd);
    glVertex3f( hw, height,  hd);
    glVertex3f( hw, height, -hd);

    // Боковые грани (для объема)
    glColor3f(0.6f, 0.6f, 0.6f);

    // Передняя
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-hw, 0.0f,   hd);
    glVertex3f( hw, 0.0f,   hd);
    glVertex3f( hw, height, hd);
    glVertex3f(-hw, height, hd);

    // Задняя
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-hw, height, -hd);
    glVertex3f( hw, height, -hd);
    glVertex3f( hw, 0.0f,   -hd);
    glVertex3f(-hw, 0.0f,   -hd);

    // Правая
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(hw, 0.0f,   -hd);
    glVertex3f(hw, height, -hd);
    glVertex3f(hw, height,  hd);
    glVertex3f(hw, 0.0f,    hd);

    // Левая
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-hw, 0.0f,    hd);
    glVertex3f(-hw, height,  hd);
    glVertex3f(-hw, height, -hd);
    glVertex3f(-hw, 0.0f,   -hd);

    glEnd();
}

void Window::BuildBoardDisplayList(const Model& model)
{
    if (m_wallDisplayList != 0)
    {
        glDeleteLists(m_wallDisplayList, 1);
    }

    m_wallDisplayList = glGenLists(1);
    glNewList(m_wallDisplayList, GL_COMPILE);

    // Настройки сетки

    int rows = model.GetWidth();
    int cols = model.GetHeight();

    float tileSize = 1.0f;
    float tileHeight = 0.3f;
    float spacing = 0.2f;

    // Вычисляем смещение, чтобы центр сетки был в координатах 0,0,0
    float totalWidth = cols * tileSize + (cols - 1) * spacing;
    float totalDepth = rows * tileSize + (rows - 1) * spacing;

    float startX = -totalWidth / 2.0f + tileSize / 2.0f;
    float startZ = -totalDepth / 2.0f + tileSize / 2.0f;

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            float x = startX + c * (tileSize + spacing);
            float z = startZ + r * (tileSize + spacing);

            glPushMatrix();

            glTranslatef(x, 0.0f, z);

            DrawTile(tileSize, tileSize, tileHeight);

            glPopMatrix();
        }
    }

    glEndList();
}

void Window::RenderBoard(const Model& model)
{
    if (m_wallDisplayList == 0)
    {
        BuildBoardDisplayList(model);
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
    int width, height;
    glfwGetFramebufferSize(GetWindow(), &width, &height);

    double xpos, ypos;
    glfwGetCursorPos(GetWindow(), &xpos, &ypos);

    auto [rayOrigin, rayDir] = GetMouseRay(xpos, ypos);

    // P = rayStart + t * rayDirection
    // нужен t при Y = 0
    double t = -rayOrigin.y / rayDir.y;
    if (t > 0)
    {
        // мировые (x, 0, z)
        auto hitPoint = rayOrigin + t * rayDir;
        std::cout << hitPoint << std::endl;

    }

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

    m_wallTextures.push_back(LoadTexture("Textures/wall1.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/wall2.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/wall3.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/wall4.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/wall5.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/wall6.jpg"));
}

void Window::SetupLighting()
{
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Расчет света для обоих сторон полигона
    // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1f); // Чем больше число, тем быстрее гаснет свет
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05f);

    const GLfloat globalAmbient[] = {0.15f, 0.15f, 0.15f, 1.0f};
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

    int width, height, channels;
    // Флип по вертикали
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
        throw std::runtime_error("Failed to load texture!");
    }

    return textureID;
}

void Window::Draw(int width, int height)
{
    double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - m_lastTime);
    m_lastTime = currentTime;

    // m_presenter.UpdateMovement(deltaTime);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetupCameraMatrix();

    GLfloat lightPosition[] = { 2.0f, 3.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    RenderBoard(m_presenter.GetModel());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Window::SetupCameraMatrix()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glm::dvec3 pos(0.0, 7.0, 7.0);
    glm::dvec3 front(0.0, 0.0, 0.0);
    glm::dvec3 up(0.0, 1.0, 0.0);

    glm::dmat4 view = glm::lookAt(pos, front, up);
    glLoadMatrixd(&view[0][0]);
}

// Window.cpp
std::pair<glm::dvec3, glm::dvec3> Window::GetMouseRay(double mouseX, double mouseY) {
    int width, height;
    glfwGetFramebufferSize(GetWindow(), &width, &height);

    float x_ndc = (2.0f * mouseX) / width - 1.0f;
    float y_ndc = 1.0f - (2.0f * mouseY) / height;

    glm::dmat4 projection;
    glGetDoublev(GL_PROJECTION_MATRIX, glm::value_ptr(projection));
    glm::dmat4 view;
    glGetDoublev(GL_MODELVIEW_MATRIX, glm::value_ptr(view));
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Точки на плоскостях отсечения
    glm::dvec3 rayStart_ndc = glm::dvec3(x_ndc, y_ndc, -1.0);
    glm::dvec3 rayEnd_ndc   = glm::dvec3(x_ndc, y_ndc,  1.0);

    glm::dvec3 rayStart_world = glm::unProject(
            rayStart_ndc,
            view,
            projection,
            glm::dvec4(viewport[0], viewport[1], viewport[2], viewport[3]));
    glm::dvec3 rayEnd_world   = glm::unProject(
            rayEnd_ndc,
            view,
            projection,
            glm::dvec4(viewport[0], viewport[1], viewport[2], viewport[3]));

    glm::dvec3 rayDir_world = glm::normalize(rayEnd_world - rayStart_world);
    return {rayStart_world, rayDir_world};
}
