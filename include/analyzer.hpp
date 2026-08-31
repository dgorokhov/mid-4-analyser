#pragma once

#include <unistd.h>
#include <algorithm>
#include <any>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <optional>

namespace fs = std::filesystem;
namespace rv = std::ranges::views;
namespace rs = std::ranges;

// ============================================================================
// 1. ИЗ БЫВШЕГО file.hpp
// ============================================================================
namespace analyzer::file {

struct File {
    static inline const std::string command_prefix =
        "tree-sitter parse --config-path /root/.config/tree-sitter/config.json ";
    File(const std::string &filename);
    std::string name;
    std::string ast;
    std::vector<std::string> source_lines;

private:
    std::vector<std::string> ReadSourceFile(std::ifstream &file);
    std::string GetAst(const std::string &filename);
};

}  // namespace analyzer::file

// ============================================================================
// 2. ИЗ БЫВШЕГО function.hpp
// ============================================================================
namespace analyzer::function {

struct Function {
    std::string filename;
    std::optional<std::string> class_name;
    std::string name;
    std::string ast;
};

struct FunctionExtractor {
    std::vector<Function> Get(const analyzer::file::File &file);

private:
    struct Position {
        size_t line;
        size_t col;
    };

    struct FunctionNameLocation {
        Position start;
        Position end;
        std::string name;
    };

    struct ClassInfo {
        std::string name;
        Position start;
        Position end;
    };

    FunctionNameLocation GetNameLocation(const std::string &function_ast);
    std::string GetNameFromSource(const std::string &function_ast, const std::vector<std::string> &lines);
    std::optional<ClassInfo> FindEnclosingClass(const std::string &ast, const FunctionNameLocation &func_loc);
    std::string GetClassNameFromSource(const ClassInfo &class_info, const std::vector<std::string> &lines);
};

}  // namespace analyzer::function

// ============================================================================
// 3. ИЗ БЫВШЕГО metric.hpp + КЛАССЫ МЕТРИК
// ============================================================================
namespace analyzer::metric {

struct MetricResult {
    using ValueType = int;
    std::string metric_name;  // Название метрики
    ValueType value;          // Значение метрики
};

struct IMetric {
    virtual ~IMetric() = default;
    MetricResult Calculate(const function::Function &f) const {
        return MetricResult{.metric_name = Name(), .value = CalculateImpl(f)};
    }

protected:
    virtual MetricResult::ValueType CalculateImpl(const function::Function &f) const = 0;
    virtual std::string Name() const = 0;
};

using MetricResults = std::vector<MetricResult>;

struct MetricExtractor {
    void RegisterMetric(std::unique_ptr<IMetric> metric);
    MetricResults Get(const function::Function &func) const;
    std::vector<std::unique_ptr<IMetric>> metrics;
};

// --- Сюда же переносим объявления самих метрик ---
namespace metric_impl {

class CodeLinesCountMetric : public IMetric {
public:
    static inline const std::string kName = "code_lines_count";
protected:
    MetricResult::ValueType CalculateImpl(const function::Function &f) const override;
    std::string Name() const override { return kName; }
};

class CyclomaticComplexityMetric : public IMetric {
public:
    static inline const std::string kName = "cyclomatic_complexity";
protected:
    MetricResult::ValueType CalculateImpl(const function::Function &f) const override;
    std::string Name() const override { return kName; }
};

class CountParametersMetric : public IMetric {
public:
    static inline const std::string kName = "parameters_count";
protected:
    MetricResult::ValueType CalculateImpl(const function::Function &f) const override;
    std::string Name() const override { return kName; }
};

} // namespace metric_impl
}  // namespace analyzer::metric
