#ifndef IMG_PROC_NEGATIVE_H
#define IMG_PROC_NEGATIVE_H

#include "filter.h"

class NegFilter : public Filter {
public:
    explicit NegFilter(const FilterDescriptor& fd) {
        if (fd.GetFilterName() != "neg") {
            throw std::exception();
        }
        if (!fd.GetFilterParams().empty()) {
            throw std::exception();
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

Filter* NegFilterMaker(const FilterDescriptor& fd) {
    Filter* pointer = new NegFilter{fd};
    return pointer;
}

#endif