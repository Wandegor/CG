#include "pch.h"
#include "Window.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

#include "Presenter.h"

namespace
{
    // Угол обзора по вертикали
    constexpr double FIELD_OF_VIEW = 60 * M_PI / 180.0;

    constexpr double Z_NEAR = 0.05;
    constexpr double Z_FAR = 50;

    float pieceSize = 1.0f;
    float pieceHeight = 0.3f;

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

Window::Window(int w, int h, const char* title)
    : BaseWindow(w, h, title),
      m_lastTime(glfwGetTime()) {}

void Window::OnKey(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
    }

    m_presenter->OnKey(key, action);
}

void Window::OnMouseButton(int button, int action, int mods)
{
}

void Window::OnMouseMove(double x, double y)
{
    m_presenter->OnMouseMove(x, y);
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

    // смешивание Цвета с текстурой
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    m_wallTextures.push_back(LoadTexture("Textures/wall1.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/wall2.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/wall3.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/wall4.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/wall5.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/wall6.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/sniper.jpg"));
    m_wallTextures.push_back(LoadTexture("Textures/toxis.jpg"));
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

    const GLfloat globalAmbient[] = {0.25f, 0.25f, 0.25f, 1.0f};
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

void Window::Draw(int width, int height)
{
    double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - m_lastTime);
    m_lastTime = currentTime;

    if (deltaTime > 0.1f) deltaTime = 0.1f;

    if (m_presenter)
    {
        m_presenter->Update(deltaTime);
    }

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetupCameraMatrix();

    GLfloat lightPosition[] = { 2.0f, 3.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    RenderBoard(deltaTime);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Window::DrawTile(float width, float depth, float height)
{
    if (m_presenter->IsAnimating())
    {
        Move move = m_presenter->GetCurrentMove();
        // Не рисовать статично фигуру которая в анимации
    }
    float hw = width / 2.0f;
    float hd = depth / 2.0f;

    glBegin(GL_QUADS);

    // Верхняя грань (Рубашка)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-hw, height, -hd);
    glVertex3f(-hw, height,  hd);
    glVertex3f( hw, height,  hd);
    glVertex3f( hw, height, -hd);

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

    // Нижняя (лицо)
}

void Window::RenderBoard(float dt)
{
    const auto& model = m_presenter->GetModel();

    int boardSize = 8;
    float fullSize = boardSize * pieceSize;
    float startX = -fullSize / 2.0f + pieceSize / 2.0f;
    float startZ = -fullSize / 2.0f + pieceSize / 2.0f;

    // Доска
    for (int r = 0; r < boardSize; ++r) {
        for (int c = 0; c < boardSize; ++c) {

            float x = startX + c * pieceSize;
            float z = startZ + r * pieceSize;

            // id для текстуры
            // int tileId = model.GetTileId(r, c);
            // GLuint currentTexture = m_wallTextures[m_wallTextures.size()];

            glPushMatrix();
            glTranslatef(x, 0.0f, z);

            if ((r + c) % 2 == 0) glColor3f(0.3f, 0.3f, 0.3f);
            else glColor3f(0.8f, 0.8f, 0.8f);

            DrawTile(pieceSize, pieceSize, 0.05f);
            glPopMatrix();
        }
    }

    // Фигуры
    for (int r = 0; r < boardSize; ++r) {
        for (int c = 0; c < boardSize; ++c) {

            Piece piece = model.GetPiece(r, c);
            if (piece.isEmpty) continue;

            // Пропуск той что анимируется
            if (m_presenter->IsAnimating()) {
                Move move = m_presenter->GetCurrentMove();
                if (r == move.from.row && c == move.from.col) continue;
            }

            float x = startX + c * pieceSize;
            float z = startZ + r * pieceSize;

            // id для текстуры
            // int tileId = model.GetTileId(r, c);
            // GLuint currentTexture = m_wallTextures[m_wallTextures.size()];

            glPushMatrix();
            glTranslatef(x, 0.05f, z);

            if (piece.color == PieceColor::White) glColor3f(1.0f, 1.0f, 0.9f);
            else glColor3f(0.1f, 0.1f, 0.1f);

            DrawTile(pieceSize/2, pieceSize/2, pieceHeight);
            glPopMatrix();
        }
    }

    glPushMatrix();

    float worldX = startX + m_presenter->GetAnimX() * pieceSize;
    float worldZ = startZ + m_presenter->GetAnimZ() * pieceSize;

    glTranslatef(worldX, 0.05f, worldZ);

    Move move = m_presenter->GetCurrentMove();
    Piece piece = model.GetPiece(move.from.row, move.from.col);

    if (piece.color == PieceColor::White) glColor3f(1.0f, 1.0f, 0.9f);
    else glColor3f(0.1f, 0.1f, 0.1f);

    DrawTile(pieceSize/2, pieceSize/2, pieceHeight);
    glPopMatrix();

}

void Window::Redraw()
{
    // std::cout << "C" ;
}

void Window::SetPresenter(std::shared_ptr<Presenter> presenter)
{
    m_presenter = presenter;
}
