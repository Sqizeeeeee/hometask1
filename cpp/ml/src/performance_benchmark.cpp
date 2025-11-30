/**
 * @brief Бенчмарк для анализа производительности Holt-Winters
 */
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>
#include "time_series.h"
#include "holt_winters.h"
#include "metrics.h"

/**
 * @brief Замеряет время выполнения алгоритма на данных разного размера
 */
void benchmark_time_complexity() {
    std::cout << "=== БЕНЧМАРК ВРЕМЕННОЙ СЛОЖНОСТИ ===" << std::endl;
    
    TimeSeries ts;
    if (!ts.loadFromCSV("../../../data/processed/time_series.csv")) {
        return;
    }
    
    auto full_data = ts.getValues();
    std::vector<int> data_sizes = {100, 200, 400, 600, 730};
    std::vector<double> training_times;
    std::vector<double> prediction_times;
    
    std::cout << std::setw(10) << "Размер" 
              << std::setw(15) << "Время обучения" 
              << std::setw(15) << "Время прогноза" 
              << std::setw(15) << "WAPE" << std::endl;
    std::cout << std::string(55, '-') << std::endl;
    
    for (int size : data_sizes) {
        if (size > full_data.size()) continue;
        
        std::vector<double> sample_data(full_data.begin(), full_data.begin() + size);
        int train_size = static_cast<int>(size * 0.7);
        int test_size = size - train_size;
        
        std::vector<double> train_data(sample_data.begin(), sample_data.begin() + train_size);
        std::vector<double> test_data(sample_data.begin() + train_size, sample_data.end());
        
        HoltWinters model(7);
        
        // Замер времени обучения
        auto start_train = std::chrono::high_resolution_clock::now();
        model.fit(train_data, 0.07, 0.01, 0.07);
        auto end_train = std::chrono::high_resolution_clock::now();
        double train_time = std::chrono::duration<double, std::milli>(end_train - start_train).count();
        
        // Замер времени прогноза
        auto start_pred = std::chrono::high_resolution_clock::now();
        auto predictions = model.predict(test_size);
        auto end_pred = std::chrono::high_resolution_clock::now();
        double pred_time = std::chrono::duration<double, std::milli>(end_pred - start_pred).count();
        
        double wape = Metrics::wape(test_data, predictions);
        
        training_times.push_back(train_time);
        prediction_times.push_back(pred_time);
        
        std::cout << std::setw(10) << size 
                  << std::setw(15) << std::fixed << std::setprecision(2) << train_time << " мс"
                  << std::setw(15) << pred_time << " мс"
                  << std::setw(15) << std::setprecision(1) << wape << "%" << std::endl;
    }
    
    // Сохраняем результаты в JSON
    std::ofstream json_file("../../../results/time_complexity.json");
    json_file << "{\n";
    json_file << "  \"time_complexity\": {\n";
    json_file << "    \"data_sizes\": [";
    for (size_t i = 0; i < data_sizes.size(); ++i) {
        json_file << data_sizes[i] << (i < data_sizes.size() - 1 ? ", " : "");
    }
    json_file << "],\n";
    json_file << "    \"training_times_ms\": [";
    for (size_t i = 0; i < training_times.size(); ++i) {
        json_file << training_times[i] << (i < training_times.size() - 1 ? ", " : "");
    }
    json_file << "],\n";
    json_file << "    \"prediction_times_ms\": [";
    for (size_t i = 0; i < prediction_times.size(); ++i) {
        json_file << prediction_times[i] << (i < prediction_times.size() - 1 ? ", " : "");
    }
    json_file << "]\n";
    json_file << "  }\n";
    json_file << "}\n";
    json_file.close();
}

/**
 * @brief Анализирует использование памяти (теоретически)
 */
void analyze_memory_complexity() {
    std::cout << "\n=== АНАЛИЗ ИСПОЛЬЗОВАНИЯ ПАМЯТИ ===" << std::endl;
    
    std::vector<int> data_sizes = {100, 200, 400, 600, 730};
    std::vector<double> memory_kb;
    
    std::cout << std::setw(10) << "Размер" 
              << std::setw(20) << "Память (теор.)" 
              << std::setw(20) << "Сложность" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (int size : data_sizes) {
        // Теоретическая оценка: O(n) для данных + O(m) для сезонных компонент
        double memory_bytes = size * sizeof(double) +  // данные
                             7 * sizeof(double) +      // сезонные компоненты
                             2 * sizeof(double);       // уровень и тренд
        
        memory_kb.push_back(memory_bytes / 1024.0);
        
        std::cout << std::setw(10) << size 
                  << std::setw(20) << std::fixed << std::setprecision(2) << memory_kb.back() << " КБ"
                  << std::setw(20) << "O(n)" << std::endl;
    }
    
    // Сохраняем результаты
    std::ofstream json_file("../../../results/memory_complexity.json");
    json_file << "{\n";
    json_file << "  \"memory_complexity\": {\n";
    json_file << "    \"data_sizes\": [";
    for (size_t i = 0; i < data_sizes.size(); ++i) {
        json_file << data_sizes[i] << (i < data_sizes.size() - 1 ? ", " : "");
    }
    json_file << "],\n";
    json_file << "    \"memory_kb\": [";
    for (size_t i = 0; i < memory_kb.size(); ++i) {
        json_file << memory_kb[i] << (i < memory_kb.size() - 1 ? ", " : "");
    }
    json_file << "]\n";
    json_file << "  }\n";
    json_file << "}\n";
    json_file.close();
}

int main() {
    std::cout << "ПРОИЗВОДИТЕЛЬНОСТЬ HOLT-WINTERS АЛГОРИТМА\n" << std::endl;
    
    benchmark_time_complexity();
    analyze_memory_complexity();
    
    std::cout << "\n=== РЕЗУЛЬТАТЫ СОХРАНЕНЫ ===" << std::endl;
    std::cout << "Файлы созданы в results/" << std::endl;
    
    return 0;
}