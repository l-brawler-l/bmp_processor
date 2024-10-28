#include "cmd_line_parser.h"
#include <stdexcept>


CmdLineParser::CmdLineParser(int argc, char** argv) {
    if (argc <= 2) {
        throw std::invalid_argument("Not enough arguments.");
    }
    input_file_ = argv[1];
    output_file_ = argv[2];
    filters_ = {};
    for (size_t i = 3; i < static_cast<size_t>(argc); ++i) {
        if (argv[i][0] == '-') {
            std::string_view filter{argv[i]};
            filter = filter.substr(1);
            filters_.push_back(FilterDescriptor{filter});
        } else {
            if (filters_.empty()) {
                throw std::out_of_range(
                    "The filter argument must be preceded by the name of the filter, starting with a hyphen.");
            }
            filters_.back().AddParam(argv[i]);
        }
    }
}   