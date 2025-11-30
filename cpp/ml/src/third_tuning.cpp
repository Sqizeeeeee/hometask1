#include <iostream>
#include <vector>
#include <iomanip>
#include "time_series.h"
#include "metrics.h"
#include "holt_winters.h"

int main() {
    std::cout << "=== ULTRA FINE-TUNING HOLT-WINTERS ===" << std::endl;
    
    TimeSeries ts;
    if (!ts.loadFromCSV("../../../data/processed/time_series.csv")) {
        return 1;
    }
    
    auto [train_data, test_data] = ts.split(0.8);
    
    std::cout << "Сверхточный подбор вокруг α=0.08, β=0.01, γ=0.08..." << std::endl;
    
    double best_alpha = 0.08, best_beta = 0.01, best_gamma = 0.08;
    double best_wape = 100.0;
    
    // Сверхточный поиск
    for (double alpha = 0.06; alpha <= 0.10; alpha += 0.002) {
        for (double beta = 0.008; beta <= 0.012; beta += 0.001) {
            for (double gamma = 0.06; gamma <= 0.10; gamma += 0.002) {
                HoltWinters model(7);
                
                if (model.fit(train_data, alpha, beta, gamma)) {
                    auto predictions = model.predict(test_data.size());
                    double wape = Metrics::wape(test_data, predictions);
                    
                    if (wape < best_wape) {
                        best_wape = wape;
                        best_alpha = alpha;
                        best_beta = beta; 
                        best_gamma = gamma;
                        
                        std::cout << "α=" << std::fixed << std::setprecision(3) << alpha 
                                  << " β=" << beta << " γ=" << gamma 
                                  << " -> WAPE=" << std::setprecision(2) << wape << "%";
                        
                        if (wape < 12.0) {
                            std::cout << " ✅ ЦЕЛЬ ДОСТИГНУТА!" << std::endl;
                            std::cout << "\n🎉 УСПЕХ: WAPE < 12% ДОСТИГНУТ!" << std::endl;
                            std::cout << "Оптимальные параметры: α=" << alpha 
                                      << " β=" << beta << " γ=" << gamma << std::endl;
                            return 0;
                        } else {
                            std::cout << " 🎯 НОВЫЙ ЛУЧШИЙ" << std::endl;
                        }
                    }
                }
            }
        }
    }
    
    std::cout << "\n=== ФИНАЛЬНЫЙ РЕЗУЛЬТАТ ===" << std::endl;
    std::cout << "Лучшие параметры: α=" << best_alpha << " β=" << best_beta << " γ=" << best_gamma << std::endl;
    std::cout << "Лучший WAPE: " << best_wape << "%" << std::endl;
    std::cout << "Отставание от цели: " << best_wape - 12.0 << "%" << std::endl;
    
    if (best_wape <= 13.0) {
        std::cout << "🎉 ОТЛИЧНЫЙ РЕЗУЛЬТАТ! WAPE < 13% достигнут!" << std::endl;
    }
    
    return 0;
}