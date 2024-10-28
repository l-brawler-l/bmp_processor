#ifndef IMG_PROC_EDGE_H
#define IMG_PROC_EDGE_H

#include <cstdlib>
#include <memory>
#include "cmd_line_parser.h"
#include "filter.h"
#include "grayscale.h"

using namespace std::literals;

class EdgeFilter : public Filter {
public:
    ~EdgeFilter() override {
    }
    explicit EdgeFilter(double threshold) :
        threshold_(threshold), 
        gs_filter_(std::make_shared<GsFilter>(GsFilter())) {
    }
    void Apply(Bmp& bmp) override {
        gs_filter_->Apply(bmp);
        Matrix<BGR> new_data{bmp.data.GetRowsNum(), bmp.data.GetColsNum(), {}};
        for (size_t i = 0; i != bmp.data.GetRowsNum(); ++i) {
            for (size_t j = 0; j != bmp.data.GetColsNum(); ++j) {
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
    void Check(BGR& x) const {
        x.blue = x.blue > threshold_ ? 1 : 0;
        x.green = x.green > threshold_ ? 1 : 0;
        x.red = x.red > threshold_ ? 1 : 0;
    }

private:
    double threshold_;
    const double coef_[3][3] = {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}};
    std::shared_ptr<GsFilter> gs_filter_;
};

inline std::shared_ptr<Filter> EdgeFilterMaker(const FilterDescriptor& fd) {
    if (fd.GetFilterName() != "edge") {
        throw std::runtime_error("Filter descriptor has incorrect name");
    }
    if (fd.GetFilterParams().size() != 1) {
        throw std::invalid_argument("Incorrect count of parameters for edge filter.");
    }
    double threshold = TurnStringViewIntoDouble(fd.GetFilterParams()[0]);
    return std::make_shared<EdgeFilter>(threshold);
}

#endif