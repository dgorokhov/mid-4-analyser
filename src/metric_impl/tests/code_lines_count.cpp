#include "metric_impl/code_lines_count.hpp"
#include "file.hpp"
#include "function.hpp"
#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric_impl/parameters_count.hpp"  // проверь точное имя
#include <gtest/gtest.h>

using namespace analyzer::metric::metric_impl;

// здесь ваш код//TEST(BasicCheck, Sum) { EXPECT_EQ(1 + 1, 2); }

// 1
TEST(MetricIntegrationTest, CommentsFile) {
    std::string filepath = "comments.py";

    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);
        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];
        EXPECT_EQ(func.name, "Func_comments");
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 3);
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 1);
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 3);
    });
}
// 2
TEST(MetricIntegrationTest, TryExceptionsFile) {
    std::string filepath = "exceptions.py";

    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);
        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];
        EXPECT_EQ(func.name, "Try_Exceptions");
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 0);
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 4);
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 7);
    });
}
// 3
TEST(MetricIntegrationTest, TestIfFile) {
    std::string filepath = "if.py";
    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);
        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];
        EXPECT_EQ(func.name, "testIf");
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 1);
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 2);
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 3);
    });
}
// 4
TEST(MetricIntegrationTest, TestMultilineFile) {
    std::string filepath = "many_lines.py";  // проверь точное имя файла
    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);
        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];
        EXPECT_EQ(func.name, "testmultiline");
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 0);
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 2);
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 11);
    });
}
// 5
TEST(MetricIntegrationTest, TestMultiParametersFile) {
    std::string filepath = "many_parameters.py";  // проверь точное имя файла

    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);

        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];
        EXPECT_EQ(func.name, "__test_multiparameters__");
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 5);
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 2);
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 1);
    });
}
// 6
TEST(MetricIntegrationTest, TestMatchCaseFile) {
    std::string filepath = "match_case.py";  // проверь имя файла на диске
    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);
        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];
        EXPECT_EQ(func.name, "test_Match_case");
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 1);
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 4);
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 7);
    });
}
// 7
TEST(MetricIntegrationTest, TestNestedIfFile) {
    std::string filepath = "nested_if.py";  // проверь точное имя файла

    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);
        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];
        EXPECT_EQ(func.name, "Testnestedif");
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 2);
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 4);
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 8);
    });
}
// 8
TEST(MetricIntegrationTest, TestSimpleFile) {
    std::string filepath = "simple.py";  // проверь имя файла на диске

    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);
        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];
        EXPECT_EQ(func.name, "test_simple");
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 0);
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 2);
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 5);
    });
}
// 9
TEST(MetricIntegrationTest, TestTernaryFile) {
    std::string filepath = "ternary.py";  // проверь имя файла на диске

    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);
        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];
        EXPECT_EQ(func.name, "teSt_ternary");
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 1);
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 3);
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 1);
    });
}

// 10
TEST(MetricIntegrationTest, TestLoopsFile) {
    std::string filepath = "loops.py";  // проверь точное имя файла

    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);

        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];
        EXPECT_EQ(func.name, "TestLoops");
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 1);
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 4);
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 6);
    });
}
