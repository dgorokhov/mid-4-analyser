#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ranges>

// 💡 Опережающее объявление: строго говорим компилятору, что такой тип существует
namespace analyzer::metric {
    struct MetricResult; 
}

namespace analyzer::metric_accumulator {

// ==========================================
// 1. БАЗОВЫЙ ИНТЕРФЕЙС
// ==========================================
struct IAccumulator {
    // Используем полный путь к типу
    virtual void Accumulate(const ::analyzer::metric::MetricResult &metric_result) = 0;
    virtual void Finalize() = 0;
    virtual void Reset() = 0;
    virtual ~IAccumulator() = default;

protected:
    bool is_finalized = false;
};

// ==========================================
// 2. РЕАЛИЗАЦИИ АККУМУЛЯТОРОВ
// ==========================================
namespace metric_accumulator_impl {

struct AverageAccumulator : public IAccumulator {
    void Accumulate(const metric::MetricResult &metric_result) override;
    void Finalize() override;
    void Reset();
    double Get() const;

private:
    int sum = 0;
    int count = 0;
    double average = 0;
};


struct SumAverage {
        int sum;
        double average;
        auto operator<=>(const SumAverage &) const = default;
    };
    
struct SumAverageAccumulator : public IAccumulator {
    
    void Accumulate(const metric::MetricResult &metric_result) override;
    virtual void Finalize() override;
    virtual void Reset() override;
    SumAverage Get() const;

private:
    int sum = 0;
    int count = 0;
    double average = 0;
};

class CategoricalAccumulator : public IAccumulator {
public:
    void Accumulate(const ::analyzer::metric::MetricResult &metric_result) override;
    void Finalize() override;
    void Reset() override;
    std::string Get() const;

private:
    std::unordered_map<int, int> category_counts;
    std::string dominant_category = "none";
};

} // namespace metric_accumulator_impl

// ==========================================
// 3. ОСНОВНОЙ ДИСПЕТЧЕР АККУМУЛЯТОРОВ
// ==========================================
struct MetricsAccumulator {
    template <typename Accumulator>
    void RegisterAccumulator(const std::string &metric_name, std::unique_ptr<Accumulator> acc) {
        accumulators.emplace(metric_name, std::move(acc));
    }

    template <typename Accumulator>
    const Accumulator &GetFinalizedAccumulator(const std::string &metric_name) const {
        auto metric_accumulator = accumulators.at(metric_name);
        metric_accumulator->Finalize();
        return dynamic_cast<const Accumulator&>(*metric_accumulator);
    }

    // Передаем вектор с полным именем типа
    void AccumulateNextFunctionResults(const std::vector<::analyzer::metric::MetricResult> &metric_results) const;
    void ResetAccumulators();

private:
    std::unordered_map<std::string, std::shared_ptr<IAccumulator>> accumulators;
};

}  // namespace analyzer::metric_accumulator
