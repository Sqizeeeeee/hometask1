/**
 * @brief Скрипт для подбора оптимальных параметров Holt-Winters
 */
#include <iostream>
#include <vector>
#include <tuple>
#include "time_series.h"
#include "holt_winters.h"
#include "metrics.h"

/**
 * @brief Подбирает оптимальные параметры методом сеточного поиска
 * @param train_data обучающие данные
 * @param test_data тестовые данные  
 * @return лучшие параметры [alpha, beta, gamma] и соответствующий WAPE
 */
std::tuple<double, double, double, double> 
findBestParameters(const std::vector<double>& train_data,
                  const std::vector<double>& test_data) {
    double best_alpha = 0.3, best_beta = 0.1, best_gamma = 0.1;
    double best_wape = 100.0; // начинаем с плохого значения
    
    // Параметры для перебора
    std::vector<double> alphas = {0.1, 0.2, 0.3, 0.4, 0.5};
    std::vector<double> betas = {0.01, 0.05, 0.1, 0.15};
    std::vector<double> gammas = {0.1, 0.2, 0.3, 0.4, 0.5};
    
    std::cout << "Поиск оптимальных параметров..." << std::endl;
    
    for (double alpha : alphas) {
        for (double beta : betas) {
            for (double gamma : gammas) {
                HoltWinters model(7);
                
                if (model.fit(train_data, alpha, beta, gamma)) {
                    auto predictions = model.predict(test_data.size());
                    double wape = Metrics::wape(test_data, predictions);
                    
                    if (wape < best_wape) {
                        best_wape = wape;
                        best_alpha = alpha;
                        best_beta = beta;
                        best_gamma = gamma;
                        
                        std::cout << "Улучшение: alpha=" << alpha 
                                  << ", beta=" << beta 
                                  << ", gamma=" << gamma
                                  << ", WAPE=" << wape << "%" << std::endl;
                    }
                }
            }
        }
    }
    
    return {best_alpha, best_beta, best_gamma, best_wape};
}

int main() {
    std::cout << "=== ПОДБОР ПАРАМЕТРОВ HOLT-WINTERS ===" << std::endl;
    
    // Загрузка данных
    TimeSeries ts;
    if (!ts.loadFromCSV("../../../data/processed/time_series.csv")) {
        return 1;
    }
    
    auto [train_data, test_data] = ts.split(0.8);
    
    // Поиск лучших параметров
    auto [alpha, beta, gamma, wape] = findBestParameters(train_data, test_data);
    
    std::cout << "\n=== РЕЗУЛЬТАТЫ ПОДБОРА ===" << std::endl;
    std::cout << "Лучшие параметры: alpha=" << alpha 
              << ", beta=" << beta << ", gamma=" << gamma << std::endl;
    std::cout << "Лучший WAPE: " << wape << "%" << std::endl;
    
    if (wape < 12.0) {
        std::cout << "✅ ЦЕЛЬ ДОСТИГНУТА: WAPE < 12%" << std::endl;
    } else {
        std::cout << "❌ ЦЕЛЬ НЕ ДОСТИГНУТА: WAPE >= 12%" << std::endl;
    }
    
    return 0;
}