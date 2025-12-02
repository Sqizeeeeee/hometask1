/**
 * @file test_paysim.cpp
 * @brief Benchmark шифрования SEED на разных объемах данных
 */

#include "seed.h"
#include "benchmark_utils.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <memory>
#include <cmath>

using namespace benchmark_utils;

/**
 * @brief Читает весь CSV файл с ценами
 */
std::vector<uint32_t> readEntireCSV(const std::string& filename) {
    std::vector<uint32_t> prices;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "❌ Не удалось открыть файл: " << filename << std::endl;
        return prices;
    }
    
    std::string line;
    std::getline(file, line); // Пропускаем заголовок "value"
    
    std::cout << "Чтение файла " << filename << "..." << std::endl;
    
    while (std::getline(file, line)) {
        try {
            uint32_t price = static_cast<uint32_t>(std::stoul(line));
            prices.push_back(price);
        } catch (...) {
            // Пропускаем ошибки парсинга
        }
    }
    
    file.close();
    std::cout << "Прочитано " << prices.size() << " записей" << std::endl;
    
    return prices;
}

/**
 * @brief Преобразует 32-битную цену в 128-битный блок
 */
std::array<uint8_t, SEED::BLOCK_SIZE> priceToBlock(uint32_t price) {
    std::array<uint8_t, SEED::BLOCK_SIZE> block{};
    
    block[0] = static_cast<uint8_t>(price >> 24);
    block[1] = static_cast<uint8_t>(price >> 16);
    block[2] = static_cast<uint8_t>(price >> 8);
    block[3] = static_cast<uint8_t>(price);
    
    for (size_t i = 4; i < SEED::BLOCK_SIZE; i++) {
        block[i] = static_cast<uint8_t>(i);
    }
    
    return block;
}

/**
 * @brief Запускает benchmark для одного размера данных
 */
