#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <zlib.h>
#include <vector>

struct PNGchunk
{
    uint32_t Lenght;
    char Type[4];
    std::vector<uint8_t> data;
    uint32_t crc;
};

class PNGLoader
{
public:
    static char *load_png(const char *path, int &width, int &height, int &channels)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            return nullptr;

        char signature[8];
        file.read(signature, 8);
        if (memcmp(signature, "\x89\x50\x4E\x47\x0D\x1A\x0A", 8) != 0) return nullptr;
        
    }
};