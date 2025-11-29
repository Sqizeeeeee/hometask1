#ifndef METRICS_H
#define METRICS_H

#include <vector>

/**
 * @brief Класс для расчета метрик качества прогнозирования
 */
class Metrics {
public:
    /**
     * @brief Вычисляет WAPE (Weighted Absolute Percentage Error)
     * @param actual вектор фактических значений
     * @param predicted вектор предсказанных значений
     * @return WAPE в процентах
     */
    static double wape(const std::vector<double>& actual, 
                      const std::vector<double>& predicted);
    
    /**
     * @brief Вычисляет MAE (Mean Absolute Error)
     * @param actual вектор фактических значений
     * @param predicted вектор предсказанных значений
     * @return MAE
     */
    static double mae(const std::vector<double>& actual, 
                     const std::vector<double>& predicted);
    
    /**
     * @brief Вычисляет RMSE (Root Mean Square Error)
     * @param actual вектор фактических значений
     * @param predicted вектор предсказанных значений
     * @return RMSE
     */
    static double rmse(const std::vector<double>& actual, 
                      const std::vector<double>& predicted);

private:
    /**
     * @brief Проверяет что векторы одинакового размера
     */
    static void validateInputs(const std::vector<double>& actual,
                              const std::vector<double>& predicted);
};

#endif // METRICS_H