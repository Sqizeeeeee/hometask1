/**
 * @file benchmark_utils.cpp
 * @brief Реализация утилит для измерения производительности
 */

#include "benchmark_utils.h"
#include <fstream>
#include <sys/resource.h>  // Для getrusage
#include <unistd.h>        // Для getpid
#include <cstring>
#include <sstream>
#include <iomanip>
#include <cstdlib>        // Для system()

namespace benchmark_utils {

// Реализация getCurrentMemoryUsage для Linux/macOS
size_t getCurrentMemoryUsage() {
    size_t memory_usage = 0;
    
#if defined(__APPLE__) || defined(__MACH__)
    // macOS - getrusage возвращает ru_maxrss в БАЙТАХ (начиная с OS X 10.6)
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        memory_usage = usage.ru_maxrss;  // Уже в байтах на современных macOS
        
        // Для совместимости проверяем, не слишком ли маленькое значение
        // Если < 1MB, возможно это в килобайтах (старые системы)
        if (memory_usage < 1024 * 1024) {
            memory_usage *= 1024;  // Конвертируем килобайты в байты
        }
    }
    
    // Альтернативный метод через malloc_size или malloc_statistics
    // (более точный, но сложнее в реализации)
    
#elif defined(__linux__)
    // Linux - читаем из /proc
    std::ifstream status("/proc/self/statm");
    if (status) {
        size_t size, resident, share, text, lib, data, dt;
        status >> size >> resident >> share >> text >> lib >> data >> dt;
        status.close();
        long page_size = sysconf(_SC_PAGESIZE);
        memory_usage = resident * page_size;  // RSS в байтах
    }
#endif
    
    // Дополнительная диагностика
    if (memory_usage == 0) {
        std::cerr << "⚠️  Не удалось измерить использование памяти" << std::endl;
    }
    
    return memory_usage;
}

bool saveAllResultsToJson(const std::vector<BenchmarkResult>& results, 
                         const std::string& filename) {
    try {
        // Создаем директорию
        size_t last_slash = filename.find_last_of("/");
        if (last_slash != std::string::npos) {
            std::string dir_path = filename.substr(0, last_slash);
            if (!createDirectory(dir_path)) {
                std::cerr << "❌ Не удалось создать директорию: " << dir_path << std::endl;
                return false;
            }
        }
        
        // Создаем JSON структуру
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        ss << "{\n";
        ss << "  \"benchmarks\": [\n";
        
        for (size_t i = 0; i < results.size(); i++) {
            const auto& result = results[i];
            
            ss << "    {\n";
            ss << "      \"id\": " << (i + 1) << ",\n";
            ss << "      \"algorithm\": \"" << result.algorithm << "\",\n";
            ss << "      \"dataset\": \"" << result.dataset << "\",\n";
            ss << "      \"blocks_processed\": " << result.blocks_processed << ",\n";
            ss << "      \"data_size_bytes\": " << result.data_size_bytes << ",\n";
            ss << "      \"data_size_mb\": " 
               << (result.data_size_bytes / (1024.0 * 1024.0)) << ",\n";
            
            // Timing metrics
            ss << "      \"timing\": {\n";
            ss << "        \"total_time_ms\": " << result.total_time_ms << ",\n";
            ss << "        \"encryption_time_ms\": " << result.encryption_time_ms << ",\n";
            ss << "        \"decryption_time_ms\": " << result.decryption_time_ms << ",\n";
            ss << "        \"encryption_speed_ops_sec\": " << result.encryption_speed_ops_sec << ",\n";
            ss << "        \"decryption_speed_ops_sec\": " << result.decryption_speed_ops_sec << "\n";
            ss << "      },\n";
            
            // Throughput metrics
            ss << "      \"throughput\": {\n";
            ss << "        \"encryption_mbps\": " << result.encryption_throughput_mbps << ",\n";
            ss << "        \"decryption_mbps\": " << result.decryption_throughput_mbps << ",\n";
            ss << "        \"total_mbps\": " 
               << (result.data_size_bytes * 8.0 / (result.total_time_ms / 1000.0) / 1e6) << "\n";
            ss << "      },\n";
            
            // Memory metrics
            ss << "      \"memory\": {\n";
            ss << "        \"usage_bytes\": " << result.memory_usage_bytes << ",\n";
            ss << "        \"usage_mb\": " << (result.memory_usage_bytes / (1024.0 * 1024.0)) << ",\n";
            ss << "        \"usage_kb\": " << (result.memory_usage_bytes / 1024.0) << ",\n";
            ss << "        \"bytes_per_block\": " 
               << (result.memory_usage_bytes / (double)result.blocks_processed) << "\n";
            ss << "      }\n";
            
            ss << "    }";
            if (i < results.size() - 1) {
                ss << ",";
            }
            ss << "\n";
        }
        
        ss << "  ],\n";
        
        // Metadata
        ss << "  \"metadata\": {\n";
        ss << "    \"timestamp\": \"" << __DATE__ << " " << __TIME__ << "\",\n";
        ss << "    \"platform\": \"";
        
#ifdef __APPLE__
        ss << "macOS";
#elif __linux__
        ss << "Linux";
#elif _WIN32
        ss << "Windows";
#else
        ss << "Unknown";
#endif
        
        ss << "\",\n";
        ss << "    \"compiler\": \"clang++\",\n";
        ss << "    \"block_size_bits\": 128,\n";
        ss << "    \"block_size_bytes\": 16,\n";
        ss << "    \"key_size_bytes\": 16,\n";
        ss << "    \"total_records\": " << results.back().blocks_processed << ",\n";
        ss << "    \"memory_measurement_method\": \"getrusage()\"\n";
        ss << "  }\n";
        ss << "}\n";
        
        // Сохраняем в файл
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "❌ Не удалось открыть файл для записи: " << filename << std::endl;
            return false;
        }
        
        file << ss.str();
        file.close();
        
        std::cout << "✅ Результаты сохранены в: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при сохранении результатов: " << e.what() << std::endl;
        return false;
    }
}

} // namespace benchmark_utils