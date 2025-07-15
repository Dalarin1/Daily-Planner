#pragma once
#include "TextRender.hpp"
#include <functional>
#include <memory>
class IUIElement
{
public:
    virtual ~IUIElement() = default;
    virtual void draw(unsigned int shader_program) const = 0;
    virtual void update(double mouseX, double mouseY, int windowWidth, int windowHeight) = 0;
    virtual void handle_click() = 0;
    virtual bool contains_point(double x, double y, int windowWidth, int windowHeight) const = 0;
};
#if 0
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
            reset_state();
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
    void reset_state()
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
class Textbox : public IUIElement
{
public:
    std::string Text;
    Rectangle Bounds;
};

class UIBox : public IUIElement
{
private:
    GLuint m_vao;
    GLuint m_vbo;
    Color m_currentBackgroundColor;
    Color m_currentBorderColor;
    Color m_baseBackgroundColor;
    Color m_baseBorderColor;

public:
    int BorderWidth;
    Rectangle Bounds;

    UIBox(vector3 position = vector3(),
          vector3 size = vector3(),
          Color background_color = Color(),
          Color border_color = Color(),
          int border_width = 1)
    {
        m_baseBackgroundColor = background_color;
        m_baseBorderColor = border_color;
        m_currentBackgroundColor = m_baseBackgroundColor;
        m_currentBorderColor = m_baseBorderColor;
        BorderWidth = border_width;
        Bounds = Rectangle(position, size);

        glGenBuffers(1, &m_vbo);
        glGenVertexArrays(1, &m_vao);

        update_geometry();
    }

    void draw(unsigned int shader_program) const override
    {
        GLint colorloc = glGetUniformLocation(shader_program, "color");
        glUniform4f(colorloc,
                    m_currentBackgroundColor.Red / 255.0f,
                    m_currentBackgroundColor.Green / 255.0f,
                    m_currentBackgroundColor.Blue / 255.0f,
                    1.0f);
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        if (BorderWidth > 0)
        {
            glUniform4f(colorloc,
                        m_currentBorderColor.Red / 255.0f,
                        m_currentBorderColor.Green / 255.0f,
                        m_currentBorderColor.Blue / 255.0f,
                        1.0f);

            glLineWidth(BorderWidth);
            glDrawArrays(GL_LINE_LOOP, 0, 4);
            glBindVertexArray(0);
        }
    }
    void update(double mouseX, double mouseY, int windowWidth, int windowHeight) override {}
    void update_geometry()
    {
        Bounds.Update();

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Bounds.Points), Bounds.Points, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void handle_click() override {}
    bool contains_point(double x, double y, int windowWidth, int windowHeight) const override
    {
        return Bounds.Contains(vector3(2 * (x / windowWidth) - 1, 1 - 2 * (y / windowHeight), 0));
    }

    void set_background_color(const Color &color)
    {
        m_currentBackgroundColor = color;
    }
    void set_border_color(const Color &color)
    {
        m_currentBorderColor = color;
    }

    void reset_state()
    {
        m_currentBackgroundColor = m_baseBackgroundColor;
        m_currentBorderColor = m_baseBorderColor;
    }
};

class UIButton : public IUIElement
{
private:
    TextRenderer *m_textRenderer;

public:
    UIBox Box;
    bool IsPressed;
    bool IsHovered;
    std::string Text;
    Color TextColor;
    std::function<void()> OnClick;
    std::function<void()> OnHover;
    UIButton(vector3 position = vector3(),
             vector3 size = vector3(),
             Color background_color = Color(),
             Color border_color = Color(),
             Color text_color = Color(),
             int border_width = 1,
             const std::string &text = "",
             std::function<void()> on_click = nullptr,
             std::function<void()> on_hover = nullptr)
    {
        Box = UIBox(position, size, background_color, border_color, border_width);
        Text = text;
        TextColor = text_color;
        OnClick = on_click;
        OnHover = on_hover;
    }