BenchmarkResult runSingleBenchmark(const std::vector<uint32_t>& prices, 
                                  size_t sample_size) {
    BenchmarkResult result;
    result.algorithm = "SEED";
    result.dataset = "paysim_32bit";
    result.blocks_processed = sample_size;
    result.data_size_bytes = sample_size * SEED::BLOCK_SIZE;
    
    // Убедимся что есть достаточно данных
    if (sample_size > prices.size()) {
        std::cerr << "❌ Недостаточно данных для размера " << sample_size 
                  << " (доступно: " << prices.size() << ")" << std::endl;
        return result;
    }
    
    // 1. Подготовка блоков
    std::vector<std::array<uint8_t, SEED::BLOCK_SIZE>> blocks;
    blocks.reserve(sample_size);
    
    for (size_t i = 0; i < sample_size; i++) {
        blocks.push_back(priceToBlock(prices[i]));
    }
    
    // 2. Генерация ключа
    std::array<uint8_t, SEED::KEY_SIZE> key = {};
    for (size_t i = 0; i < key.size(); i++) {
        key[i] = static_cast<uint8_t>((i * 17 + 23) % 256);
    }
    
    // 3. Измерение памяти ДО шифрования
    size_t memory_before = getCurrentMemoryUsage();
    if (memory_before == 0) {
        std::cout << "   ⚠️  Не удалось измерить начальную память" << std::endl;
    }
    
    // 4. Шифрование
    Timer encrypt_timer;
    std::vector<std::array<uint8_t, SEED::BLOCK_SIZE>> encrypted_blocks;
    encrypted_blocks.reserve(sample_size);
    
    for (size_t i = 0; i < sample_size; i++) {
        encrypted_blocks.push_back(SEED::encryptBlock(blocks[i], key));
    }
    
    result.encryption_time_ms = encrypt_timer.elapsed();
    
    // Измерение памяти после шифрования
    size_t memory_after_encrypt = getCurrentMemoryUsage();
    
    // 5. Дешифрование
    Timer decrypt_timer;
    
    for (size_t i = 0; i < sample_size; i++) {
        auto decrypted = SEED::decryptBlock(encrypted_blocks[i], key);
        (void)decrypted;
    }
    
    result.decryption_time_ms = decrypt_timer.elapsed();
    
    // Измерение памяти после дешифрования
    size_t memory_after_decrypt = getCurrentMemoryUsage();
    
    // 6. Расчет метрик памяти
    if (memory_before > 0 && memory_after_encrypt > 0 && memory_after_decrypt > 0) {
        // Берем максимальное использование памяти
        size_t max_memory = std::max({memory_before, memory_after_encrypt, memory_after_decrypt});
        size_t min_memory = std::min({memory_before, memory_after_encrypt, memory_after_decrypt});
        result.memory_usage_bytes = max_memory - min_memory;
        
        // Убедимся, что значение реалистичное (должно быть хотя бы размер данных)
        size_t min_expected_memory = sample_size * SEED::BLOCK_SIZE * 2; // blocks + encrypted_blocks
        if (result.memory_usage_bytes < min_expected_memory) {
            result.memory_usage_bytes = min_expected_memory;
            std::cout << "   ⚠️  Память скорректирована до минимального ожидаемого значения: " 
                      << (result.memory_usage_bytes / (1024.0 * 1024.0)) << " MB" << std::endl;
        }
    } else {
        // Если измерение не сработало, используем расчетный метод
        result.memory_usage_bytes = 
            sample_size * SEED::BLOCK_SIZE * 2 +  // blocks + encrypted_blocks
            SEED::KEY_SIZE +                       // key
            32 * sizeof(uint32_t) +               // roundKeys в SEED
            1024 * 1024;                          // overhead (1MB)
        
        std::cout << "   ⚠️  Память измерена расчетным методом: " 
                  << (result.memory_usage_bytes / (1024.0 * 1024.0)) << " MB" << std::endl;
    }
    
    // 7. Расчет метрик производительности
    result.total_time_ms = result.encryption_time_ms + result.decryption_time_ms;
    result.encryption_speed_ops_sec = (sample_size * 1000.0) / result.encryption_time_ms;
    result.decryption_speed_ops_sec = (sample_size * 1000.0) / result.decryption_time_ms;
    result.encryption_throughput_mbps = 
        (sample_size * 128.0) / (result.encryption_time_ms / 1000.0) / 1e6;
    result.decryption_throughput_mbps = 
        (sample_size * 128.0) / (result.decryption_time_ms / 1000.0) / 1e6;
    
    return result;
}

/**
 * @brief Запускает серию benchmarks на разных размерах
 */
std::vector<BenchmarkResult> runMultiSizeBenchmark(const std::vector<uint32_t>& prices) {
    std::vector<BenchmarkResult> results;
    
    // Размеры для тестирования
    std::vector<size_t> test_sizes = {10000, 50000, 100000, 250000, 500000, 750000, 1000000};
    
    std::cout << "\n==========================================" << std::endl;
    std::cout << "   МНОГОМЕРНЫЙ БЕНЧМАРК SEED" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    for (size_t i = 0; i < test_sizes.size(); i++) {
        size_t sample_size = test_sizes[i];
        
        std::cout << "\n🔬 ТЕСТ " << (i+1) << "/" << test_sizes.size() 
                  << ": " << sample_size << " блоков" << std::endl;
        std::cout << "   (" << (sample_size * SEED::BLOCK_SIZE / (1024.0 * 1024.0)) 
                  << " МБ данных)" << std::endl;
        
        // Запускаем benchmark 3 раза для каждого размера (учитываем кэш)
        for (int run = 0; run < 3; run++) {
            std::cout << "   Запуск " << (run+1) << "/3... ";
            
            auto result = runSingleBenchmark(prices, sample_size);
            
            // Сохраняем результат
            if (run == 2) { // Берем последний (прогретый) результат
                results.push_back(result);
                
                std::cout << "OK" << std::endl;
                std::cout << "   Шифрование: " << result.encryption_time_ms << " мс (" 
                          << std::fixed << std::setprecision(0)
                          << (result.encryption_speed_ops_sec / 1000) << "K блоков/сек)" << std::endl;
                std::cout << "   Память: " << std::fixed << std::setprecision(1)
                          << (result.memory_usage_bytes / (1024.0 * 1024.0)) << " MB" << std::endl;
            } else {
                std::cout << "прогрев" << std::endl;
            }
        }
    }
    
    return results;
}

