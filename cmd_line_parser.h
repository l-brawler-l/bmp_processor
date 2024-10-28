

#ifndef IMG_PROC_CMD_LINE_PARSER_H
#define IMG_PROC_CMD_LINE_PARSER_H

#include <vector>
#include <string_view>

class FilterDescriptor {
public:
    explicit FilterDescriptor(std::string_view filter) : filter_(filter) {
    }
    FilterDescriptor(std::string_view filter, const std::vector<std::string_view>& params)
        : filter_(filter), params_(params) {
    }
    void AddParam(std::string_view param) {
        params_.push_back(param);
    }

    std::string_view GetFilterName() const {
        return filter_;
    }

    const std::vector<std::string_view>& GetFilterParams() const {
        return params_;
    }

private:
    std::string_view filter_;
    std::vector<std::string_view> params_;
};

// Следующий класс, который мы напишем, -- это CmdLineParser.
// Параметры argc, ardv можем передвать через конструктор, или через функцию Parse, или ещё как-то.
class CmdLineParser {
public:
    CmdLineParser(int argc, char** argv);

    const char* const GetInputFile() const {
        return input_file_;
    }

    const char* const GetOutputFile() const {
        return output_file_;
    }

    const std::vector<FilterDescriptor>& GetFilters() const {
        return filters_;
    }

private:
    std::vector<FilterDescriptor> filters_;
    char* input_file_;
    char* output_file_;
};

#endif  // IMG_PROC_CMD_LINE_PARSER_H
