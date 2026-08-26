#include "metric_accumulator.hpp"
#include "analyzer.hpp" // 💡 В .cpp файле инклуд абсолютно безопасен!

namespace analyzer::metric_accumulator {

// ==========================================
// MetricsAccumulator (std::ranges)
// ==========================================

void MetricsAccumulator::AccumulateNextFunctionResults(const std::vector<::analyzer::metric::MetricResult> &metric_results) const {
    std::ranges::for_each(metric_results, [this](const auto &metric_result) {
        auto it = accumulators.find(metric_result.metric_name);
        if (it != accumulators.end() && it->second != nullptr) {
            it->second->Accumulate(metric_result);
        }
    });
}

void MetricsAccumulator::ResetAccumulators() {
    auto active_accumulators = accumulators 
                             | std::views::values 
                             | std::views::filter([](const auto &acc) { return acc != nullptr; });

    std::ranges::for_each(active_accumulators, [](auto &acc) {
        acc->Reset();
    });
}

// ==========================================
// Реализация Accumulator'ов
// ==========================================
namespace metric_accumulator_impl {


void AverageAccumulator::Accumulate(const metric::MetricResult &metric_result) {
    sum += metric_result.value;
    count++;
}
void AverageAccumulator::Finalize() {
    average = (count ? static_cast<double>(sum) / count : 0);
    is_finalized = true;
}

void AverageAccumulator::Reset() {
    is_finalized = false;
    sum = 0;
    count = 0;
    average = 0;
}

double AverageAccumulator::Get() const {
    if (!is_finalized)
        throw std::runtime_error("AverageAccumulator::Get() called before Finalize()");
    return average;
}



void SumAverageAccumulator::Accumulate(const ::analyzer::metric::MetricResult &metric_result) {
    sum += metric_result.value;
    count++;
}

void SumAverageAccumulator::Finalize() {
    if (count > 0) {
        average = static_cast<double>(sum) / count;
    } else {
        average = 0.0;
    }
    is_finalized = true;
}

void SumAverageAccumulator::Reset() {
    sum = 0;
    count = 0;
    average = 0.0;
    is_finalized = false;
}

SumAverage SumAverageAccumulator::Get() const {
    return {sum, average};
}

// ==========================================
// Реализация методов CategoricalAccumulator
// ==========================================

void CategoricalAccumulator::Accumulate(const ::analyzer::metric::MetricResult &metric_result) {
    category_counts[metric_result.value]++;
}

void CategoricalAccumulator::Finalize() {
    if (category_counts.empty()) {
        dominant_category = "none";
        is_finalized = true;
        return;
    }

    auto max_it = std::ranges::max_element(category_counts, [](const auto &p1, const auto &p2) {
        return p1.second < p2.second;
    });

    dominant_category = "category_" + std::to_string(max_it->first);
    is_finalized = true;
}

void CategoricalAccumulator::Reset() {
    category_counts.clear();
    dominant_category = "none";
    is_finalized = false;
}

std::string CategoricalAccumulator::Get() const {
    return dominant_category;
}

} // namespace metric_accumulator_impl
} // namespace analyzer::metric_accumulator
