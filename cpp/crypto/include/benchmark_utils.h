/**
 * @file benchmark_utils.h
 * @brief Утилиты для измерения производительности и использования памяти
 */

#ifndef BENCHMARK_UTILS_H
#define BENCHMARK_UTILS_H

#include <cstddef>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <sys/stat.h>  // Для mkdir
#include <cstring>
#include <cerrno>

namespace benchmark_utils {

/**
 * @brief Структура для хранения результатов benchmark
 */
struct BenchmarkResult {
    std::string algorithm;
    std::string dataset;
    double total_time_ms;
    double encryption_time_ms;
    double decryption_time_ms;
    size_t memory_usage_bytes;
    size_t data_size_bytes;
    size_t blocks_processed;
    double encryption_speed_ops_sec;
    double decryption_speed_ops_sec;
    double encryption_throughput_mbps;
    double decryption_throughput_mbps;
    
    // Пустой конструктор
    BenchmarkResult() 
        : total_time_ms(0), encryption_time_ms(0), decryption_time_ms(0),
          memory_usage_bytes(0), data_size_bytes(0), blocks_processed(0),
          encryption_speed_ops_sec(0), decryption_speed_ops_sec(0),
          encryption_throughput_mbps(0), decryption_throughput_mbps(0) {}
};

/**
 * @brief Класс для измерения времени выполнения
 */
class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::string name;
    bool stopped;
    double elapsed_ms;
    
public:
    Timer(const std::string& timer_name = "") : name(timer_name), stopped(false) {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    ~Timer() {
        if (!stopped && !name.empty()) {
            auto elapsed = this->elapsed();
            std::cout << "[TIMER] " << name << ": " << elapsed << " ms" << std::endl;
        }
    }
    
    double elapsed() {
        if (!stopped) {
            auto now = std::chrono::high_resolution_clock::now();
            elapsed_ms = std::chrono::duration<double, std::milli>(now - start_time).count();
        }
        return elapsed_ms;
    }
    
    double stop() {
        if (!stopped) {
            elapsed_ms = elapsed();
            stopped = true;
        }
        return elapsed_ms;
    }
    
    void reset() {
        start_time = std::chrono::high_resolution_clock::now();
        stopped = false;
    }
};

/**
 * @brief Создает директорию (рекурсивно)
 */
inline bool createDirectory(const std::string& path) {
    // Простая реализация для POSIX систем
    std::string cmd = "mkdir -p " + path;
    return system(cmd.c_str()) == 0;
}

/**
 * @brief Получает использование памяти процессом (RSS) в байтах
 * @return Использование памяти в байтах
 */
size_t getCurrentMemoryUsage();

/**
 * @brief Сохраняет все результаты в один JSON файл
 */
bool saveAllResultsToJson(const std::vector<BenchmarkResult>& results, 
                         const std::string& filename);

} // namespace benchmark_utils

#endif // BENCHMARK_UTILS_H