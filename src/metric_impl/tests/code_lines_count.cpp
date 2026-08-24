#include "metric_impl/code_lines_count.hpp"
#include "file.hpp"
#include "function.hpp"
#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric_impl/parameters_count.hpp"  // проверь точное имя
#include <gtest/gtest.h>

using namespace analyzer::metric::metric_impl;

// здесь ваш код//TEST(BasicCheck, Sum) { EXPECT_EQ(1 + 1, 2); }

TEST(MetricIntegrationTest, FuncCommentsFile) {
    // 1. Загружаем и парсим реальный файл с диска через tree-sitter
    // Путь должен быть относительно места запуска тестов (обычно корня проекта)
    std::string filepath = "src/metric_impl/tests/files/func_comments.py";

    ASSERT_NO_THROW({
        analyzer::file::File test_file(filepath);
        analyzer::function::FunctionExtractor extractor;
        auto functions = extractor.Get(test_file);

        // Проверяем, что в файле нашлась наша функция
        ASSERT_EQ(functions.size(), 1);
        const auto &func = functions[0];

        // Проверяем имя функции
        EXPECT_EQ(func.name, "Func_comments");

        // 2. Тестируем метрику количества параметров (ожидаем 3: result, a, b)
        CountParametersMetric param_metric;
        EXPECT_EQ(param_metric.Calculate(func).value, 3);

        // 3. Тестируем цикломатическую сложность (ожидаем 1, так как ветвлений нет)
        CyclomaticComplexityMetric complexity_metric;
        EXPECT_EQ(complexity_metric.Calculate(func).value, 1);

        // 4. Тестируем количество строк кода (ожидаем 3 строки чистого кода в теле)
        CodeLinesCountMetric lines_metric;
        EXPECT_EQ(lines_metric.Calculate(func).value, 3);
    });
}