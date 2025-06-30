#pragma once
#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
std::string read_file(const char *path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file");
    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}

unsigned int create_shader_program(const char *vert_shader_path, const char *frag_shader_path)
{
    std::string vert_str = read_file(vert_shader_path);
    std::string frag_str = read_file(frag_shader_path);

    const char *vertShaderFinal = vert_str.c_str();
    const char *fragShaderFinal = frag_str.c_str();

    int success;
    char infoLog[512];

    unsigned int frag_shader;
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(frag_shader, 1, &fragShaderFinal, NULL);
    glCompileShader(frag_shader);
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
        std::cerr << "Vertex shader error: " << infoLog << std::endl;
    }

    unsigned int vert_shader;
    vert_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vert_shader, 1, &vertShaderFinal, NULL);
    glCompileShader(vert_shader);
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vert_shader, 512, NULL, infoLog);
        std::cerr << "Vertex shader error: " << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert_shader);
    glAttachShader(shaderProgram, frag_shader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    return shaderProgram;
}
void add_shades_to_program(const unsigned int &shader_program, const char *vert_shader_path, const char *frag_shader_path)
{
    std::string vert_str = read_file(vert_shader_path);
    std::string frag_str = read_file(frag_shader_path);

    const char *vertShaderFinal = vert_str.c_str();
    const char *fragShaderFinal = frag_str.c_str();

    int success;
    char infoLog[512];

    unsigned int frag_shader;
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(frag_shader, 1, &fragShaderFinal, NULL);
    glCompileShader(frag_shader);
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
        std::cerr << "Vertex shader error: " << infoLog << std::endl;
    }

    unsigned int vert_shader;
    vert_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vert_shader, 1, &vertShaderFinal, NULL);
    glCompileShader(vert_shader);
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vert_shader, 512, NULL, infoLog);
        std::cerr << "Vertex shader error: " << infoLog << std::endl;
    }

    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
}


/*
1. Одна программа = один набор шейдеров
Каждая шейдерная программа в OpenGL может содержать:
Только один вершинный шейдер
Только один фрагментный шейдер
Только один шейдер каждого другого типа (геометрический, тесселяции и т.д.)
*/
// TODO
// Реализовать mat4
class ShaderProgram
{
public:
    unsigned int program;
    ShaderProgram()
    {
        program = glCreateProgram();
        if (program == 0) {
            throw std::runtime_error("Failed to create shader program");
        }
    }

    void add_shader(GLenum shader_type, const char* shader_path){
        unsigned int shader = glCreateShader(shader_type);
        std::string _ = read_file(shader_path);
        const char* shader_data = _.c_str();

        glShaderSource(shader, 1, &shader_data, NULL);
        glCompileShader(shader);

        glAttachShader(program, shader);
        glDeleteShader(shader);
    }
    void link() { glLinkProgram(program); }
    void use() { glUseProgram(this->program); }

    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
    }
    
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(program, name.c_str()), value);
    }
    
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(program, name.c_str()), value);
    }
    
    
    void setMat4(const std::string& name, const mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, mat.m);
    }

    ~ShaderProgram() { glDeleteProgram(program); }
};