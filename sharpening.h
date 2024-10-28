#ifndef IMG_PROC_SHARPENING_H
#define IMG_PROC_SHARPENING_H

#include "filter.h"

class SharpFilter : public Filter {
public:
    explicit SharpFilter(const FilterDescriptor& fd) {
        if (fd.GetFilterName() != "sharp") {
            throw std::exception();
        }
        if (!fd.GetFilterParams().empty()) {
            throw std::exception();
        }
    }
    void Apply(Bmp& bmp) override {
        Matrix<BGR> new_data{bmp.data.GetRowsNum(), bmp.data.GetColsNum(), {}};
        for (size_t i = 0; i < bmp.data.GetRowsNum(); ++i) {
            for (size_t j = 0; j < bmp.data.GetColsNum(); ++j) {
                new_data.At(i, j) = bmp.data.At(i, j) * coef_[1][1];
                new_data.At(i, j) += bmp.data.At(i == 0 ? 0 : i - 1, j) * coef_[0][1];
                new_data.At(i, j) += bmp.data.At(i == (bmp.data.GetRowsNum() - 1) ? i : i + 1, j) * coef_[2][1];
                new_data.At(i, j) += bmp.data.At(i, j == 0 ? 0 : j - 1) * coef_[1][0];
                new_data.At(i, j) += bmp.data.At(i, j == (bmp.data.GetColsNum() - 1) ? j : j + 1) * coef_[1][2];
                Check(new_data.At(i, j));
            }
        }
        bmp.data = new_data;
    }

private:
    void Check(BGR& x) {
        x.blue = std::min(one_, std::max(zero_, x.blue));
        x.green = std::min(one_, std::max(zero_, x.green));
        x.red = std::min(one_, std::max(zero_, x.red));
    }

private:
    const float coef_[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
    const float zero_ = 0;
    const float one_ = 1;
};

Filter* SharpFilterMaker(const FilterDescriptor& fd) {
    Filter* pointer = new SharpFilter{fd};
    return pointer;
}

#endif