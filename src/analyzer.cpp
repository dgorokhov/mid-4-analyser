#include <array>
#include <string_view>
#include <ranges>
#include <numeric>

#include "analyzer.hpp"
#include <sys/wait.h>


namespace analyzer::file {

File::File(const std::string &filename) : name{filename} {
    std::ifstream file(name);
    if (!file.is_open()) {
        throw std::invalid_argument("Can't open file " + filename);
    }
    ast = GetAst(filename);
    source_lines = ReadSourceFile(file);
}

std::vector<std::string> File::ReadSourceFile(std::ifstream &file) {
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::string File::GetAst(const std::string &filename) try {
    std::string full_cmd = File::command_prefix + filename + " 2>&1";
    std::string result;
    std::array<char, 256> buffer;

    using PipePtr = std::unique_ptr<FILE, decltype([](FILE *pipe) {
        if (pipe) pclose(pipe);
    })>;

    FILE *raw_pipe = popen(full_cmd.c_str(), "r");
    if (!raw_pipe) {
        throw std::runtime_error("Failed to execute command: " + std::string(std::strerror(errno)));
    }
    PipePtr pipe(raw_pipe);

    while (fgets(buffer.data(), buffer.size(), pipe.get())) {
        result += buffer.data();
    }

    FILE* raw_ptr = pipe.release(); 
    int status = pclose(raw_ptr);

    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        if (exit_status != 0) {
            throw std::runtime_error("Command failed with exit code " + std::to_string(exit_status));
        }
    } else {
        throw std::runtime_error("Command terminated abnormally");
    }

    return result;
} catch (const std::exception &e) {
    std::cerr << "Internal error: " << e.what() << std::endl;
    throw std::runtime_error("Error while getting ast from " + filename);
}

}  // namespace analyzer::file


namespace analyzer::metric {

void MetricExtractor::RegisterMetric(std::unique_ptr<IMetric> metric) {
    if (metric) {
        metrics.push_back(std::move(metric));
    }
}

MetricResults MetricExtractor::Get(const function::Function &func) const {
    auto results_view = metrics 
                      | std::views::transform([&](const auto &metric) { return metric->Calculate(func); });
    return MetricResults(results_view.begin(), results_view.end());
}

// ============================================================================
// РЕАЛИЗАЦИЯ ВСЕХ ТРЕХ МЕТРИК КОДА
// ============================================================================
namespace metric_impl {

MetricResult::ValueType CodeLinesCountMetric::CalculateImpl(const function::Function &f) const {
    auto &function_ast = f.ast;

    auto line_number = [&](int start_pos) {
        size_t line_pos = function_ast.find("[", start_pos);
        if (line_pos == std::string::npos) return 0;
        size_t comma_pos = function_ast.find(",", line_pos);
        if (comma_pos == std::string::npos) return 0;
        return std::stoi(function_ast.substr(line_pos + 1, comma_pos - line_pos - 1));
    };

    const int start_line = line_number(0);
    size_t last_hyphen = function_ast.rfind("] -");
    const int end_line = (last_hyphen != std::string::npos) ? line_number(last_hyphen) : start_line;

    auto is_code_line = [&](int line) {
        size_t pos = 0;
        bool has_real_code = false;
        while (true) {
            pos = function_ast.find("[", pos);
            if (pos == std::string::npos) break;

            size_t comma_pos = function_ast.find(",", pos);
            if (comma_pos != std::string::npos) {
                int parsed_line = std::stoi(function_ast.substr(pos + 1, comma_pos - pos - 1));
                if (parsed_line == line) {
                    size_t node_start = function_ast.rfind('(', pos);
                    if (node_start != std::string::npos) {
                        std::string_view node_type =
                            std::string_view(function_ast)
                                .substr(node_start + 1, function_ast.find_first_of(" \n[", node_start + 1) - node_start - 1);
                        if (node_type != "comment") {
                            has_real_code = true;
                            break;
                        }
                    }
                }
            }
            pos++;
        }
        return has_real_code;
    };

    if (start_line >= end_line) {
        return is_code_line(start_line) ? 1 : 0;
    }

    auto code_lines_range = std::views::iota(start_line + 1, end_line + 1) |
                            std::views::filter([&](int line) { return is_code_line(line); });

    return std::ranges::distance(code_lines_range);
}
/*
MetricResult::ValueType CyclomaticComplexityMetric::CalculateImpl(const function::Function &f) const {
    auto &function_ast = f.ast;
    constexpr std::array<std::string_view, 9> complexity_nodes = {
        "if_statement", "elif_statement", "for_statement", "while_statement",
        "try_statement", "finally_clause", "case_clause", "assert", "conditional_expression"
    };

    MetricResult::ValueType complexity = 1;
    for (const auto node_type : complexity_nodes) {
        size_t pos = function_ast.find(node_type, 0);
        while (pos != std::string::npos) {
            complexity++;
            pos = function_ast.find(node_type, pos + node_type.length());
        }
    }
    return complexity;
}
*/

MetricResult::ValueType CyclomaticComplexityMetric::CalculateImpl(const function::Function &f) const {
    const auto &function_ast = f.ast;
    
    constexpr std::array<std::string_view, 9> complexity_nodes = {
        "if_statement", "elif_statement", "for_statement", "while_statement",
        "try_statement", "finally_clause", "case_clause", "assert", "conditional_expression"
    };

    auto counts_view = complexity_nodes | std::views::transform([&function_ast](std::string_view node_type) {
        MetricResult::ValueType count = 0;
        size_t pos = function_ast.find(node_type, 0);
        while (pos != std::string::npos) {
            count++;
            pos = function_ast.find(node_type, pos + node_type.length());
        }
        return count;
    });

    // В C++20 используем accumulate для подсчета суммы из view
    return std::accumulate(counts_view.begin(), counts_view.end(), MetricResult::ValueType{1});
}


MetricResult::ValueType CountParametersMetric::CalculateImpl(const function::Function &f) const {
    auto &function_ast = f.ast;
    const std::string parameters_marker = "(parameters";
    size_t params_start = function_ast.find(parameters_marker);
    if (params_start == std::string::npos) return 0;

    size_t balance = 1;
    size_t params_end = params_start + parameters_marker.length();
    while (params_end < function_ast.size() && balance > 0) {
        if (function_ast[params_end] == '(') balance++;
        else if (function_ast[params_end] == ')') balance--;
        params_end++;
    }

    std::string_view params_block(function_ast.data() + params_start, params_end - params_start);
    int count = 0;
    size_t pos = 0;
    const std::string id_marker = "(identifier";

    while ((pos = params_block.find(id_marker, pos)) != std::string_view::npos) {
        count++;
        pos += id_marker.length();
    }
    return count;
}

} // namespace metric_impl
} // namespace analyzer::metric
