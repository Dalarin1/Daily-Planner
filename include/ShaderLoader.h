#pragma once
#include <iostream>
#include <fstream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

std::string read_file(const char* path) {
    std::ifstream file(path);
    if(!file.is_open()) throw std::runtime_error("Failed to open file");
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

    return shaderProgram;
}