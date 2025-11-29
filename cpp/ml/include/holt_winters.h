#ifndef HOLT_WINTERS_H
#define HOLT_WINTERS_H

#include <vector>

/**
 * @brief Класс для тройного экспоненциального сглаживания (Holt-Winters)
 * 
 * Реализует аддитивную модель для временных рядов с трендом и сезонностью
 */
class HoltWinters {
public:
    /**
     * @brief Конструктор
     * @param season_length длина сезонного цикла (например, 7 для недельной сезонности)
     */
    explicit HoltWinters(int season_length = 7);
    
    /**
     * @brief Обучает модель на исторических данных
     * @param data временной ряд для обучения
     * @param alpha параметр сглаживания для уровня (0-1)
     * @param beta параметр сглаживания для тренда (0-1) 
     * @param gamma параметр сглаживания для сезонности (0-1)
     * @return true если обучение успешно
     */
    bool fit(const std::vector<double>& data,
             double alpha = 0.3, double beta = 0.1, double gamma = 0.1);
    
    /**
     * @brief Прогнозирует значения на заданное количество шагов вперед
     * @param horizon количество шагов прогноза
     * @return вектор предсказанных значений
     */
    std::vector<double> predict(int horizon) const;
    
    /**
     * @brief Возвращает последнее значение уровня
     */
    double getLevel() const { return level; }
    
    /**
     * @brief Возвращает последнее значение тренда
     */
    double getTrend() const { return trend; }
    
    /**
     * @brief Возвращает сезонные компоненты
     */
    const std::vector<double>& getSeasonal() const { return seasonal; }

private:
    int season_length;           ///< Длина сезонного цикла
    double level;               ///< Текущий уровень
    double trend;               ///< Текущий тренд
    std::vector<double> seasonal; ///< Сезонные компоненты
    
    /**
     * @brief Инициализирует начальные значения компонент
     */
    void initializeComponents(const std::vector<double>& data);
    
    /**
     * @brief Проверяет корректность параметров
     */
    bool validateParameters(double alpha, double beta, double gamma) const;
};

#endif // HOLT_WINTERS_H