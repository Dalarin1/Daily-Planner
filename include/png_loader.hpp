#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <vector>
#include <map>
#include <zlib.h>

struct PNGchunk
{
    uint32_t length;
    char type[4];
    std::vector<uint8_t> data;
    uint32_t crc;
};

struct Color8bit
{
public:
    char r, g, b, a;
    Color8bit(char _r, char _g, char _b, char _a = 1) : r(_r), g(_g), b(_b), a(_a)
    {
    }
};

struct Color16bit
{
public:
    int16_t r, g, b, a;
    Color16bit(int16_t _r, int16_t _g, int16_t _b, int16_t _a = 1) : r(_r), g(_g), b(_b), a(_a)
    {
    }
};

class sPLT_palette{
    std::string name;
    uint8_t color_depth;
    
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
        if (memcmp(signature, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8) != 0)
            return nullptr;

        std::vector<PNGchunk> chunks;
        while (true)
        {
            PNGchunk chunk;
            file.read(reinterpret_cast<char *>(&chunk.length), 4);

            file.read(chunk.type, 4);
            chunk.data.resize(chunk.length);
            file.read(reinterpret_cast<char *>(chunk.data.data()), chunk.length);

            file.read(reinterpret_cast<char *>(&chunk.crc), 4);
            chunks.push_back(chunk);

            if (memcmp(chunk.type, "IEND", 4) == 0)
                break;
        }

        std::vector<uint8_t> compressed = {};
        std::vector<Color8bit> PLTE_palette = {};

