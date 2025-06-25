#pragma once
#include "Primitives.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Button
{
public:
    Color BackgroundColor;
    Color BorderColor;
    Rectangle Bounds;
    void (*Onclick)(Button *);
    void (*OnHover)(Button *);

    GLuint vbo;
    GLuint vao;
    bool inited;
    GLfloat points[12];

    Button(vector3 _pos = vector3(0, 0, 0),
           vector3 _size = vector3(0, 0, 0),
           Color _backgroundColor = Color(255, 255, 255),
           Color _borderColor = Color(0, 0, 0),
           void (*_onClick)(Button *) = nullptr,
           void (*_onHover)(Button *) = nullptr)
    {
        BackgroundColor = _backgroundColor;
        BorderColor = _borderColor;
        Bounds = Rectangle(_pos, _size);
        Onclick = _onClick;
        OnHover = _onHover;

        // Инициализация OpenGL объектов
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        inited = true;

        UpdateGeometry();
    }
    void UpdateGeometry()
    {
        for (int i = 0; i < 4; i++)
        {
            points[3 * i] = Bounds.Points[i].X;
            points[3 * i + 1] = Bounds.Points[i].Y;
            points[3 * i + 2] = Bounds.Points[i].Z;
        }

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    bool IsHovered(double mouseX, double mouseY, int windowWidth, int windowHeight)
    {
        float xabs = 2 * (mouseX / windowWidth) - 1;
        float yabs = 1 - 2 * (mouseY / windowHeight);
        return Bounds.Contains(vector3(xabs, yabs, 0));
    }
    void MoveToCursor(double x, double y, int windowWidth, int widnowHeight)
    {
        Bounds.Location = vector3((float)(2 * x / windowWidth - 1), ((float)(1 - 2 * y / widnowHeight)), 0);
        Bounds.Update();
        UpdateGeometry();
    }
    void Draw(unsigned int shaderProgram)
    {
        GLint colorloc = glGetUniformLocation(shaderProgram, "color");
        glUniform4f(colorloc,
                    BackgroundColor.Red,
                    BackgroundColor.Green,
                    BackgroundColor.Blue,
                    1.0f);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glUniform4f(colorloc,
                    BorderColor.Red,
                    BorderColor.Green,
                    BorderColor.Blue,
                    1.0f);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindVertexArray(0);
    }
    ~Button()
    {
        if (inited)
        {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
        }
    }
};