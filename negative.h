#ifndef IMG_PROC_NEGATIVE_H
#define IMG_PROC_NEGATIVE_H

#include "filter.h"

class NegFilter : public Filter {
public:
    explicit NegFilter(const FilterDescriptor& fd) {
        if (fd.GetFilterName() != "neg") {
            throw std::runtime_error("Filter descriptor has incorrect name");
        }
        if (!fd.GetFilterParams().empty()) {
            throw std::invalid_argument("Incorrect count of parameters for negative filter.");
        }
    }
    void Apply(Bmp& bmp) override {
        for (size_t i = 0; i < bmp.data.GetRowsNum(); ++i) {
            for (size_t j = 0; j < bmp.data.GetColsNum(); ++j) {
                bmp.data.At(i, j).blue = 1 - bmp.data.At(i, j).blue;
                bmp.data.At(i, j).green = 1 - bmp.data.At(i, j).green;
                bmp.data.At(i, j).red = 1 - bmp.data.At(i, j).red;
            }
        }
    }
};

inline std::shared_ptr<Filter> NegFilterMaker(const FilterDescriptor& fd) {
    return std::make_shared<NegFilter>(fd);
}

#endif