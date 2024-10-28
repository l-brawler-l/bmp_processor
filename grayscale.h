#ifndef IMG_PROC_GRAYSCALE_H
#define IMG_PROC_GRAYSCALE_H

#include <cstddef>
#include <exception>
#include "filter.h"
#include "bmp.h"

class GsFilter : public Filter {
public:
    explicit GsFilter(const FilterDescriptor& fd) {
        if (fd.GetFilterName() != "gs") {
            throw std::exception();
        }
        if (!fd.GetFilterParams().empty()) {
            throw std::exception();
        }
    }
    void Apply(Bmp& bmp) override {
        for (size_t i = 0; i < bmp.data.GetRowsNum(); ++i) {
            for (size_t j = 0; j < bmp.data.GetColsNum(); ++j) {
                float color = red_coef_ * bmp.data.At(i, j).red + green_coef_ * bmp.data.At(i, j).green +
                              blue_coef_ * bmp.data.At(i, j).blue;
                bmp.data.At(i, j) = BGR{color, color, color};
            }
        }
    }

private:
    const float red_coef_ = 0.299;
    const float green_coef_ = 0.587;
    const float blue_coef_ = 0.114;
};

Filter* GsFilterMaker(const FilterDescriptor& fd) {
    Filter* pointer = new GsFilter{fd};
    return pointer;
}

#endif