    void draw(unsigned int shader_program) const
    {
        Box.draw(shader_program);
        if (!Text.empty())
            m_textRenderer->render_text_GL_coords(Text, Box.Bounds.Location.X, Box.Bounds.Location.Y, 1.0f, TextColor, 800, 800);
    }
    void update(double mouseX, double mouseY, int windowWidth, int windowHeight)
    {
        bool newHoverState = Box.contains_point(mouseX, mouseY, windowWidth, windowHeight);

        if (newHoverState && !IsHovered)
        {
            if (OnHover)
                OnHover();
            IsHovered = true;
        }
        else if (!newHoverState && IsHovered)
        {
            Box.reset_state();
            IsHovered = false;
        }
    }
    void handle_click()
    {
        if (OnClick)
            OnClick();
    }
    bool contains_point(double x, double y, int windowWidth, int windowHeight) const
    {
        return Box.contains_point(x, y, windowWidth, windowHeight);
    }
};

class UICheckbox : public IUIElement
{
private:
    TextRenderer *m_textRenderer;

public:
    UIBox Box;
    bool IsChecked;
    bool IsHovered;
    std::string Text;
    Color TextColor;
    std::function<void()> OnCheck;
    std::function<void()> OnHover;

    UICheckbox(vector3 position = vector3(),
               vector3 size = vector3(),
               Color background_color = Color(),
               Color border_color = Color(),
               Color text_color = Color(),
               int border_width = 1,
               const std::string &text = "",
               std::function<void()> on_check = nullptr,
               std::function<void()> on_hover = nullptr)
    {
        Box = UIBox(position, size, background_color, border_color, border_width);
        Text = text;
        TextColor = text_color;
        OnCheck = on_check;
        OnHover = on_hover;
    }
    void draw(unsigned int shader_program) const
    {
        Box.draw(shader_program);
        if (!Text.empty())
            m_textRenderer->render_text_GL_coords(Text, Box.Bounds.Location.X, Box.Bounds.Location.Y, 1.0f, TextColor, 800, 800);
    }
    void update(double mouseX, double mouseY, int windowWidth, int windowHeight)
    {
        bool newHoverState = Box.contains_point(mouseX, mouseY, windowWidth, windowHeight);

        if (newHoverState && !IsHovered)
        {
            if (OnHover)
                OnHover();
            IsHovered = true;
        }
        else if (!newHoverState && IsHovered)
        {
            Box.reset_state();
            IsHovered = false;
        }
    }
    void handle_click()
    {
        if (OnCheck)
        {
            OnCheck();
        }
    }
    bool contains_point(double x, double y, int windowWidth, int windowHeight) const
    {
        return Box.contains_point(x, y, windowWidth, windowHeight);
    }
};

class UITextfield : public IUIElement
{
private:
    TextRenderer *m_textRenderer;

public:
    UIBox Box;
    std::string Text;
    Color TextColor;
    UITextfield(vector3 position = vector3(),
                vector3 size = vector3(),
                Color background_color = Color(),
                Color border_color = Color(),
                Color text_color = Color(),
                int border_width = 1,
                const std::string &text = "")
    {
        Box = UIBox(position, size, background_color, border_color, border_width);
        Text = text;
        TextColor = text_color;
    }
    void draw(unsigned int shader_program) const override
    {
        Box.draw(shader_program);
        if (!Text.empty())
            m_textRenderer->render_text_GL_coords(Text, Box.Bounds.Location.X, Box.Bounds.Location.Y, 1.0f, TextColor, 800, 800);
    }
    void update(double mouseX, double mouseY, int windowWidth, int windowHeight) override {}
    void handle_click() override {}
    bool contains_point(double x, double y, int windowWidth, int windowHeight) const override
    {
        return Box.contains_point(x, y, windowWidth, windowHeight);
    }
};
#endif
// Базовый класс для прямоугольных элементов
class UIRectangle : public IUIElement
{
protected:
    Color m_baseBackgroundColor;
    Color m_baseBorderColor;
    int m_baseBorderWidth;
    std::shared_ptr<TextRenderer> m_textRenderer; // Общий рендерер текста
    GLuint m_vao;
    GLuint m_vbo;

    void setup_buffers()
    {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        update_geometry();
    }

public:
    Color BackgroundColor;
    Color BorderColor;
    int BorderWidth;
    Rectangle Bounds;

    UIRectangle(vector3 position = vector3(),
                vector3 size = vector3(),
                Color background = Color(),
                Color border = Color(),
                int border_width = 1,
                std::shared_ptr<TextRenderer> renderer = nullptr)
        : m_textRenderer(renderer),
          BackgroundColor(background),
          BorderColor(border),
          BorderWidth(border_width),
          Bounds(position, size)
    {
        m_baseBackgroundColor = background;
        m_baseBorderColor = border;
        m_baseBorderWidth = border_width;
        setup_buffers();
    }

