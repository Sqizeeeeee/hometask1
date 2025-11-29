#include "time_series.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

/**
 * @brief Загружает временной ряд из CSV файла
 * @param filename путь к CSV файлу
 * @param date_col индекс колонки с датами
 * @param value_col индекс колонки со значениями
 * @return true если загрузка успешна, false в случае ошибки
 */
bool TimeSeries::loadFromCSV(const std::string& filename, 
                           int date_col, 
                           int value_col) {
    values.clear();
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        return false;
    }
    
    std::string line;
    bool first_line = true;
    
    while (std::getline(file, line)) {
        // Пропускаем первую строку (заголовок)
        if (first_line) {
            first_line = false;
            continue;
        }
        
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;
        
        // Разбиваем строку на ячейки
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        
        // Проверяем что есть достаточно колонок
        if (row.size() <= static_cast<size_t>(std::max(date_col, value_col))) {
            std::cerr << "Предупреждение: пропущена строка с недостаточным количеством колонок" << std::endl;
            continue;
        }
        
        try {
            // Извлекаем числовое значение
            double value = std::stod(row[value_col]);
            values.push_back(value);
        } catch (const std::exception& e) {
            std::cerr << "Ошибка преобразования данных: " << e.what() << std::endl;
            continue;
        }
    }
    
    std::cout << "Загружено " << values.size() << " значений из " << filename << std::endl;
    return !values.empty();
}

/**
 * @brief Разделяет ряд на обучающую и тестовую выборки
 * @param train_ratio доля обучающей выборки (0.0 - 1.0)
 * @return пара: обучающая выборка, тестовая выборка
 */
std::pair<std::vector<double>, std::vector<double>> 
TimeSeries::split(double train_ratio) const {
    if (train_ratio <= 0.0 || train_ratio >= 1.0) {
        throw std::invalid_argument("train_ratio должен быть между 0.0 и 1.0");
    }
    
    size_t train_size = static_cast<size_t>(values.size() * train_ratio);
    
    std::vector<double> train_data(values.begin(), values.begin() + train_size);
    std::vector<double> test_data(values.begin() + train_size, values.end());
    
    return {train_data, test_data};
}