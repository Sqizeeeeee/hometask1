#ifndef TIME_SERIES_H
#define TIME_SERIES_H

#include <vector>
#include <string>
#include <utility>

/**
 * @brief Класс для работы с временными рядами
 * 
 * Загружает данные из CSV файла и предоставляет доступ к ним
 */
class TimeSeries {
public:
    /**
     * @brief Загружает временной ряд из CSV файла
     * @param filename путь к CSV файлу
     * @param date_col индекс колонки с датами (по умолчанию 0)
     * @param value_col индекс колонки со значениями (по умолчанию 1)
     * @return true если загрузка успешна, false в случае ошибки
     */
    bool loadFromCSV(const std::string& filename, 
                    int date_col = 0, 
                    int value_col = 1);
    
    /**
     * @brief Возвращает значения временного ряда
     */
    const std::vector<double>& getValues() const { return values; }
    
    /**
     * @brief Возвращает размер временного ряда
     */
    size_t size() const { return values.size(); }
    
    /**
     * @brief Возвращает значение по индексу
     */
    double operator[](size_t index) const { return values[index]; }
    
    /**
     * @brief Разделяет ряд на обучающую и тестовую выборки
     * @param train_ratio доля обучающей выборки (0.0 - 1.0)
     * @return пара: обучающая выборка, тестовая выборка
     */
    std::pair<std::vector<double>, std::vector<double>> 
    split(double train_ratio) const;

private:
    std::vector<double> values;
};

#endif // TIME_SERIES_H