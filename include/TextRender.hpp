#pragma once
#include <iostream>
#include <map>
#include <string>
#include "Primitives.hpp"
#include "ShaderLoader.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

class Font
{
public:
    struct Glyph
    {
        vector2 size;     // Размеры глифа
        vector2 bearing;  // Смещение от базовой линии
        FT_Pos advance;   // Расстояние до следующего символа
        GLuint textureID; // OpenGL-текстура глифа
    };

    std::map<unsigned long, Glyph> glyphs;
    float line_height;

    Font(const std::string &path, unsigned int size)
    {
        FT_Library ft_library;
        if (FT_Init_FreeType(&ft_library))
            std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

        // Загрузка шрифта
        FT_Face face;
        if (FT_New_Face(ft_library, path.c_str(), 0, &face))
            std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;

        FT_Set_Pixel_Sizes(face, 0, size);

        for (unsigned int c = 0x0; c < 0x04FF; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }

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
            glyphs[c] = {
                {(float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows},
                {(float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top},
                face->glyph->advance.x,
                texture};
        }

        line_height = face->size->metrics.height >> 6;
        FT_Done_Face(face);
    }
    Glyph *get_glyph(unsigned long charcode) const
    {
        return const_cast<Glyph *>(&glyphs.at(charcode));
    }
};

class Text_renderer
{
public:
    ShaderProgram *shader_program;
    GLuint VAO, VBO;
    std::map<std::string, Font *> fonts;


    Text_renderer(ShaderProgram *_shader_program)
    {
        shader_program = _shader_program;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        shader_program->use();
        shader_program->setMat4("projection", mat4::ortho(0.0f, 800.0f, 0.0f, 800.0f, -1.0f, 1.0f).m);
        shader_program->setInt("text", 0);
    }
    ~Text_renderer()
    {
        fonts.clear();

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void load_font(const std::string &alias, const std::string &path, unsigned int size)
    {
        fonts[alias] = new Font(path, size);
    }
    void load_font(Font * font, const std::string& alias){
        fonts[alias] = font;
    }

    void render_text(const std::string &font_alias,
                     const std::wstring &text,
                     const vector2 &position,
                     const Color &color,
                     float scale)
    {
        shader_program->use();
        shader_program->setVector3("textColor", (const_cast<Color &>(color)).to_vector3() / 255.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        auto font = fonts.at(font_alias);
        float x = position.X;
        float y = position.Y;

        for (unsigned int c : text)
        {
            const auto &glyph = font->get_glyph(c);

            float xpos = x + glyph->bearing.X * scale;
            float ypos = y - (glyph->size.Y - glyph->bearing.Y) * scale;

            float w = glyph->size.X * scale;
            float h = glyph->size.Y * scale;

            float vertices[6][4] = {
                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos, ypos, 0.0f, 1.0f},
                {xpos + w, ypos, 1.0f, 1.0f},
                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos + w, ypos, 1.0f, 1.0f},
                {xpos + w, ypos + h, 1.0f, 0.0f}};

            glBindTexture(GL_TEXTURE_2D, glyph->textureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (glyph->advance >> 6) * scale;
        }
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

    inline void render_text_GL_coords(const std::string &text, float x, float y, float scale, const Color &color, int window_width, int window_height)
    {
        this->render_text(text,
                          window_width * (x + 1) / 2,
                          window_height * (y + 1) / 2,
                          scale,
                          color);
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