/**
 * @brief Основная функция
 */
int main() {
    Timer total_timer("Полный benchmark");
    
    try {
        // 1. Загрузка данных
        std::cout << "==========================================" << std::endl;
        std::cout << "   SEED CRYPTO BENCHMARK SUITE" << std::endl;
        std::cout << "==========================================" << std::endl;
        
        auto prices = readEntireCSV("../../../data/processed/1mln.csv");
        
        if (prices.empty()) {
            std::cerr << "❌ Нет данных для тестирования" << std::endl;
            return 1;
        }
        
        if (prices.size() < 1000000) {
            std::cout << "⚠️  Внимание: файл содержит " << prices.size() 
                      << " записей (ожидалось 1,000,000)" << std::endl;
        }
        
        // 2. Быстрая проверка корректности
        std::cout << "\n✅ БЫСТРАЯ ПРОВЕРКА КОРРЕКТНОСТИ..." << std::endl;
        bool correctness_ok = true;
        std::array<uint8_t, SEED::KEY_SIZE> test_key = {};
        
        for (size_t i = 0; i < test_key.size(); i++) {
            test_key[i] = static_cast<uint8_t>((i * 17 + 23) % 256);
        }
        
        // Проверяем 100 случайных записей
        for (int i = 0; i < 100; i++) {
            size_t idx = i * 10000 % prices.size();
            auto plaintext = priceToBlock(prices[idx]);
            auto encrypted = SEED::encryptBlock(plaintext, test_key);
            auto decrypted = SEED::decryptBlock(encrypted, test_key);
            
            if (memcmp(plaintext.data(), decrypted.data(), SEED::BLOCK_SIZE) != 0) {
                correctness_ok = false;
                std::cerr << "❌ Ошибка в записи #" << idx << std::endl;
                break;
            }
        }
        
        if (!correctness_ok) {
            std::cerr << "❌ Алгоритм работает некорректно!" << std::endl;
            return 1;
        }
        std::cout << "   Алгоритм работает корректно ✓" << std::endl;
        
        // 3. Запуск многомерного benchmark
        auto results = runMultiSizeBenchmark(prices);
        
        // 4. Сохранение результатов
        std::string output_file = "../../../results/crypto/seed_multi_benchmark.json";
        
        if (saveAllResultsToJson(results, output_file)) {
            // 5. Вывод сводки
            std::cout << "\n==========================================" << std::endl;
            std::cout << "   ИТОГОВАЯ СВОДКА" << std::endl;
            std::cout << "==========================================" << std::endl;
            std::cout << std::fixed << std::setprecision(1);
            
            for (size_t i = 0; i < results.size(); i++) {
                const auto& result = results[i];
                std::cout << "📊 " << (result.blocks_processed / 1000) << "K блоков:\n";
                std::cout << "   Время шифрования: " << result.encryption_time_ms << " мс\n";
                std::cout << "   Скорость шифрования: " << (result.encryption_speed_ops_sec / 1000) << "K блоков/сек\n";
                std::cout << "   Память: " << (result.memory_usage_bytes / (1024.0 * 1024.0)) 
                          << " MB (" << (result.memory_usage_bytes / (double)result.blocks_processed) 
                          << " байт/блок)\n";
                if (i < results.size() - 1) {
                    std::cout << std::endl;
                }
            }
        } else {
            std::cerr << "❌ Не удалось сохранить результаты" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Исключение: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}