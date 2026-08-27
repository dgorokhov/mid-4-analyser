#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

//#include "metric_accumulator.hpp"

namespace analyzer {

namespace rv = std::ranges::views;
namespace rs = std::ranges;
/**
 * @brief Анализирует список Python-файлов и извлекает метрики для всех функций и методов.
 *
 * Эта функция — центральный "конвейер" обработки:
 * 1. Принимает имена файлов.
 * 2. Для каждого файла создаёт объект `File`, который автоматически парсит его через tree-sitter
 *    и строит AST.
 * 3. Извлекает из AST все функции и методы с помощью `FunctionExtractor`.
 * 4. Объединяет все функции из всех файлов в один плоский список (`join`).
 * 5. Для каждой функции вычисляет набор метрик через переданный `metric_extractor`.
 * 6. Возвращает вектор пар: (функция, результаты её метрик).
 */

/*
inline auto AnalyseFunctions(const std::vector<std::string> &files,
                             const analyzer::metric::MetricExtractor &metric_extractor) {
    // 1. Создаем общий список для функций и экстрактор
    std::vector<analyzer::function::Function> all_functions;
    analyzer::function::FunctionExtractor func_extractor;

    for (const auto &filename : files) {
        try {
            // Создаем структуру File (запускает парсинг tree-sitter внутри)
            analyzer::file::File source_file(filename);

            // Выделяем функции из этого файла
            std::vector<analyzer::function::Function> file_functions = func_extractor.Get(source_file);

            // Быстро перемещаем их в общий пул без лишнего копирования строк
            all_functions.insert(all_functions.end(), std::make_move_iterator(file_functions.begin()),
                                 std::make_move_iterator(file_functions.end()));
        } catch (const std::exception &e) {
            // Пропускаем проблемный файл, выводя предупреждение
            std::cerr << "Warning: Skipping file " << filename << " due to error: " << e.what() << std::endl;
        }
    }

    // 2. Описываем итоговый контейнер. Компилятор выведет его тип для возврата.
    std::vector<std::pair<analyzer::function::Function, analyzer::metric::MetricResults>> analysis_results;
    analysis_results.reserve(all_functions.size());

    for (auto &func : all_functions) {
        // Считам набоор метрик для ф-ии
        analyzer::metric::MetricResults metrics = metric_extractor.Get(func);
        // Сохраняем пару [Функция, метрики]
        analysis_results.emplace_back(std::move(func), std::move(metrics));
    }

    return analysis_results;
}
*/

inline auto AnalyseFunctions(const std::vector<std::string> &files,
                             const analyzer::metric::MetricExtractor &metric_extractor) {
    analyzer::function::FunctionExtractor func_extractor;

    auto pipeline = files 
        | std::views::transform([&](const std::string &filename) {
            try {
                analyzer::file::File source_file(filename);
                return func_extractor.Get(source_file);
            } catch (const std::exception &e) {
                std::cerr << "Warning: Skipping file " << filename << " due to error: " << e.what() << std::endl;
                return std::vector<analyzer::function::Function>{};
            }
        })
        | std::views::join
        | std::views::transform([&](analyzer::function::Function& func) {
            auto metrics = metric_extractor.Get(func);
            return std::make_pair(std::move(func), std::move(metrics));
        });

    std::vector<std::pair<anal
    yzer::function::Function, analyzer::metric::MetricResults>> analysis_results;
    std::ranges::move(pipeline, std::back_inserter(analysis_results));

    return analysis_results;
}


/**
 *
 * @brief Группирует результаты анализа по классам.
 *
 * Эта функция:
 * 1. Отфильтровывает только те функции, которые являются **методами классов**
 *    (у них `class_name.has_value()` == true).
 * 2. Группирует последовательные элементы с одинаковым именем класса с помощью `chunk_by`.
 *
 * Важно:
 * - `chunk_by` работает только с **последовательными** одинаковыми элементами!
 *   Поэтому предполагается, что входной диапазон уже упорядочен по классам
 *   (например, порядок методов в AST сохраняется как в исходном файле).
 * - Если порядок нарушен, один и тот же класс может быть разбит на несколько групп.
 *
 *  Чтобы убедиться, что фильтрация работает, проверьте, что свободные функции (без class_name)
 * действительно исчезают из результата.
 */
inline auto SplitByClasses(const auto &analysis) {
    // Ключ — имя класса, значение — список пар (функция, её метрики)
    std::unordered_map<std::string, std::vector<std::decay_t<decltype(analysis[0])>>> class_groups;

    for (const auto &item : analysis) {
        // item.first — это Function. Проверяем, принадлежит ли она классу
        if (item.first.class_name.has_value()) {
            std::string class_name = item.first.class_name.value();
            class_groups[class_name].push_back(item);
        }
    }
    return class_groups;
}
/**
 * @brief Группирует результаты анализа по исходным файлам.
 *
 * Эта функция:
 * - Разбивает весь список функций на группы, где каждая группа содержит
 *   только функции из одного и того же файла (`filename`).
 * - Использует `chunk_by`, поэтому **порядок функций в `analysis` должен быть по файлам**.
 */
inline auto SplitByFiles(const auto &analysis) {
    // Ключ — имя файла, значение — список пар (функция, её метрики)
    std::unordered_map<std::string, std::vector<std::decay_t<decltype(analysis[0])>>> file_groups;

    for (const auto &item : analysis) {
        std::string filename = item.first.filename;
        file_groups[filename].push_back(item);
    }
    return file_groups;
}

/**
 * @brief Агрегирует метрики всех функций с помощью аккумулятора.
 *
 * Эта функция:
 * - Проходит по каждому элементу результата `AnalyseFunctions`
 *   (то есть по каждой функции и её метрикам).
 * - Передаёт результаты метрик (`elem.second`) в аккумулятор через `AccumulateNextFunctionResults`.
 */


template <typename T, typename U>
inline void AccumulateFunctionAnalysis(const T &analysis_container, U &metrics_accumulator) {
    for (const auto &item : analysis_container) {
        // item.second — это и есть std::vector<metric::MetricResult> для конкретной функции
        const auto &metrics_results = item.second;

        // Передаем весь вектор результатов функции в метод аккумулятора
        metrics_accumulator.AccumulateNextFunctionResults(metrics_results);
    }
}
}  // namespace analyzer
