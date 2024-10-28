#ifndef IMG_PROC_GRAYSCALE_H
#define IMG_PROC_GRAYSCALE_H

#include <cstddef>
#include <memory>
#include "filter.h"
#include "bmp.h"

class GsFilter : public Filter {
public:
    explicit GsFilter() {}
    void Apply(Bmp& bmp) override {
        for (size_t i = 0; i != bmp.data.GetRowsNum(); ++i) {
            for (size_t j = 0; j != bmp.data.GetColsNum(); ++j) {
                double color = red_coef_ * bmp.data.At(i, j).red + green_coef_ * bmp.data.At(i, j).green +
                              blue_coef_ * bmp.data.At(i, j).blue;
                bmp.data.At(i, j) = BGR{color, color, color};
            }
        }
    }

private:
    const double red_coef_ = 0.299;
    const double green_coef_ = 0.587;
    const double blue_coef_ = 0.114;
};

inline std::shared_ptr<Filter> GsFilterMaker(const FilterDescriptor& fd) {
    if (fd.GetFilterName() != "gs") {
        throw std::runtime_error("Filter descriptor has incorrect name");
    }
    if (!fd.GetFilterParams().empty()) {
        throw std::invalid_argument("Incorrect count of parameters for grayscale filter.");
    }
    return std::make_shared<GsFilter>(GsFilter());
}

#endif