        for (const auto &chunk : chunks)
        {
            // Обработка IHDR
            if (memcmp(chunk.type, "IHDR", 4) == 0)
            {
                width = read_uint32(chunk.data.data());
                height = read_uint32(chunk.data.data() + 4);
                uint8_t bit_depth = chunk.data[8];
                uint8_t color_type = chunk.data[9];

                if (bit_depth != 8)
                    return nullptr; // Только 8-битные

                if (color_type == 2) // RGB
                {
                    channels = 3;
                }
                else if (color_type == 6) // RGBA
                {
                    channels = 4;
                }
                else // Неподдерживаемый тип
                {
                    return nullptr;
                }
            }
            if (memcmp(chunk.type, "IDAT", 4) == 0)
            {
                compressed.insert(compressed.end(), chunk.data.begin(), chunk.data.end());
                continue;
            }
            if (memcmp(chunk.type, "PLTE", 4) == 0)
            {
                if (chunk.length % 3 != 0) // spec; Длина блока PLTE делится на 3, иначе ошибка
                {
                    return nullptr;
                }

                for (int i = 0; i < chunk.length; i += 3)
                {
                    PLTE_palette.push_back(Color8bit(chunk.data[i], chunk.data[i + 1], chunk.data[i + 2]));
                }
                continue;
            }
            if (memcmp(chunk.type, "IEND", 4) == 0)
            {
                break;
            }
            if (memcmp(chunk.type, "sPLT", 4) == 0)
            {
                std::string palette_name = "";
                int i = 0;
                for (; i < chunk.data.size(); i++)
                {
                    if (chunk.data[i] != '\0')
                    {
                        palette_name.push_back(chunk.data[i]);
                    }
                    else
                    {
                        break;
                    }
                }
                i++; // пропуск нуль-терминатора после имени палитры
                uint8_t color_depth = chunk.data[i++];

                if (color_depth == 8)
                {
                    std::vector<Color8bit> scheme;
                    for (; i < chunk.data.size(); i += 6)
                    {
                        scheme.push_back(Color8bit(chunk.data[i], chunk.data[i + 1], chunk.data[i + 2], chunk.data[i + 3]));
                        uint16_t freq = chunk.data[i + 4] << 8 | chunk.data[i + 5];
                    }
                }
                else if (color_depth == 16)
                {
                    std::vector<Color16bit> scheme;
                    for (; i < chunk.data.size(); i += 10)
                    {
                        scheme.push_back(Color16bit(chunk.data[i] << 8 | chunk.data[i + 1],
                                                    chunk.data[i + 2] << 8 | chunk.data[i + 3],
                                                    chunk.data[i + 4] << 8 | chunk.data[i + 5],
                                                    chunk.data[i + 6] << 8 | chunk.data[i + 7]));

                        uint16_t freq = chunk.data[i + 8] << 8 | chunk.data[i + 9];
                    }
                }
                else
                {
                    return nullptr; // ошибка глубины цвета
                }
            }

            if (memcmp(chunk.type, "bKGD", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "cHRM", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "gAMA", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "hIST", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "iCCP", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "iTXt", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "pHYs", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "sBIT", 4) == 0)
            {
                continue;
            }

            if (memcmp(chunk.type, "sRGB", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "sTER", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "tEXt", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "tIME", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "tRNS", 4) == 0)
            {
                continue;
            }
            if (memcmp(chunk.type, "zTXt", 4) == 0)
            {
                continue;
            }
            // Распаковка zlib
            std::vector<uint8_t> pixel_data = inflate_data(compressed, width, height, channels);
            if (pixel_data.empty())
                return nullptr;

            // Применение фильтров
            apply_filters(pixel_data, width, height, channels);

            // Копируем данные в буфер
            char *image_data = new char[pixel_data.size()];
            memcpy(image_data, pixel_data.data(), pixel_data.size());

            return image_data;
        }
    }
    static uint8_t paeth_predictor(uint8_t a, uint8_t b, uint8_t c)
    {
        int p = a + b - c;
        int pa = abs(p - a);
        int pb = abs(p - b);
        int pc = abs(p - c);

        if (pa <= pb && pa <= pc)
            return a;
        if (pb <= pc)
            return b;
        return c;
    }
    static void apply_filters(std::vector<uint8_t> &data,
                              int width, int height, int channels)
    {
        size_t bpp = channels;
        size_t row_bytes = width * bpp;

        for (size_t y = 0; y < height; ++y)
        {
            uint8_t filter_type = data[y * (row_bytes + 1)];
            uint8_t *row = data.data() + y * (row_bytes + 1) + 1;
            uint8_t *prev_row = (y > 0) ? data.data() + (y - 1) * (row_bytes + 1) + 1 : nullptr;

            switch (filter_type)
            {
            case 0: // None
                break;
            case 1: // Sub
                for (size_t i = bpp; i < row_bytes; ++i)
                {
                    row[i] += row[i - bpp];
                }
                break;
            case 2: // Up
                if (prev_row)
                {
                    for (size_t i = 0; i < row_bytes; ++i)
                    {
                        row[i] += prev_row[i];
                    }
                }
                break;
            case 3: // Average
                for (size_t i = 0; i < row_bytes; ++i)
                {
                    uint8_t left = (i >= bpp) ? row[i - bpp] : 0;
                    uint8_t up = prev_row ? prev_row[i] : 0;
                    row[i] += (left + up) / 2;
                }
                break;
            case 4: // Paeth
                for (size_t i = 0; i < row_bytes; ++i)
                {
                    uint8_t left = (i >= bpp) ? row[i - bpp] : 0;
                    uint8_t up = prev_row ? prev_row[i] : 0;
                    uint8_t up_left = (prev_row && i >= bpp) ? prev_row[i - bpp] : 0;
                    row[i] += paeth_predictor(left, up, up_left);
                }
                break;
            }
        }
    }
    static uint32_t read_uint32(const uint8_t *data)
    {
        return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    }
    static std::vector<uint8_t> inflate_data(const std::vector<uint8_t> &compressed,
                                             int width, int height, int channels)
    {
        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;

        if (inflateInit(&stream) != Z_OK)
            return {};

        stream.avail_in = compressed.size();
        stream.next_in = const_cast<Bytef *>(compressed.data());

        std::vector<uint8_t> uncompressed(width * height * channels + height);
        stream.avail_out = uncompressed.size();
        stream.next_out = uncompressed.data();

        if (inflate(&stream, Z_FINISH) != Z_STREAM_END)
        {
            inflateEnd(&stream);
            return {};
        }

        inflateEnd(&stream);
        return uncompressed;
    }
};