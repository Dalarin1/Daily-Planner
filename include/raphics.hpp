#pragma once
#include "Primitives.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>

class Button
{
public:
    Color base_bkg_color;
    Color base_border_color;
    Color current_bkg_color;
    Color current_border_color;

    Rectangle Bounds;
    std::function<void(Button *)> OnClick;
    std::function<void(Button *)> OnHover;

    bool is_hovered = false;

    GLuint vbo;
    GLuint vao;
    bool inited;
    GLfloat points[12];

    Button(vector3 _pos = vector3(0, 0, 0),
           vector3 _size = vector3(0, 0, 0),
           Color _backgroundColor = Color(255, 255, 255),
           Color _borderColor = Color(0, 0, 0),
           std::function<void(Button *)> _onClick = nullptr,
           std::function<void(Button *)> _onHover = nullptr)
    {
        base_bkg_color = _backgroundColor;
        base_border_color = _borderColor;
        current_bkg_color = base_bkg_color;
        current_border_color = base_border_color;

        Bounds = Rectangle(_pos, _size);
        OnClick = _onClick;
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
    void CheckHover(double mouseX, double mouseY, int windowWidth, int windowHeight)
    {
        bool newHoverState = IsHovered(mouseX, mouseY, windowWidth, windowHeight);

        if (newHoverState && !is_hovered)
        {
            // Только при наведении
            if (OnHover)
                OnHover(this);
            is_hovered = true;
        }
        else if (!newHoverState && is_hovered)
        {
            // Только при снятии наведения
            ResetColors();
            is_hovered = false;
        }
    }
    bool IsHovered(double mouseX, double mouseY, int windowWidth, int windowHeight)
    {
        float xabs = 2 * (mouseX / windowWidth) - 1;
        float yabs = 1 - 2 * (mouseY / windowHeight);
        return Bounds.Contains(vector3(xabs, yabs, 0));
    }
    void Draw(unsigned int shaderProgram) const
    {
        GLint colorloc = glGetUniformLocation(shaderProgram, "color");
        glUniform4f(colorloc,
                    current_bkg_color.Red / 255.0f,
                    current_bkg_color.Green / 255.0f,
                    current_bkg_color.Blue / 255.0f,
                    1.0f);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glUniform4f(colorloc,
                    current_border_color.Red / 255.0f,
                    current_border_color.Green / 255.0f,
                    current_border_color.Blue / 255.0f,
                    1.0f);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindVertexArray(0);
    }
    void ResetColors()
    {
        std::cout << "RESET COLOR\n";
        current_bkg_color = base_bkg_color;
        current_border_color = base_border_color;
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

class Checkbox
{
public:
    bool Checked;
    Button BoxButton;
    Color CheckedColor;
    Color UncheckedColor;

    Checkbox(vector3 pos = vector3(0,0,0),
             vector3 size = vector3(0.05, 0.05, 0),
             Color uncheckedColor = Color(),
             Color checkedColor = Color(0, 255, 0),
             std::function<void(Checkbox *)> onClick = nullptr)
        : Checked(false),
          BoxButton(pos, size, uncheckedColor, Color(0, 0, 0)),
          CheckedColor(checkedColor),
          UncheckedColor(uncheckedColor)
    {
        BoxButton.OnClick = [this, onClick](Button *btn)
        {
            Checked = !Checked;
            UpdateAppearance();

            if (onClick)
            {
                onClick(this);
            }
        };
        BoxButton.OnHover = [this](Button *btn)
        {
            btn->current_bkg_color = btn->base_bkg_color.Lerp(
                Checked ? CheckedColor : UncheckedColor,
                25.0f);
        };

        UpdateAppearance();
    }

    void UpdateAppearance()
    {
        BoxButton.base_bkg_color = Checked ? CheckedColor : UncheckedColor;
        BoxButton.current_bkg_color = BoxButton.base_bkg_color;
    }

    void Draw(unsigned int shaderProgram) const
    {
        BoxButton.Draw(shaderProgram);
    }

    void CheckHover(double mouseX, double mouseY, int windowWidth, int windowHeight)
    {
        BoxButton.CheckHover(mouseX, mouseY, windowWidth, windowHeight);
    }

};