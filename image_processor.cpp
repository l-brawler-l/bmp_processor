#include <cassert>
#include <exception>
#include <iostream>
#include <stdexcept>

#include "cmd_line_parser.h"
#include "bmp.h"
#include "filter.h"

#include "crop.h"
#include "grayscale.h"
#include "negative.h"
#include "sharpening.h"
#include "edge.h"

void Test01(int argc, char** argv) {
    CmdLineParser xd{argc, argv};
    std::cout << xd.GetInputFile() << " " << xd.GetOutputFile() << std::endl;
    for (const auto& elem : xd.GetFilters()) {
        std::cout << elem.GetFilterName() << std::endl;
        for (auto i : elem.GetFilterParams()) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
}

void Test02(){
    Bmp bmp;
    std::string xd = "test.bmp";
    std::string xd_out = "out.bmp";
    char* str = xd.data();
    bmp.Read(str);
    bmp.Write(xd_out.data());
    assert(bmp.bmp_header.header_field == 19778);
    assert(bmp.bmp_header.size == 70);
    assert(bmp.bmp_header.reserved1 == 0);
    assert(bmp.bmp_header.reserved2 == 0);
    assert(bmp.bmp_header.bitmap_offset == 54);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Programm, that allows you to apply various filters to images, similar to filters in popular "
                     "graphic editors."
                  << std::endl;
        std::cout
            << "Use {programm_name} {input_file_path} {output_file_path} [-{filter_1_name} [filter_1_argument_1] "
               "[filter_1_argument_2] ...] [-{filter_2_name} [filter_2_argument_1] [filter_2_argument_2] ...] ...";
        return 0;
    }
    try {
        CmdLineParser clp{argc, argv};
        FilterFactory ff;
        ff.AddFilterProducer({"crop", CropFilterMaker});
        ff.AddFilterProducer({"gs", GsFilterMaker});
        ff.AddFilterProducer({"neg", NegFilterMaker});
        ff.AddFilterProducer({"sharp", SharpFilterMaker});
        ff.AddFilterProducer({"edge", EdgeFilterMaker});
        std::unique_ptr fp = ff.MakePipeline(clp.GetFilters());
        Bmp bmp;
        bmp.Read(clp.GetInputFile());
        fp->ApplyFilters(bmp);
        bmp.Write(clp.GetOutputFile());
    } catch(const std::logic_error& e) {
        std::cout << "logic error: " << e.what() << std::endl;
    }  catch(const std::runtime_error& e) {
        std::cout << "runtime error: " << e.what() << std::endl;
    }  catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    } 
    return 0;
}
