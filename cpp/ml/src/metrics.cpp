#include "metrics.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

/**
 * @brief Проверяет что векторы одинакового размера
 * @param actual вектор фактических значений
 * @param predicted вектор предсказанных значений
 */
void Metrics::validateInputs(const std::vector<double>& actual,
                           const std::vector<double>& predicted) {
    if (actual.size() != predicted.size()) {
        throw std::invalid_argument("Векторы actual и predicted должны быть одинакового размера");
    }
    if (actual.empty()) {
        throw std::invalid_argument("Векторы не должны быть пустыми");
    }
}

/**
 * @brief Вычисляет WAPE (Weighted Absolute Percentage Error)
 * @param actual вектор фактических значений
 * @param predicted вектор предсказанных значений
 * @return WAPE в процентах
 */
double Metrics::wape(const std::vector<double>& actual, 
                    const std::vector<double>& predicted) {
    validateInputs(actual, predicted);
    
    double sum_abs_error = 0.0;
    double sum_actual = 0.0;
    
    for (size_t i = 0; i < actual.size(); ++i) {
        sum_abs_error += std::abs(actual[i] - predicted[i]);
        sum_actual += std::abs(actual[i]);
    }
    
    if (sum_actual == 0.0) {
        throw std::runtime_error("Сумма фактических значений равна 0, WAPE не может быть вычислен");
    }
    
    return (sum_abs_error / sum_actual) * 100.0;
}

/**
 * @brief Вычисляет MAE (Mean Absolute Error)
 * @param actual вектор фактических значений
 * @param predicted вектор предсказанных значений
 * @return MAE
 */
double Metrics::mae(const std::vector<double>& actual, 
                   const std::vector<double>& predicted) {
    validateInputs(actual, predicted);
    
    double sum_abs_error = 0.0;
    for (size_t i = 0; i < actual.size(); ++i) {
        sum_abs_error += std::abs(actual[i] - predicted[i]);
    }
    
    return sum_abs_error / actual.size();
}

/**
 * @brief Вычисляет RMSE (Root Mean Square Error)
 * @param actual вектор фактических значений
 * @param predicted вектор предсказанных значений
 * @return RMSE
 */
double Metrics::rmse(const std::vector<double>& actual, 
                    const std::vector<double>& predicted) {
    validateInputs(actual, predicted);
    
    double sum_squared_error = 0.0;
    for (size_t i = 0; i < actual.size(); ++i) {
        double error = actual[i] - predicted[i];
        sum_squared_error += error * error;
    }
    
    return std::sqrt(sum_squared_error / actual.size());
}