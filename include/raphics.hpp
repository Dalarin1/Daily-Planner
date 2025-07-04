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
        Bounds.Update();
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
    void draw_at(unsigned int shaderProgram, vector3 pos)
    {
        button.Bounds.Location = pos;
        button.update_geometry();
        button.draw(shaderProgram);
    }

    void update_appearance()
    {
        button.base_bkg_color = checked ? checkedColor : uncheckedColor;
        button.current_bkg_color = button.base_bkg_color;
    }
};

class Box : public IUIElement
{
public:
    Color BackgroundColor;
    Color BorderColor;
    int BorderWidth;
    Rectangle Bounds;

    Box(vector3 position = vector3(),
        vector3 size = vector3(),
        Color background_color = Color(),
        Color border_color = Color(),
        int border_width = 1);

    void draw(unsigned int shader_program) const override;
    void update(double mouseX, double mouseY, int windowWidth, int windowHeight) override = 0;
    void update_geometry();
    void handle_click() override = 0;
    bool contains_point(double x, double y, int windowWidth, int windowHeight) const override;

protected:
    GLuint _vao;
    GLuint _vbo;
    GLfloat _points[12];
};

Box::Box(vector3 position,
         vector3 size,
         Color background_color,
         Color border_color,
         int border_width)
{
    BackgroundColor = background_color;
    BorderColor = border_color;
    BorderWidth = border_width;
    Bounds = Rectangle(position, size);

    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);

    update_geometry();
}

void Box::update_geometry()
{
    Bounds.Update();
    for (int i = 0; i < 4; i++)
    {
        _points[3 * i] = Bounds.Points[i].X;
        _points[3 * i + 1] = Bounds.Points[i].Y;
        _points[3 * i + 2] = Bounds.Points[i].Z;
    }

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_points), _points, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Box::draw(unsigned int shader_program) const
{
    GLint colorloc = glGetUniformLocation(shader_program, "color");
    glUniform4f(colorloc,
                BackgroundColor.Red / 255.0f,
                BackgroundColor.Green / 255.0f,
                BackgroundColor.Blue / 255.0f,
                1.0f);
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glUniform4f(colorloc,
                BackgroundColor.Red / 255.0f,
                BackgroundColor.Green / 255.0f,
                BackgroundColor.Blue / 255.0f,
                1.0f);

    glLineWidth(BorderWidth);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

bool Box::contains_point(double x, double y, int windowWidth, int windowHeight) const
{
    return Bounds.Contains(vector3(2 * (x / windowWidth) - 1, 1 - 2 * (y / windowHeight), 0));
}
