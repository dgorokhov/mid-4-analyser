#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator_impl/categorical_accumulator.hpp" 
#include "metric_accumulator_impl/sum_average_accumulator.hpp" // проверь точный путь к hpp
#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric_impl/parameters_count.hpp"  // проверь точное имя
#include "metric_impl/code_lines_count.hpp"

#include <gtest/gtest.h>



///****************************************************************************************
// MetricsTests
///****************************************************************************************

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




///****************************************************************************************
// SumAverageAccumulatorTest
///****************************************************************************************


using namespace analyzer::metric;
using namespace analyzer::metric_accumulator::metric_accumulator_impl;

// Тест 1: Проверка начального состояния после создания (должно быть 0)
TEST(SumAverageAccumulatorTest, InitialStateIsZero) {
    SumAverageAccumulator acc;
    acc.Finalize();
    EXPECT_EQ(acc.Get().sum, 0);
    EXPECT_EQ(acc.Get().average, 0);
}

// Тест 2: Аккумуляция одного значения
TEST(SumAverageAccumulatorTest, AccumulateSingleValue) {
    SumAverageAccumulator acc;
    MetricResult res{.metric_name = "code_lines_count", .value = 10};
    
    acc.Accumulate(res);
    acc.Finalize();

    EXPECT_EQ(acc.Get().sum, 10);
    EXPECT_EQ(acc.Get().average, 10);
}

// Тест 3: Аккумуляция нескольких значений и проверка среднего
TEST(SumAverageAccumulatorTest, AccumulateMultipleValues) {
    SumAverageAccumulator acc;
    
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 10});
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 20});
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 30});
    acc.Finalize();
    
    EXPECT_EQ(acc.Get().sum, 60);
    EXPECT_EQ(acc.Get().average, 20); // (10 + 20 + 30) / 3 = 20
}

// Тест 4: Проверка корректной работы метода Reset()
TEST(SumAverageAccumulatorTest, ResetClearsState) {
    SumAverageAccumulator acc;
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 100});
    acc.Finalize();
    acc.Reset();
    acc.Finalize();
    EXPECT_EQ(acc.Get().sum, 0);
    EXPECT_EQ(acc.Get().average, 0);
}

// Тест 5: Обработка отрицательных или граничных значений (если применимо к метрикам)
TEST(SumAverageAccumulatorTest, HandlesLargeValues) {
    SumAverageAccumulator acc;
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 1000});
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 2000});
    acc.Finalize();
    
    EXPECT_EQ(acc.Get().sum, 3000);
    EXPECT_EQ(acc.Get().average, 1500);
}




///****************************************************************************************
// AverageAccumulatorTest
///****************************************************************************************
/*

// Тест 1: Проверка начального состояния после создания (должно быть 0)
TEST(AverageAccumulatorTest, InitialStateIsZero) {
    SumAverageAccumulator acc;
    acc.Finalize();
    EXPECT_EQ(acc.Get().sum, 0);
    EXPECT_EQ(acc.Get().average, 0);
}

// Тест 2: Аккумуляция одного значения
TEST(AverageAccumulatorTest, AccumulateSingleValue) {
    SumAverageAccumulator acc;
    MetricResult res{.metric_name = "code_lines_count", .value = 10};
    acc.Accumulate(res);
    acc.Finalize();

    EXPECT_EQ(acc.Get().sum, 10);
    EXPECT_EQ(acc.Get().average, 10);
}

// Тест 3: Аккумуляция нескольких значений и проверка среднего
TEST(AverageAccumulatorTest, AccumulateMultipleValues) {
    SumAverageAccumulator acc;
    
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 10});
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 20});
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 30});
    acc.Finalize();
    
    EXPECT_EQ(acc.Get().sum, 60);
    EXPECT_EQ(acc.Get().average, 20); // (10 + 20 + 30) / 3 = 20
}

// Тест 4: Проверка корректной работы метода Reset()
TEST(AverageAccumulatorTest, ResetClearsState) {
    SumAverageAccumulator acc;
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 100});
    acc.Finalize();
    acc.Reset();
    acc.Finalize();
    EXPECT_EQ(acc.Get().sum, 0);
    EXPECT_EQ(acc.Get().average, 0);
}

// Тест 5: Обработка отрицательных или граничных значений (если применимо к метрикам)
TEST(AverageAccumulatorTest, HandlesLargeValues) {
    SumAverageAccumulator acc;
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 1000});
    acc.Accumulate(MetricResult{.metric_name = "code_lines_count", .value = 2000});
    acc.Finalize();
    
    EXPECT_EQ(acc.Get().sum, 3000);
    EXPECT_EQ(acc.Get().average, 1500);
}


///****************************************************************************************
// CategoricalAccumulatorTest
///****************************************************************************************


using namespace analyzer::metric;
using namespace analyzer::metric_accumulator::metric_accumulator_impl;

// Тест 1: Проверка пустого аккумулятора сразу после создания
TEST(CategoricalAccumulatorTest, InitialStateIsEmpty) {
    CategoricalAccumulator acc;
    acc.Finalize();
    EXPECT_TRUE(acc.Get().empty());
}
// Теst 2: Добавление одной категории метрики
TEST(CategoricalAccumulatorTest, AccumulateSingleCategory) {
    CategoricalAccumulator acc;
    MetricResult res{.metric_name = "naming_style", .value = 1}; // например, 1 означает CamelCase
    acc.Finalize();    
    acc.Accumulate(res);
    acc.Finalize();
 
    EXPECT_FALSE(acc.Get().empty());
    // Здесь проверяется внутренняя логика (например, счетчик для категории '1' равен 1)
    // EXPECT_EQ(acc.Get().at(1), 1); 
}
// Тест 3: Подсчет частоты разных категорий
TEST(CategoricalAccumulatorTest, CountsMultipleCategories) {
    CategoricalAccumulator acc;
    
    acc.Accumulate(MetricResult{.metric_name = "naming_style", .value = 1});
    acc.Accumulate(MetricResult{.metric_name = "naming_style", .value = 2});
    acc.Accumulate(MetricResult{.metric_name = "naming_style", .value = 1});
    acc.Finalize();
    
    auto results = acc.Get();
    // Проверь, как именно возвращаются данные из CategoricalAccumulator, 
    // и адаптируй эти ассерты под сигнатуру метода Get()
}
// Тест 4: Сброс состояния аккумулятора категорий
TEST(CategoricalAccumulatorTest, ResetClearsCategories) {
    CategoricalAccumulator acc;
    acc.Accumulate(MetricResult{.metric_name = "naming_style", .value = 3});
    acc.Reset();
    EXPECT_TRUE(acc.Get().empty());
}

// Тест 5: Повторная финализация не ломает данные
TEST(CategoricalAccumulatorTest, MultipleFinalizeCalls) {
    CategoricalAccumulator acc;
    acc.Accumulate(MetricResult{.metric_name = "naming_style", .value = 2});
    acc.Finalize();
    auto res1 = acc.Get();
    acc.Finalize();
    auto res2 = acc.Get();
    // Результаты не должны задваиваться или ломаться от повторного вызова Finalize
    EXPECT_EQ(res1, res2);
}


*/