    virtual ~UIRectangle()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
    }

    void update_geometry()
    {
        Bounds.Update();
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Bounds.Points), Bounds.Points, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void update(double x, double y, int w, int h) override {}
    void draw(unsigned int shader_program) const override
    {
        GLint colorLoc = glGetUniformLocation(shader_program, "color");

        // Отрисовка фона
        glUniform4f(colorLoc, BackgroundColor.Red / 255.0f, BackgroundColor.Green / 255.0f, BackgroundColor.Blue / 255.0f, 1.0f);
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Отрисовка границы
        if (BorderWidth > 0)
        {
            glUniform4f(colorLoc, BorderColor.Red / 255.0f, BorderColor.Green / 255.0f, BorderColor.Blue / 255.0f, 1.0f);
            glLineWidth(BorderWidth);
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
        glBindVertexArray(0);
    }
    void handle_click() {}
    bool contains_point(double x, double y, int w, int h) const override
    {
        const float nx = 2.0f * (x / w) - 1.0f;
        const float ny = 1.0f - 2.0f * (y / h);
        return Bounds.Contains(vector3(nx, ny, 0));
    }

    Color get_base_background_color() const { return m_baseBackgroundColor; }
    Color get_base_border_color() const { return m_baseBorderColor; }

    void reset_state()
    {
        BackgroundColor = m_baseBackgroundColor;
        BorderColor = m_baseBorderColor;
        BorderWidth = m_baseBorderWidth;
    }
    void set_base_background_color(const Color &color) { m_baseBackgroundColor = color; }
    void set_base_border_color(const Color &color) { m_baseBorderColor = color; }
};

// Класс для интерактивных элементов
class UIInteractive : public UIRectangle
{
protected:
    bool m_hovered = false;
    bool m_pressed = false;

public:
    std::function<void()> OnClick;
    std::function<void()> OnHover;

    UIInteractive(vector3 pos = vector3(),
                  vector3 size = vector3(),
                  Color bg = Color(),
                  Color border = Color(),
                  int border_width = 1,
                  std::shared_ptr<TextRenderer> renderer = nullptr)
        : UIRectangle(pos, size, bg, border, border_width, renderer) {}

    void update(double mx, double my, int w, int h) override
    {
        const bool nowHovered = contains_point(mx, my, w, h);

        if (nowHovered && !m_hovered)
        {
            if (OnHover)
                OnHover();
            m_hovered = true;
        }
        else if (!nowHovered && m_hovered)
        {
            m_hovered = false;
        }
    }

    void handle_click() override
    {
        if (m_hovered && OnClick)
        {
            OnClick();
        }
    }

    bool is_hovered() const { return m_hovered; }
    bool is_pressed() const { return m_pressed; }
};

// Кнопка
class UIButton : public UIInteractive
{
public:
    std::string Text;
    Color TextColor;
    UIButton(vector3 pos = vector3(),
             vector3 size = vector3(),
             Color bg = Color(),
             Color border = Color(),
             Color textColor = Color(0, 0, 0),
             int border_width = 1,
             const std::string &text = "",
             std::function<void()> onClick = nullptr,
             std::function<void()> onHover = nullptr,
             std::shared_ptr<TextRenderer> renderer = nullptr)
        : UIInteractive(pos, size, bg, border, border_width, renderer),
          Text(text),
          TextColor(textColor)
    {
        OnClick = onClick;
        OnHover = onHover;
    }
    void update(double mouseX, double mouseY, int windowWidth, int windowHeight) override
    {
        bool newHoverState = contains_point(mouseX, mouseY, windowWidth, windowHeight);

        if (newHoverState && !m_hovered)
        {
            if (OnHover)
                OnHover();
            m_hovered = true;
        }
        else if (!newHoverState && m_hovered)
        {
            reset_state();
            m_hovered = false;
        }
    }
    void draw(unsigned int shader) const override
    {
        UIInteractive::draw(shader);
        if (m_textRenderer && !Text.empty())
        {
            m_textRenderer->render_text_GL_coords(
                Text,
                Bounds.Location.X,
                Bounds.Location.Y - Bounds.Size.Y,
                1.0f,
                TextColor,
                800,
                800);
        }
    }
};

