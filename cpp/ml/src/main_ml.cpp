/**
 * @brief Основная программа для тестирования алгоритма Holt-Winters
 * 
 * Загружает данные, обучает модель и оценивает качество прогноза
 */
#include <iostream>
#include <vector>
#include "time_series.h"
#include "holt_winters.h"
#include "metrics.h"

/**
 * @brief Основная функция
 * @return код завершения программы
 */
int main() {
    std::cout << "=== ТЕСТИРОВАНИЕ HOLT-WINTERS ===" << std::endl;
    
    // 1. Загрузка данных
    TimeSeries ts;
    std::string data_file = "../../../data/processed/time_series.csv";
    
    std::cout << "Загрузка данных из: " << data_file << std::endl;
    if (!ts.loadFromCSV(data_file)) {
        std::cerr << "Не удалось загрузить данные!" << std::endl;
        return 1;
    }
    
    // 2. Разделение на обучающую и тестовую выборки
    std::cout << "Разделение данных (80% train, 20% test)..." << std::endl;
    auto [train_data, test_data] = ts.split(0.8);
    
    std::cout << "Обучающая выборка: " << train_data.size() << " точек" << std::endl;
    std::cout << "Тестовая выборка: " << test_data.size() << " точек" << std::endl;
    
    // 3. Обучение модели Holt-Winters
    std::cout << "\nОбучение модели Holt-Winters..." << std::endl;
    HoltWinters model(7); // недельная сезонность
    
    if (!model.fit(train_data, 0.3, 0.1, 0.1)) {
        std::cerr << "Ошибка обучения модели!" << std::endl;
        return 1;
    }
    
    // 4. Прогнозирование
    std::cout << "\nПрогнозирование на " << test_data.size() << " шагов..." << std::endl;
    std::vector<double> predictions = model.predict(test_data.size());
    
    // 5. Оценка качества
    std::cout << "\n=== РЕЗУЛЬТАТЫ ===" << std::endl;
    
    double wape_value = Metrics::wape(test_data, predictions);
    double mae_value = Metrics::mae(test_data, predictions);
    double rmse_value = Metrics::rmse(test_data, predictions);
    
    std::cout << "WAPE: " << wape_value << "%" << std::endl;
    std::cout << "MAE: " << mae_value << std::endl;
    std::cout << "RMSE: " << rmse_value << std::endl;
    
    // 6. Проверка критерия успеха
    std::cout << "\n=== КРИТЕРИЙ УСПЕХА ===" << std::endl;
    if (wape_value < 12.0) {
        std::cout << "✅ УСПЕХ: WAPE < 12% (" << wape_value << "%)" << std::endl;
    } else {
        std::cout << "❌ НЕУДАЧА: WAPE >= 12% (" << wape_value << "%)" << std::endl;
    }
    
    // 7. Пример прогноза
    std::cout << "\n=== ПРИМЕР ПРОГНОЗА ===" << std::endl;
    std::cout << "Первые 5 фактических значений: ";
    for (int i = 0; i < 5 && i < test_data.size(); ++i) {
        std::cout << test_data[i] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Первые 5 предсказанных значений: ";
    for (int i = 0; i < 5 && i < predictions.size(); ++i) {
        std::cout << predictions[i] << " ";
    }
    std::cout << std::endl;
    
    return 0;
}