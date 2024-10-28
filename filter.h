#ifndef IMG_PROC_FILTER_H
#define IMG_PROC_FILTER_H

#include "bmp.h"
#include "cmd_line_parser.h"
#include <memory>
#include <stdexcept>
#include <vector>
#include <map>

class Filter {
public:
    virtual void Apply(Bmp& bmp) = 0;
    virtual ~Filter(){};
};

class FilterPipeline {
public:
    FilterPipeline() {
    }

    explicit FilterPipeline(const std::vector<std::shared_ptr<Filter>>& fp) : fp_{fp} {
    }

    ~FilterPipeline() {
    }

    void AddFilter(std::shared_ptr<Filter> filter) {
        fp_.push_back(filter);
    }

    void DeleteFilter() {
        fp_.pop_back();
    }

    void ApplyFilters(Bmp& bmp) {
        for (std::shared_ptr<Filter> filter : fp_) {
            filter->Apply(bmp);
        }
    }

    std::shared_ptr<Filter> operator[](size_t ind) {
        return fp_[ind];
    }
    const std::shared_ptr<Filter const> operator[](size_t ind) const {
        return fp_[ind];
    }

    std::shared_ptr<Filter> At(size_t ind) {
        return fp_.at(ind);
    }
    const std::shared_ptr<Filter const> At(size_t ind) const {
        return fp_.at(ind);
    }

    const std::vector<std::shared_ptr<Filter>>& GetFilter() const {
        return fp_;
    }

private:
    std::vector<std::shared_ptr<Filter>> fp_;
};

using PFitlerProducer = std::shared_ptr<Filter> (*)(const FilterDescriptor& fd);

class FilterFactory {
public:
    FilterFactory() {
    }

    explicit FilterFactory(const std::map<std::string_view, PFitlerProducer>& filter_producers)
        : filter_producers_{filter_producers} {
    }

    void AddFilterProducer(std::pair<std::string, PFitlerProducer> fp) {
        if (IsInFilterProducers(fp.first)) {
            throw std::invalid_argument("Another FilterProducer in FilterFactory has the same name.");
        }
        filter_producers_[fp.first] = fp.second;
    }

    void DeleteFilterProducer(std::string name) {
        if (!IsInFilterProducers(name)) {
            throw std::out_of_range("This FilterProducer is not in FilterFactory.");
        }
        filter_producers_.erase(name);
    }
    std::shared_ptr<Filter> MakeFilter(const FilterDescriptor& fd) {
        if (!IsInFilterProducers(fd.GetFilterName())) {
            throw std::invalid_argument("No registred filter producers for this filter descriptor.");
        }
        PFitlerProducer make_filter = filter_producers_[fd.GetFilterName()];
        std::shared_ptr<Filter> filter = make_filter(fd);
        if (!filter) {
            throw std::invalid_argument("Parameters of this filter descriptor are incorrect.");
        }
        return filter;
    }

    std::unique_ptr<FilterPipeline> MakePipeline(const std::vector<FilterDescriptor>& fds) {
        std::unique_ptr<FilterPipeline> fp(new FilterPipeline());
        for (const FilterDescriptor& fd : fds) {
            std::shared_ptr<Filter> filter = MakeFilter(fd);
            if (!filter) {
                throw std::invalid_argument(
                    "You cannot create a filter for this filter descriptor, because either the parameters are "
                    "incorrect, or filter descriptor is not added to filter producers");
            }
            fp->AddFilter(filter);
        }
        return fp;
    }

    const std::map<std::string_view, PFitlerProducer>& GetFilterProducers() const {
        return filter_producers_;
    }

private:
    bool IsInFilterProducers(std::string_view name) const {
        return filter_producers_.find(name) != filter_producers_.end();
    }

private:
    std::map<std::string_view, PFitlerProducer> filter_producers_;
};

#endif