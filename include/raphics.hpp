#pragma once
#include "TextRender.hpp"
#include <functional>


class IUIElement
{
public:
    virtual ~IUIElement() = default;
    virtual void draw(unsigned int shader_program) const = 0;
    virtual void update(double mouseX, double mouseY, int windowWidth, int windowHeight) = 0;
    virtual void handle_click() = 0;
    virtual bool contains_point(double x, double y, int windowWidth, int windowHeight) const = 0;
};

class Button : public IUIElement
{
public:
    Color base_bkg_color;
    Color base_border_color;
    Color current_bkg_color;
    Color current_border_color;

    Rectangle Bounds;
    std::function<void()> OnClick;
    std::function<void()> OnHover;

    bool is_hovered = false;

    GLuint vbo;
    GLuint vao;
    bool inited;
    GLfloat points[12];

    Button(vector3 _pos = vector3(0, 0, 0),
           vector3 _size = vector3(0, 0, 0),
           Color _backgroundColor = Color(255, 255, 255),
           Color _borderColor = Color(0, 0, 0),
           std::function<void()> _onClick = nullptr,
           std::function<void()> _onHover = nullptr)
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

        update_geometry();
    }

    void update_geometry()
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
    void handle_click()
    {
        if (OnClick)
            OnClick();
    }
    void update(double mouseX, double mouseY, int windowWidth, int windowHeight) override
    {
        bool newHoverState = contains_point(mouseX, mouseY, windowWidth, windowHeight);

        if (newHoverState && !is_hovered)
        {
            if (OnHover)
                OnHover();
            is_hovered = true;
        }
        else if (!newHoverState && is_hovered)
        {
            reset_colors();
            is_hovered = false;
        }
    }

    bool contains_point(double mouseX, double mouseY, int windowWidth, int windowHeight) const override
    {
        float xabs = 2 * (mouseX / windowWidth) - 1;
        float yabs = 1 - 2 * (mouseY / windowHeight);
        return Bounds.Contains(vector3(xabs, yabs, 0));
    }
    void draw(unsigned int shaderProgram) const override
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
    void reset_colors()
    {
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
class Checkbox : public IUIElement
{
public:
    bool checked;
    Button button;
    Color checkedColor;
    Color uncheckedColor;
    std::function<void(bool)> OnChange;

    Checkbox(vector3 pos = vector3(0, 0, 0),
             vector3 size = vector3(0.05, 0.05, 0),
             Color uncheckedColor = Color(),
             Color checkedColor = Color(0, 255, 0),
             std::function<void(bool)> _onChange = nullptr)
        : checked(false),
          button(pos, size, uncheckedColor, Color(0, 0, 0)),
          checkedColor(checkedColor),
          uncheckedColor(uncheckedColor),
          OnChange(_onChange)
    {
        button.OnClick = [this]
        {
            checked = !checked;
            update_appearance();
            if (OnChange)
                OnChange(checked);
        };

        button.OnHover = [this]
        {
            // Логика наведения
        };

        update_appearance();
    }

    void update(double mouseX, double mouseY, int windowWidth, int windowHeight) override
    {
        button.update(mouseX, mouseY, windowWidth, windowHeight);
    }

    void handle_click() override
    {
        button.handle_click();
    }

    bool contains_point(double mouseX, double mouseY, int windowWidth, int windowHeight) const override
    {
        return button.contains_point(mouseX, mouseY, windowWidth, windowHeight);
    }

    void draw(unsigned int shaderProgram) const override
    {
        button.draw(shaderProgram);
    }

    void update_appearance()
    {
        button.base_bkg_color = checked ? checkedColor : uncheckedColor;
        button.current_bkg_color = button.base_bkg_color;
    }
};
class Textbox : public IUIElement
{
public:
    const std::string &_text;
    Color base_text_color;
    Color current_text_color;
    Color base_bkg_color;
    Color current_bkg_color;
    Color base_border_color;
    Color current_border_color;
    Rectangle _bounds;

    GLuint vao;
    GLuint vbo;
    GLfloat bounds_points[12];

    Textbox(const std::string &text = "Text",
            vector3 pos = vector3(0, 0, 0),
            vector3 size = vector3(0, 0, 0),
            Color text_color = Color(0, 0, 0),
            Color bkg_color = Color(),
            Color border_color = Color(0, 0, 0))
        : _text(text), _bounds(pos, size), base_text_color(text_color), base_bkg_color(bkg_color), base_border_color(border_color)
    {
        current_bkg_color = base_bkg_color;
        current_border_color = base_border_color;
        current_text_color = base_text_color;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        update_geometry();
    }
    void draw(unsigned int shader_program, TextRenderer *renderer) const
    {
        renderer->render_text(_text, _bounds.Location.X, _bounds.Location.Y, 1.0f, current_text_color);
    }
    void update_geometry()
    {
        for (int i = 0; i < 4; i++)
        {
            bounds_points[3 * i] = _bounds.Points[i].X;
            bounds_points[3 * i + 1] = _bounds.Points[i].Y;
            bounds_points[3 * i + 2] = _bounds.Points[i].Z;
        }

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(bounds_points), bounds_points, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void update(double mouseX, double mouseY, int windowWidth, int windowHeight) = 0;
    void handle_click() = 0;
    bool contains_point(double x, double y, int windowWidth, int windowHeight)
    {
        return clamp(x, 0.0, (double)windowWidth) == x &&
               clamp(y, 0.0, (double)(windowHeight)) == y &&
               _bounds.Contains(vector3(x, y, 0));
    }
};
