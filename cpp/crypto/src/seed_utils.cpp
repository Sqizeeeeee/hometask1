/**
 * @file seed_utils.cpp
 * @brief Реализация вспомогательных функций для алгоритма SEED
 */

#include "seed_utils.h"
#include <cstdint>
#include <array>

namespace seed_utils {
    
    // Константы KC для SEED
    const uint32_t KC[16] = {
        0x9e3779b9, 0x3c6ef373, 0x78dde6e6, 0xf1bbcdcc,
        0xe3779b99, 0xc6ef3733, 0x8dde6e67, 0x1bbcdccf,
        0x3779b99e, 0x6ef3733c, 0xdde6e678, 0xbbcdccf1,
        0x779b99e3, 0xef3733c6, 0xde6e678d, 0xbcdccf1b
    };
    
    uint32_t bytesToU32(const uint8_t* bytes) {
        return (static_cast<uint32_t>(bytes[0]) << 24) |
               (static_cast<uint32_t>(bytes[1]) << 16) |
               (static_cast<uint32_t>(bytes[2]) << 8) |
               static_cast<uint32_t>(bytes[3]);
    }
    
    void u32ToBytes(uint32_t value, uint8_t* bytes) {
        bytes[0] = static_cast<uint8_t>(value >> 24);
        bytes[1] = static_cast<uint8_t>(value >> 16);
        bytes[2] = static_cast<uint8_t>(value >> 8);
        bytes[3] = static_cast<uint8_t>(value);
    }
    
    uint32_t rotl(uint32_t x, int n) {
        return (x << n) | (x >> (32 - n));
    }
    
    uint32_t rotr(uint32_t x, int n) {
        return (x >> n) | (x << (32 - n));
    }
    
    uint32_t SS0(uint8_t x) {
        // Упрощенная S-бокс для тестов: умножение на константу
        return ((x * 0x1B) & 0xFF) * 0x01010101;
    }
    
    uint32_t SS1(uint8_t x) {
        // Упрощенная S-бокс для тестов: XOR + умножение
        return ((x ^ 0x5A) * 0x3D) * 0x01010101;
    }
    
    uint32_t SS2(uint8_t x) {
        // Упрощенная S-бокс для тестов: умножение + сдвиг
        return rotr((x * 0x2F) * 0x01010101, 8);
    }
    
    uint32_t SS3(uint8_t x) {
        // Упрощенная S-бокс для тестов: сложение + сдвиг
        return rotl(((x + 0x37) & 0xFF) * 0x01010101, 16);
    }
    
    uint32_t G(uint32_t x) {
        uint8_t b0 = (x >> 24) & 0xFF;
        uint8_t b1 = (x >> 16) & 0xFF;
        uint8_t b2 = (x >> 8) & 0xFF;
        uint8_t b3 = x & 0xFF;
        
        return SS0(b0) ^ SS1(b1) ^ SS2(b2) ^ SS3(b3);
    }
    
    uint32_t F(uint32_t x, uint32_t k0, uint32_t k1) {
        uint32_t g1 = G(x ^ k0);
        uint32_t g2 = G(rotl(x ^ k1, 8));
        return rotl(g1 + g2, 1);
    }
    
    void generateRoundKeys(const std::array<uint8_t, 16>& key, uint32_t roundKeys[32]) {
        // Преобразуем ключ в 4 слова
        uint32_t A = bytesToU32(key.data());
        uint32_t B = bytesToU32(key.data() + 4);
        uint32_t C = bytesToU32(key.data() + 8);
        uint32_t D = bytesToU32(key.data() + 12);
        
        // Генерируем 16 пар раундовых ключей
        for (int i = 0; i < 16; i++) {
            // Вычисляем T0 и T1
            uint32_t T0 = (A + C - KC[i]) & 0xFFFFFFFF;
            uint32_t T1 = (B - D + KC[i]) & 0xFFFFFFFF;
            
            // Генерируем пару ключей
            roundKeys[2 * i] = rotl(T0, KC[i] & 0x1F);
            roundKeys[2 * i + 1] = rotl(T1, KC[i] & 0x1F);
            
            // Обновляем A, B, C, D для следующего раунда
            if (i % 2 == 0) {
                // Четные раунды: сдвиг
                A = rotr(A, 8);
                B = rotl(B, 8);
            } else {
                // Нечетные раунды: перестановка
                uint32_t temp = A;
                A = C;
                C = temp;
                temp = B;
                B = D;
                D = temp;
            }
        }
    }
}