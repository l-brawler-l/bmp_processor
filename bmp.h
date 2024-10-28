
#ifndef IMG_PROC_BMP_H
#define IMG_PROC_BMP_H

#include <bits/fs_fwd.h>
#include "matrix.h"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <cstddef>

// В оперативной памяти большинства архитектур хранятся в формате little-endian (младшие байты по младшим адресам).
// Например, число 298 будет храниться в виде 4 байт в шестнадцатиричной нотации: 2A 01 00 00.
// В bmp файле числа хранятся тоже в little-endian, поэтому мы можем читать их и записывать непосредственно.

// struct IntBGR {
//     uint8_t blue;
//     uint8_t green;
//     uint8_t red;

//     explicit IntBGR(const FloatBGR& float_bgr)
//         : blue(static_cast<uint8_t>(round(float_bgr.blue)))
//         , green(static_cast<uint8_t>(round(float_bgr.green)))
//         , red(static_cast<uint8_t>(round(float_bgr.red)))
//     {
//     }

// } __attribute__((packed));

struct Consts {
    enum constants : const uint8_t {
        eight = 8,
        four = 4,
        two_five_five = 255,
    };
};

struct IntBGR {
    uint8_t blue;
    uint8_t green;
    uint8_t red;

    IntBGR() : blue(0), green(0), red(0) {
    }

    IntBGR(uint8_t b, uint8_t g, uint8_t r) : blue(b), green(g), red(r) {
    }

    IntBGR(IntBGR const& other) : blue(other.blue), green(other.green), red(other.red) {
    }

    IntBGR& operator=(const IntBGR& rhs) {
        blue = rhs.blue;
        green = rhs.green;
        red = rhs.red;
        return *this;
    }
};

struct BGR {
    float blue;
    float green;
    float red;

    BGR() : blue(0), green(0), red(0) {
    }

    BGR(float b, float g, float r) : blue(b), green(g), red(r) {
    }

    explicit BGR(IntBGR int_bgr)
        : blue(static_cast<float>(int_bgr.blue) / static_cast<float>(Consts::two_five_five)),
          green(static_cast<float>(int_bgr.green) / static_cast<float>(Consts::two_five_five)),
          red(static_cast<float>(int_bgr.red) / static_cast<float>(Consts::two_five_five)) {
    }

    IntBGR ToInt() const {
        IntBGR result{static_cast<uint8_t>(round(blue * static_cast<float>(Consts::two_five_five))),
                      static_cast<uint8_t>(round(green * static_cast<float>(Consts::two_five_five))),
                      static_cast<uint8_t>(round(red * static_cast<float>(Consts::two_five_five)))};
        return result;
    }

    BGR(const BGR& other) : blue(other.blue), green(other.green), red(other.red) {
    }

    BGR& operator=(const BGR& rhs) {
        blue = rhs.blue;
        green = rhs.green;
        red = rhs.red;
        return *this;
    }

    friend BGR& operator+=(BGR& lhs, const BGR& rhs) {
        lhs.blue += rhs.blue;
        lhs.green += rhs.green;
        lhs.red += rhs.red;
        return lhs;
    }

    friend BGR& operator-=(BGR& lhs, const BGR& rhs) {
        lhs.blue -= rhs.blue;
        lhs.green -= rhs.green;
        lhs.red -= rhs.red;
        return lhs;
    }

    friend BGR& operator*=(BGR& lhs, float rhs) {
        lhs.blue *= rhs;
        lhs.green *= rhs;
        lhs.red *= rhs;
        return lhs;
    }
};  // __attribute__((packed));

BGR operator+(const BGR& lhs, const BGR& rhs) {
    BGR new_bgr = lhs;
    new_bgr += rhs;
    return new_bgr;
}

BGR operator-(const BGR& lhs, const BGR& rhs) {
    BGR new_bgr = lhs;
    new_bgr -= rhs;
    return new_bgr;
}

BGR operator*(const BGR& lhs, float rhs) {
    BGR new_bgr = lhs;
    new_bgr *= rhs;
    return new_bgr;
}

struct BmpHeader {
    uint16_t header_field;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t bitmap_offset;

} __attribute__((packed));

struct DIBHeader {
    uint32_t header_size;
    uint32_t bitmap_width;
    uint32_t bitmap_height;
    uint16_t colors_planes;
    uint16_t bits_per_pixel;
    uint32_t compression_method;
    uint32_t image_size;
    uint32_t horizonal_resolution;
    uint32_t vertical_resolution;
    uint32_t color_palette;
    uint32_t important_colors;
} __attribute__((packed));

struct Bmp {
    BmpHeader bmp_header;
    DIBHeader dib_header;
    Matrix<BGR> data{0, 0, {}};

    void Read(const char* file_name) {
        std::ifstream file(file_name, std::ios_base::binary);
        if (!file.is_open()) {
            return;
        }
        file.read(reinterpret_cast<char*>(&bmp_header), sizeof(bmp_header));  // TODO: проверка всех входящих данных
        file.read(reinterpret_cast<char*>(&dib_header), sizeof(dib_header));
        file.seekg(bmp_header.bitmap_offset, file.beg);
        data = Matrix<BGR>{dib_header.bitmap_height, dib_header.bitmap_width, BGR{}};
        uint8_t padding = ((dib_header.bits_per_pixel * dib_header.bitmap_width) / Consts::eight) % Consts::four;
        uint32_t trash_can{0};
        for (size_t i = 0; i < data.GetRowsNum(); ++i) {
            for (size_t j = 0; j < data.GetColsNum(); ++j) {
                IntBGR int_bgr;
                file.read(reinterpret_cast<char*>(&int_bgr), sizeof(int_bgr));
                data.At(i, j) = BGR{int_bgr};
            }
            file.read(reinterpret_cast<char*>(&trash_can), padding);
        }
    }

    void Write(const char* file_name) {
        std::ofstream file(file_name, std::ios_base::binary);
        if (!file.is_open()) {
            return;
        }
        file.write(reinterpret_cast<char*>(&bmp_header), sizeof(bmp_header));  // TODO: проверка всех входящих данных
        file.write(reinterpret_cast<char*>(&dib_header), sizeof(dib_header));
        uint8_t padding =
            (Consts::four - ((dib_header.bits_per_pixel * dib_header.bitmap_width) / Consts::eight) % Consts::four) %
            Consts::four;
        for (size_t i = 0; i < data.GetRowsNum(); ++i) {
            for (size_t j = 0; j < data.GetColsNum(); ++j) {
                IntBGR int_bgr = data.At(i, j).ToInt();
                file.write(reinterpret_cast<char*>(&int_bgr), sizeof(int_bgr));
            }
            for (size_t k = 0; k < padding; ++k) {
                uint8_t zero = 0;
                file.write(reinterpret_cast<char*>(&zero), sizeof(uint8_t));
            }
        }
    }
};  //__attribute__((packed));

#endif  // IMG_PROC_BMP_H