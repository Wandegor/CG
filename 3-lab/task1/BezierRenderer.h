#pragma once
#include <glad/glad.h>
#include <vector>
#include "Bezier.h"
#include "Common/Point.h"

class BezierRenderer
{
private:
    GLuint curveVAO, curveVBO;
    GLuint pointsVAO, pointsVBO;
    GLuint dashVAO, dashVBO;
    size_t curveCount;
    size_t dashCount;

    static void CreateBuffer(const std::vector<Point>& points, GLuint& VAO, GLuint& VBO)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void *)nullptr);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    static void CreateBufferFixed(const Point* points, size_t count, GLuint& VAO, GLuint& VBO)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(Point), points, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void *)nullptr);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

public:
    BezierRenderer(Bezier& bezier)
    {
        auto curvePoints = bezier.GenerateCurvePoints();
        auto dashPoints = bezier.GenerateDashedLines();
        curveCount = curvePoints.size();
        dashCount = dashPoints.size();

        CreateBuffer(curvePoints, curveVAO, curveVBO);
        CreateBufferFixed(bezier.ControlPoints, 4, pointsVAO, pointsVBO);
        CreateBuffer(dashPoints, dashVAO, dashVBO);
    }

    void DrawDashes(GLint colorLoc) const
    {
        glUniform4f(colorLoc, 0.6f, 0.6f, 0.6f, 1.0f);
        glBindVertexArray(dashVAO);
        glDrawArrays(GL_LINES, 0, dashCount);
    }

    void DrawCurve(GLint colorLoc) const
    {
        glUniform4f(colorLoc, 0.0f, 0.0f, 1.0f, 1.0f);
        glBindVertexArray(curveVAO);
        glDrawArrays(GL_LINE_STRIP, 0, curveCount);
    }

    void DrawPoints(GLint colorLoc) const
    {
        glUniform4f(colorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
        glBindVertexArray(pointsVAO);
        glDrawArrays(GL_POINTS, 0, 1);
        glDrawArrays(GL_POINTS, 1, 1);
        glDrawArrays(GL_POINTS, 2, 1);
        glDrawArrays(GL_POINTS, 3, 1);
    }

    void UpdateCurve(const std::vector<Point>& points) {
        curveCount = points.size();
        glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(Point), points.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void UpdateDashes(const std::vector<Point>& points) {
        dashCount = points.size();
        glBindBuffer(GL_ARRAY_BUFFER, dashVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(Point), points.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void UpdatePoints(const Point* points, size_t count) {
        glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Point), points);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ~BezierRenderer()
    {
        glDeleteVertexArrays(1, &curveVAO);
        glDeleteBuffers(1, &curveVBO);
        glDeleteVertexArrays(1, &pointsVAO);
        glDeleteBuffers(1, &pointsVBO);
        glDeleteVertexArrays(1, &dashVAO);
        glDeleteBuffers(1, &dashVBO);
    }
};
