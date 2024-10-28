#ifndef IMG_PROC_BLUR_H
#define IMG_PROC_BLUR_H

#include <charconv>
#include <exception>
#include "filter.h"

class BlurFilter : public Filter {
public:
    explicit BlurFilter(const FilterDescriptor& fd) {
        if (fd.GetFilterName() != "blur") {
            throw std::exception();
        }
        if (fd.GetFilterParams().size() != 1) {
            throw std::exception();
        }
        sigma_ = MakeFloat(fd.GetFilterParams()[0]);
    }
    void Apply(Bmp& bmp) override {
    }

private:
    float MakeFloat(std::string_view str) {
        float maybe = 0;
        auto result = std::from_chars(str.data(), str.data() + str.size(), maybe);
        if (result.ec == std::errc::invalid_argument) {
            throw std::exception();
        }
        return maybe;
    }

private:
    float sigma_;
};

Filter* BlurFilterMaker(const FilterDescriptor& fd) {
    Filter* pointer = new BlurFilter{fd};
    return pointer;
}

#endif