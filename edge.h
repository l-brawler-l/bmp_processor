#ifndef IMG_PROC_EDGE_H
#define IMG_PROC_EDGE_H

#include <charconv>
#include "filter.h"
#include "grayscale.h"

class EdgeFilter : public Filter {
public:
    ~EdgeFilter() override {
        delete gs_filter_;
    }
    explicit EdgeFilter(const FilterDescriptor& fd) {
        if (fd.GetFilterName() != "edge") {
            throw std::exception();
        }
        if (fd.GetFilterParams().size() != 1) {
            throw std::exception();
        }
        threshold_ = MakeFloat(fd.GetFilterParams()[0]);
        gs_filter_ = new GsFilter{{"gs", {}}};
    }
    void Apply(Bmp& bmp) override {
        gs_filter_->Apply(bmp);
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
    float MakeFloat(std::string_view str) {
        float maybe = 0;
        auto result = std::from_chars(str.data(), str.data() + str.size(), maybe);
        if (result.ec == std::errc::invalid_argument) {
            throw std::exception();
        }
        return maybe;
    }

    void Check(BGR& x) const {
        x.blue = x.blue > threshold_ ? 1 : 0;
        x.green = x.green > threshold_ ? 1 : 0;
        x.red = x.red > threshold_ ? 1 : 0;
    }

private:
    float threshold_;
    const float coef_[3][3] = {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}};
    GsFilter* gs_filter_;
};

Filter* EdgeFilterMaker(const FilterDescriptor& fd) {
    Filter* pointer = new EdgeFilter{fd};
    return pointer;
}

#endif