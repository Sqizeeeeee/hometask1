#include "holt_winters.h"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <numeric>
#include <algorithm>

HoltWinters::HoltWinters(int season_length) 
    : season_length(season_length), level(0.0), trend(0.0) {
    if (season_length <= 0) {
        throw std::invalid_argument("season_length должен быть положительным");
    }
}

bool HoltWinters::validateParameters(double alpha, double beta, double gamma) const {
    if (alpha < 0.0 || alpha > 1.0 || beta < 0.0 || beta > 1.0 || gamma < 0.0 || gamma > 1.0) {
        std::cerr << "Ошибка: параметры alpha, beta, gamma должны быть в диапазоне [0, 1]" << std::endl;
        return false;
    }
    return true;
}

void HoltWinters::initializeComponents(const std::vector<double>& data) {
    // Простая и стабильная инициализация
    int n = data.size();
    
    // Уровень - среднее всех данных
    level = std::accumulate(data.begin(), data.end(), 0.0) / n;
    
    // Тренд - простой линейный тренд
    trend = 0.0;
    if (n > 1) {
        double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_xx = 0.0;
        for (int i = 0; i < n; ++i) {
            sum_x += i;
            sum_y += data[i];
            sum_xy += i * data[i];
            sum_xx += i * i;
        }
        trend = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x);
    }
    
    // Сезонность - отклонения от тренда
    seasonal.resize(season_length, 0.0);
    std::vector<int> counts(season_length, 0);
    
    for (int i = 0; i < n; ++i) {
        int season_idx = i % season_length;
        double expected = level + trend * i;
        seasonal[season_idx] += data[i] - expected;
        counts[season_idx]++;
    }
    
    // Нормализация сезонности
    for (int i = 0; i < season_length; ++i) {
        if (counts[i] > 0) {
            seasonal[i] /= counts[i];
        }
    }
    
    // Центрирование сезонности (сумма = 0)
    double seasonal_sum = std::accumulate(seasonal.begin(), seasonal.end(), 0.0);
    double seasonal_mean = seasonal_sum / season_length;
    for (int i = 0; i < season_length; ++i) {
        seasonal[i] -= seasonal_mean;
    }
}

bool HoltWinters::fit(const std::vector<double>& data,
                     double alpha, double beta, double gamma) {
    if (data.size() < 2 * season_length) {
        std::cerr << "Ошибка: недостаточно данных для обучения. Нужно минимум " 
                  << 2 * season_length << " точек" << std::endl;
        return false;
    }
    
    if (!validateParameters(alpha, beta, gamma)) {
        return false;
    }
    
    // Инициализация компонент
    initializeComponents(data);
    
    // Сохраняем начальные значения для контроля
    double initial_level = level;
    double initial_trend = trend;
    
    // Основной цикл обучения (начинаем с season_length)
    for (size_t t = season_length; t < data.size(); ++t) {
        int season_idx = t % season_length;
        int prev_season_idx = (t - season_length) % season_length;
        
        // Временно сохраняем предыдущие значения
        double prev_level = level;
        double prev_trend = trend;
        
        // Обновление компонент с защитой от расходимости
        double new_level = alpha * (data[t] - seasonal[prev_season_idx]) 
                         + (1 - alpha) * (level + trend);
        
        double new_trend = beta * (new_level - level) 
                         + (1 - beta) * trend;
        
        double new_seasonal = gamma * (data[t] - new_level) 
                            + (1 - gamma) * seasonal[prev_season_idx];
        
        // Применяем обновления
        level = new_level;
        trend = new_trend;
        seasonal[season_idx] = new_seasonal;
        
        // Защита от расходимости - если значения уходят в отрицательные, сбрасываем
        if (level < 0 || std::abs(level) > 10000) {
            std::cout << "Предупреждение: уровень расходится, сброс к начальным значениям" << std::endl;
            level = initial_level;
            trend = initial_trend;
        }
    }
    
    std::cout << "Модель Holt-Winters обучена. Параметры: level=" << level 
              << ", trend=" << trend << std::endl;
    return true;
}

std::vector<double> HoltWinters::predict(int horizon) const {
    if (horizon <= 0) {
        throw std::invalid_argument("horizon должен быть положительным");
    }
    
    std::vector<double> predictions;
    predictions.reserve(horizon);
    
    for (int h = 1; h <= horizon; ++h) {
        int season_idx = (season_length + h - 1) % season_length;
        double forecast = level + h * trend + seasonal[season_idx];
        // Защита от отрицательных прогнозов
        predictions.push_back(std::max(forecast, 0.0));
    }
    
    return predictions;
}