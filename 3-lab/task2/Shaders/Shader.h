#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <glad/gl.h>
#include "../Common/Util.h"

class Shader
{
    GLuint program;

    static void AddShader(GLuint shaderProgram, const char* pShaderText, GLenum shaderType)
    {
        GLuint shader = glCreateShader(shaderType);

        if (shader == 0)
        {
            fprintf(stderr, "Error creating shader%d\n", shaderType);
            exit(0);
        }

        const GLchar* p[1];
        p[0] = pShaderText;

        GLint Length[1];
        Length[0] = std::strlen(pShaderText);

        glShaderSource(shader, 1, p, Length);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            GLchar infoLog[1024];
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            fprintf(stderr, "Error compiling shader type %d:\n%s\n", shaderType, infoLog);
            exit(1);
        }

        glAttachShader(shaderProgram, shader);
        glDeleteShader(shader);
    }

    // Проверка линковки программы
    static void CheckProgramLinking(unsigned int program)
    {
        int success;
        char infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (success == 0)
        {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "Error linking shader program\n" << infoLog << std::endl;
            exit(1);
        }

        glValidateProgram(program);
        glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
        if (success == 0)
        {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "Error validating shader program\n" << infoLog << std::endl;
            exit(1);
        }
    }

public:
    ~Shader() { glDeleteProgram(program); }

    Shader(const char* vsFile, const char* fsFile)
    {
        program = glCreateProgram();
        if (program == 0)
        {
            fprintf(stderr, "Error creating shader program\n");
            exit(1);
        }

        std::string vs, fs;

        if (!Util::ReadFile(vsFile, vs)) exit(1);
        AddShader(program, vs.c_str(), GL_VERTEX_SHADER);

        if (!Util::ReadFile(fsFile, fs)) exit(1);
        AddShader(program, fs.c_str(), GL_FRAGMENT_SHADER);

        glLinkProgram(program);
        CheckProgramLinking(program);
    }

    [[nodiscard]] GLuint GetProgram() const { return program; }
};