// Чекбокс
class UICheckbox : public UIInteractive
{
private:
    bool m_checked = false;
    Color m_checkedColor;
    Color m_uncheckedColor;

public:
    std::string Text;
    Color TextColor;
    std::function<void(bool)> OnChange;

    UICheckbox(vector3 pos = vector3(),
               vector3 size = vector3(),
               Color uncheckedColor = Color(),
               Color checkedColor = Color(0, 255, 0),
               Color textColor = Color(0, 0, 0),
               int border_width = 1,
               const std::string &text = "",
               std::function<void(bool)> onChange = nullptr,
               std::shared_ptr<TextRenderer> renderer = nullptr)
        : UIInteractive(pos, size, uncheckedColor, Color(0, 0, 0), border_width, renderer),
          m_checkedColor(checkedColor),
          m_uncheckedColor(uncheckedColor),
          Text(text),
          TextColor(textColor),
          OnChange(onChange)
    {
        OnClick = [this]
        {
            m_checked = !m_checked;
            update_appearance();
            if (OnChange)
                OnChange(m_checked);
        };
    }

    void update_appearance()
    {
        BackgroundColor = m_checked ? m_checkedColor : m_uncheckedColor;
    }

    void draw(unsigned int shader) const override
    {
        UIInteractive::draw(shader);
        if (m_textRenderer && !Text.empty())
        {
            m_textRenderer->render_text_GL_coords(
                Text,
                Bounds.Location.X + Bounds.Size.X,
                Bounds.Location.Y - Bounds.Size.Y,
                1.0f,
                TextColor,
                800,
                800);
        }
    }

    bool is_checked() const { return m_checked; }
};

// Текстовое поле
class UITextfield : public UIRectangle
{
private:
    bool m_imputable = false;
    bool m_focused;
    unsigned int m_cursorPos;

public:
    std::string Text;
    Color TextColor;

    UITextfield(vector3 pos = vector3(),
                vector3 size = vector3(),
                Color bg = Color(),
                Color border = Color(),
                Color textColor = Color(0, 0, 0),
                int border_width = 1,
                std::string text = "",
                std::shared_ptr<TextRenderer> renderer = nullptr)
        : UIRectangle(pos, size, bg, border, border_width, renderer),
          Text(text),
          TextColor(textColor),
          m_cursorPos(text.size()) {}

    void handle_click() override
    {
        m_focused = true; // Упрощённая реализация

        BackgroundColor = Color(100, 0, 0);
    }

    void draw(unsigned int shader) const override
    {
        UIRectangle::draw(shader);
        if (m_textRenderer && !Text.empty())
        {
            m_textRenderer->render_text_GL_coords(
                Text,
                Bounds.Location.X,
                Bounds.Location.Y - Bounds.Size.Y,
                1.0f,
                TextColor,
                800,
                800);
        }
    }
    void set_focus(bool focus)
    {
        m_focused = focus;
    }
     void handle_key_input(int key, int action) {
        if (!m_focused || action != GLFW_PRESS) return;

        switch (key) {
            case GLFW_KEY_BACKSPACE:
                if (!Text.empty() && m_cursorPos > 0) {
                    Text.erase(m_cursorPos - 1, 1);
                    m_cursorPos--;
                }
                break;
                
            case GLFW_KEY_DELETE:
                if (!Text.empty() && m_cursorPos < Text.length()) {
                    Text.erase(m_cursorPos, 1);
                }
                break;
                
            case GLFW_KEY_LEFT:
                if (m_cursorPos > 0) m_cursorPos--;
                break;
                
            case GLFW_KEY_RIGHT:
                if (m_cursorPos < Text.length()) m_cursorPos++;
                break;
                
            case GLFW_KEY_HOME:
                m_cursorPos = 0;
                break;
                
            case GLFW_KEY_END:
                m_cursorPos = Text.length();
                break;

        }
    }

    void handle_char_input(unsigned int codepoint)
    {
        if (!m_focused)
            return;
        Text.insert(m_cursorPos, 1, static_cast<char>(codepoint));
        m_cursorPos++;
    }
    void make_inputable(){
        m_imputable = true;
    }
    bool is_imputable(){return m_imputable;}
};