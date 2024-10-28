

#ifndef IMG_PROC_CMD_LINE_PARSER_H
#define IMG_PROC_CMD_LINE_PARSER_H

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <string_view>

// Описатель фильтра, который будет извлечён из командной строчки, сожержит: название фильтра без ведущего дефиса,
// параметры фильтра (коллекция) На этом этапе будем считать наиболее универсальным представлением параметров строку.

// Хранит описатель одного фильтра, извлечённого из параметров командной строки.
// Т.к. использует string_view, который не является владеющим классом, мы должны гарантировать, что подлежащий под ним
// указатель указывает на объект, время жизни которого гарантированно дольше фильтр-дескриптора. Для параметров
// командной строки это можно гарантировать. Если бы использовали char*, мы всего лишь обещали бы '\0' в конце, а все
// остальные требования остались бы прежними. Если бы необходимо было сделать дескриптор владеющим объектом, то можно
// было бы использовать string.
class FilterDescriptor {
public:
    explicit FilterDescriptor(std::string_view filter) : filter_(filter) {
    }
    FilterDescriptor(std::string_view filter, const std::vector<std::string_view>& params)
        : filter_(filter), params_(params) {
    }
    // Мы могли бы, например, оставить только один конструктор, а параметры добавлять с помощью метода AddParam:
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
    CmdLineParser(int argc, char** argv) {
        if (argc <= 2) {
            throw std::invalid_argument("Not enough arguments.");
        }
        input_file_ = argv[1];
        output_file_ = argv[2];
        filters_ = {};
        for (size_t i = 3; i < argc; ++i) {
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
    // Использовать string_view для названия файлом крайне неудачно, поэтому будем использовать char* или std::string
    // Т.к. мы не сможем открыть такой файл c-шными функциям, требующиеми c-style строки.
    // std::string_view input_;
    char* input_file_;
    char* output_file_;
};

#endif  // IMG_PROC_CMD_LINE_PARSER_H
