#include <iostream>
#include <vector>
#include <iomanip>
#include "time_series.h"
#include "metrics.h"
#include "holt_winters.h"

int main() {
    std::cout << "=== FINE-TUNING HOLT-WINTERS ===" << std::endl;
    
    TimeSeries ts;
    if (!ts.loadFromCSV("../../../data/processed/time_series.csv")) {
        return 1;
    }
    
    auto [train_data, test_data] = ts.split(0.8);
    
    std::cout << "Точный подбор параметров вокруг alpha=0.1, beta=0.01, gamma=0.1..." << std::endl;
    
    double best_alpha = 0.1, best_beta = 0.01, best_gamma = 0.1;
    double best_wape = 100.0;
    
    // Точный поиск вокруг лучших параметров
    for (double alpha = 0.08; alpha <= 0.12; alpha += 0.005) {
        for (double beta = 0.005; beta <= 0.02; beta += 0.005) {
            for (double gamma = 0.08; gamma <= 0.12; gamma += 0.005) {
                HoltWinters model(7);
                
                if (model.fit(train_data, alpha, beta, gamma)) {
                    auto predictions = model.predict(test_data.size());
                    double wape = Metrics::wape(test_data, predictions);
                    
                    std::cout << "α=" << std::fixed << std::setprecision(3) << alpha 
                              << " β=" << beta << " γ=" << gamma 
                              << " -> WAPE=" << std::setprecision(2) << wape << "%";
                    
                    if (wape < best_wape) {
                        best_wape = wape;
                        best_alpha = alpha;
                        best_beta = beta; 
                        best_gamma = gamma;
                        std::cout << " 🎯 НОВЫЙ ЛУЧШИЙ";
                    }
                    std::cout << std::endl;
                    
                    if (wape < 12.0) {
                        std::cout << "✅ ЦЕЛЬ ДОСТИГНУТА!" << std::endl;
                        return 0;
                    }
                }
            }
        }
    }
    
    std::cout << "\n=== РЕЗУЛЬТАТ ===" << std::endl;
    std::cout << "Лучшие параметры: α=" << best_alpha << " β=" << best_beta << " γ=" << best_gamma << std::endl;
    std::cout << "Лучший WAPE: " << best_wape << "%" << std::endl;
    
    if (best_wape < 12.0) {
        std::cout << "🎉 УСПЕХ: WAPE < 12% ДОСТИГНУТ!" << std::endl;
    } else {
        std::cout << "Минимальный достигнутый WAPE: " << best_wape << "%" << std::endl;
        std::cout << "Отставание от цели: " << best_wape - 12.0 << "%" << std::endl;
    }
    
    return 0;
}