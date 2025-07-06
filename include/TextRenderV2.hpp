#pragma once
#include "Primitives.hpp"
#include "ShaderLoader.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>
#include <vector>
#include <codecvt>
#include <locale>

std::wstring utf8_to_wstring(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(str);
};

struct GlyphV2
{
    GLuint TextureID; // OpenGL-текстура глифа
    vector2 Size;     // Размеры глифа
    vector2 Bearing;  // Смещение от базовой линии
    FT_Pos Advance;   // Расстояние до следующего символа
};

class FontV2
{
public:
    FT_Library ftLibrary;
    FT_Face face;
    std::map<unsigned long, GlyphV2> characters;
    FT_UInt fontSize;

    FontV2(const std::string &fontPath, FT_UInt size) : fontSize(size)
    {
        characters = {};
        if (FT_Init_FreeType(&ftLibrary))
        {
            std::cout << "Failed to load library" << '\n';
        }
        if (FT_New_Face(ftLibrary, fontPath.c_str(), 0, &face))
        {
            std::cout << "Failed to load font: " << fontPath << '\n';
        }
        FT_Set_Pixel_Sizes(face, 0, fontSize);
        loadBasicChars();
    }
    ~FontV2()
    {
        for (auto &[code, glyph] : characters)
        {
            glDeleteTextures(1, &glyph.TextureID);
        }
        FT_Done_Face(face);
        FT_Done_FreeType(ftLibrary);
    }
    const GlyphV2 &getCharacter(unsigned long charcode)
    {
        auto it = characters.find(charcode);
        if (it != characters.end())
        {
            return it->second;
        }
        return loadGlyphV2(charcode);
    }

private:
    void loadBasicChars()
    {
        for (unsigned long c = 32; c <= 128; c++)
            loadGlyphV2(c);
        for (unsigned long c = 0x0400; c <= 0x04FF; c++)
            loadGlyphV2(c); // Кириллица
    }
    const GlyphV2 &loadGlyphV2(unsigned long charcode)
    {
        if (FT_Load_Char(face, charcode, FT_LOAD_RENDER))
        {
            std::cout << "Failed to load GlyphV2: " << charcode << std::endl;
            // static GlyphV2 fallback = {0, {0, 0}, {0, 0}, 0};
            // return fallback;
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GlyphV2 GlyphV2 = {
            texture,
            vector2((float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows),
            vector2((float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top),
            face->glyph->advance.x};
        characters[charcode] = GlyphV2;
        return characters[charcode];
    }
};

class TextRendererV2
{
public:
    TextRendererV2(ShaderProgram *_shader_program)
    {
        std::cout << "TextRendererV2 constructor" << '\n';
        shader_program = _shader_program;
        setupBuffers();

        shader_program->use();
        shader_program->setMat4("projection", mat4::ortho(0.0f, 800.0f, 0.0f, 800.0f, -1.0f, 1.0f).m);
        shader_program->setInt("text", 0);
    }
    ~TextRendererV2()
    {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void renderText(FontV2 *font, const std::wstring &text, float x, float y, float scale, const Color &color)
    {
        shader_program->setVector3("textColor", color.to_vector3() / 255.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (wchar_t c : text)
        {
            const GlyphV2 &ch = font->getCharacter(c);

            float xpos = x + ch.Bearing.X * scale;
            float ypos = y - (ch.Size.Y - ch.Bearing.Y) * scale;

            float w = ch.Size.X * scale;
            float h = ch.Size.Y * scale;

            float vertices[6][4] = {
                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos, ypos, 0.0f, 1.0f},
                {xpos + w, ypos, 1.0f, 1.0f},

                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos + w, ypos, 1.0f, 1.0f},
                {xpos + w, ypos + h, 1.0f, 0.0f}};

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

private:
    ShaderProgram *shader_program;
    GLuint VAO, VBO;

    void setupBuffers()
    {
        std::cout << "setting up buffers ...";
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        std::cout << "Done.\n";
    }
};
