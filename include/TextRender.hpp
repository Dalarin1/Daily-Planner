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

    std::map<wchar_t, Glyph> glyphs;
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

        for (wchar_t c = 0x0; c < 0x04FF; c++)
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
    Glyph *get_glyph(wchar_t charcode) const
    {
        return const_cast<Glyph *>(&glyphs.at(charcode));
    }
};

class TextRenderer
{
public:
    ShaderProgram *shader_program;
    GLuint VAO, VBO;
    std::map<std::string, Font *> fonts;

    TextRenderer(ShaderProgram *_shader_program)
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
    ~TextRenderer()
    {
        fonts.clear();

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void load_font(const std::string &alias, const std::string &path, unsigned int size)
    {
        fonts[alias] = new Font(path, size);
    }

    void load_font(Font *font, const std::string &alias)
    {
        fonts[alias] = font;
    }

    inline void render_text_GL_coords(const std::string &font_alias, const std::wstring &text, float x, float y, float scale, const Color &color, int window_width, int window_height)
    {
        this->render_text(font_alias,
                          text,
                          {
                              window_width * (x + 1) / 2,
                              window_height * (y + 1) / 2,
                          },
                          color,
                          scale);
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
