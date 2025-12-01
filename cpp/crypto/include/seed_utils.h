#ifndef SEED_UTILS_H
#define SEED_UTILS_H

#include <cstdint>
#include <array>

/**
 * @namespace seed_utils
 * @brief Вспомогательные утилиты для алгоритма SEED
 */
namespace seed_utils {
    
    /**
     * @brief Константы KC для генерации ключей SEED
     */
    extern const uint32_t KC[16];
    
    /**
     * @brief Преобразует 4 байта в 32-битное слово (big-endian)
     * @param bytes Указатель на массив байт
     * @return 32-битное слово
     */
    uint32_t bytesToU32(const uint8_t* bytes);
    
    /**
     * @brief Преобразует 32-битное слово в 4 байта (big-endian)
     * @param value 32-битное слово
     * @param bytes Указатель на массив для записи байт
     */
    void u32ToBytes(uint32_t value, uint8_t* bytes);
    
    /**
     * @brief Циклический сдвиг влево
     * @param x Исходное значение
     * @param n Количество бит для сдвига
     * @return Результат сдвига
     */
    uint32_t rotl(uint32_t x, int n);
    
    /**
     * @brief Циклический сдвиг вправо
     * @param x Исходное значение
     * @param n Количество бит для сдвига
     * @return Результат сдвига
     */
    uint32_t rotr(uint32_t x, int n);
    
    /**
     * @brief S-бокс SS0 (упрощенная версия для тестов)
     * @param x Входной байт
     * @return Результат преобразования
     */
    uint32_t SS0(uint8_t x);
    
    /**
     * @brief S-бокс SS1 (упрощенная версия для тестов)
     * @param x Входной байт
     * @return Результат преобразования
     */
    uint32_t SS1(uint8_t x);
    
    /**
     * @brief S-бокс SS2 (упрощенная версия для тестов)
     * @param x Входной байт
     * @return Результат преобразования
     */
    uint32_t SS2(uint8_t x);
    
    /**
     * @brief S-бокс SS3 (упрощенная версия для тестов)
     * @param x Входной байт
     * @return Результат преобразования
     */
    uint32_t SS3(uint8_t x);
    
    /**
     * @brief G-функция алгоритма SEED
     * @param x Входное 32-битное слово
     * @return Результат G-функции
     */
    uint32_t G(uint32_t x);
    
    /**
     * @brief F-функция алгоритма SEED
     * @param x Входное слово
     * @param k0 Первый раундовый ключ
     * @param k1 Второй раундовый ключ
     * @return Результат F-функции
     */
    uint32_t F(uint32_t x, uint32_t k0, uint32_t k1);
    
    /**
     * @brief Генерирует раундовые ключи для SEED
     * @param key Основной ключ (128 бит)
     * @param roundKeys Массив для записи раундовых ключей (32 элемента)
     */
    void generateRoundKeys(const std::array<uint8_t, 16>& key, uint32_t roundKeys[32]);
}

#endif // SEED_UTILS_H