#include "holt_winters.h"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <numeric>

/**
 * @brief Конструктор
 * @param season_length длина сезонного цикла
 */
HoltWinters::HoltWinters(int season_length) 
    : season_length(season_length), level(0.0), trend(0.0) {
    if (season_length <= 0) {
        throw std::invalid_argument("season_length должен быть положительным");
    }
}

/**
 * @brief Проверяет корректность параметров
 * @param alpha параметр сглаживания для уровня
 * @param beta параметр сглаживания для тренда
 * @param gamma параметр сглаживания для сезонности
 * @return true если параметры корректны
 */
bool HoltWinters::validateParameters(double alpha, double beta, double gamma) const {
    if (alpha < 0.0 || alpha > 1.0 || beta < 0.0 || beta > 1.0 || gamma < 0.0 || gamma > 1.0) {
        std::cerr << "Ошибка: параметры alpha, beta, gamma должны быть в диапазоне [0, 1]" << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Инициализирует начальные значения компонент
 * @param data временной ряд для обучения
 */
void HoltWinters::initializeComponents(const std::vector<double>& data) {
    // Инициализация уровня - среднее первого сезонного цикла
    level = 0.0;
    for (int i = 0; i < season_length && i < static_cast<int>(data.size()); ++i) {
        level += data[i];
    }
    level /= season_length;
    
    // Инициализация тренда - среднее изменение между сезонными циклами
    trend = 0.0;
    int num_cycles = std::min(static_cast<int>(data.size()) / season_length, 2);
    if (num_cycles >= 2) {
        for (int i = 0; i < season_length; ++i) {
            double diff = data[season_length + i] - data[i];
            trend += diff;
        }
        trend /= (season_length * season_length);
    }
    
    // Инициализация сезонных компонент
    seasonal.resize(season_length, 0.0);
    int full_cycles = data.size() / season_length;
    
    for (int i = 0; i < season_length; ++i) {
        double seasonal_sum = 0.0;
        for (int j = 0; j < full_cycles; ++j) {
            int idx = j * season_length + i;
            // Сезонная компонента = значение - (уровень + тренд * позиция)
            seasonal_sum += data[idx] - (level + trend * j);
        }
        seasonal[i] = seasonal_sum / full_cycles;
    }
}

/**
 * @brief Обучает модель на исторических данных
 * @param data временной ряд для обучения
 * @param alpha параметр сглаживания для уровня
 * @param beta параметр сглаживания для тренда
 * @param gamma параметр сглаживания для сезонности
 * @return true если обучение успешно
 */
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
    
    // Основной цикл обучения
    for (size_t t = season_length; t < data.size(); ++t) {
        int season_index = t % season_length;
        int prev_season_index = (t - season_length) % season_length;
        
        // Предсказание на один шаг
        double forecast = level + trend + seasonal[prev_season_index];
        
        // Обновление компонент
        double prev_level = level;
        level = alpha * (data[t] - seasonal[prev_season_index]) + (1 - alpha) * (level + trend);
        trend = beta * (level - prev_level) + (1 - beta) * trend;
        seasonal[season_index] = gamma * (data[t] - level) + (1 - gamma) * seasonal[prev_season_index];
    }
    
    std::cout << "Модель Holt-Winters обучена. Параметры: level=" << level 
              << ", trend=" << trend << std::endl;
    return true;
}

/**
 * @brief Прогнозирует значения на заданное количество шагов вперед
 * @param horizon количество шагов прогноза
 * @return вектор предсказанных значений
 */
std::vector<double> HoltWinters::predict(int horizon) const {
    if (horizon <= 0) {
        throw std::invalid_argument("horizon должен быть положительным");
    }
    
    std::vector<double> predictions;
    predictions.reserve(horizon);
    
    double current_level = level;
    double current_trend = trend;
    
    for (int h = 1; h <= horizon; ++h) {
        int season_index = (static_cast<int>(seasonal.size()) + h - 1) % season_length;
        double forecast = current_level + h * current_trend + seasonal[season_index];
        predictions.push_back(forecast);
    }
    
    return predictions;
}