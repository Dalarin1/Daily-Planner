#pragma once
#include "Primitives.hpp"
#include "ShaderLoader.hpp"
#include <map>
#include <functional>
#include <ft2build.h>
#include FT_FREETYPE_H

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
    void draw(unsigned int shader_program, TextRenderer* renderer) const
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

class TextRenderer
{
public:
    struct Character
    {
        GLuint TextureID; // ID текстуры глифа
        vector2 Size;     // Размер глифа
        vector2 Bearing;  // Смещение от базовой линии
        FT_Pos Advance;   // Горизонтальное смещение до следующего глифа
    };

    std::map<char, Character> Characters;
    GLuint VAO, VBO;
    ShaderProgram *Program = nullptr;
    // Shader textShader;

    TextRenderer(ShaderProgram *program, const std::string &fontPath, unsigned int fontSize)
    {
        this->Program = program;
        this->Characters = {};
        // Инициализация FreeType
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

        // Загрузка шрифта
        FT_Face face;
        if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
            std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;

        // Установка размера шрифта
        FT_Set_Pixel_Sizes(face, 0, fontSize);

        // Отключаем выравнивание байтов
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Загрузка первых 128 символов ASCII
        for (unsigned char c = 0; c < 128; c++)
        {
            // Загрузка глифа
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }

            // Генерация текстуры
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer);

            // Установка параметров текстуры
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Сохранение символа для последующего использования
            Character character = {
                texture,
                vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                vector2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<FT_Pos>(face->glyph->advance.x)};

            this->Characters.insert(std::pair<char, Character>(c, character));
        }

        // Очистка FreeType
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        // Настройка VAO/VBO для отрисовки текста
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        Program->use();
        Program->setMat4("projection", mat4::ortho(0.0f, 800.0f, 0.0f, 800.0f, -1.0f, 1.0f).m);
        Program->setInt("text", 0);
    }

    void render_text(const std::string &text, float x, float y, float scale, const Color &color)
    {

        Program->use();
        Program->setVector3("textColor", (const_cast<Color &>(color)).to_vector3() / 255.0f);
        
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for (auto c = text.begin(); c != text.end(); c++)
        {
            Character ch = Characters[*c];

            float xpos = x + ch.Bearing.X * scale;
            float ypos = y - (ch.Size.Y - ch.Bearing.Y) * scale;

            float w = ch.Size.X * scale;
            float h = ch.Size.Y * scale;

            // Вершины для каждого символа (2 треугольника)
            float vertices[6][4] = {
                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos, ypos, 0.0f, 1.0f},
                {xpos + w, ypos, 1.0f, 1.0f},

                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos + w, ypos, 1.0f, 1.0f},
                {xpos + w, ypos + h, 1.0f, 0.0f}};

            // Рендеринг текстуры глифа на прямоугольнике
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);

            // Обновление содержимого VBO
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Отрисовка прямоугольника
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Смещение для следующего глифа
            x += (ch.Advance >> 6) * scale;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};