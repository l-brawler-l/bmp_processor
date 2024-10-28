#ifndef IMG_PROC_CROP_H
#define IMG_PROC_CROP_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <charconv>

#include "filter.h"

class CropFilter : public Filter {
public:
    explicit CropFilter(const FilterDescriptor& fd) {
        if (fd.GetFilterName() != "crop") {
            throw std::runtime_error("Filter descriptor has incorrect name");
        }
        if (fd.GetFilterParams().size() != 2) {
            throw std::invalid_argument("Incorrect count of parameters for crop filter.");
        }
        width_ = MakeInteger(fd.GetFilterParams()[0]);
        height_ = MakeInteger(fd.GetFilterParams()[1]);
        if (width_ <= 0 || height_ <= 0) {
            throw std::invalid_argument("Parameters for crop filter must be positive integers.");
        }
    }
    void Apply(Bmp& bmp) override {
        if (height_ > bmp.data.GetRowsNum()) {
            height_ = bmp.data.GetRowsNum();
        }
        if (width_ > bmp.data.GetColsNum()) {
            width_ = bmp.data.GetColsNum();
        }
        Matrix<BGR> new_data{height_, width_, {}};
        for (size_t i = 0; i != static_cast<size_t>(height_); ++i) {
            for (size_t j = 0; j != static_cast<size_t>(width_); ++j) {
                new_data.At(i, j) = bmp.data.At(i + bmp.data.GetRowsNum() - height_, j);  //!!!!!
            }
        }
        bmp.dib_header.bitmap_height = height_;
        bmp.dib_header.bitmap_width = width_;
        bmp.dib_header.image_size = bmp.dib_header.bitmap_height * bmp.dib_header.bitmap_width *
                                    (bmp.dib_header.bits_per_pixel / Consts::eight);
        bmp.bmp_header.size = bmp.dib_header.image_size + bmp.dib_header.header_size + sizeof(bmp.bmp_header);
        bmp.data = new_data;
    }

private:
    uint32_t MakeInteger(std::string_view str) {
        uint32_t maybe = 0;
        auto result = std::from_chars(str.data(), str.data() + str.size(), maybe);
        if (result.ec == std::errc::invalid_argument) {
            throw std::invalid_argument("Patameters for crop filter must be positive integers.");
        }
        return maybe;
    }

private:
    uint32_t width_;
    uint32_t height_;
};

inline std::shared_ptr<Filter> CropFilterMaker(const FilterDescriptor& fd) {
    return std::make_shared<CropFilter>(fd);
}

#endif