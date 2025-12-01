/**
 * @file seed.cpp
 * @brief Реализация 128-битного блочного шифра SEED
 */

#include "seed.h"
#include "seed_utils.h"
#include <cstring>
#include <stdexcept>
#include <algorithm>

using namespace seed_utils;

// ==================== ОСНОВНЫЕ МЕТОДЫ ====================

std::array<uint8_t, SEED::BLOCK_SIZE> SEED::encryptBlock(
    const std::array<uint8_t, BLOCK_SIZE>& plaintext,
    const std::array<uint8_t, KEY_SIZE>& key) {
    
    uint32_t roundKeys[32];
    generateRoundKeys(key, roundKeys);
    
    // Разбиваем блок на 4 слова
    uint32_t L0 = bytesToU32(plaintext.data());
    uint32_t L1 = bytesToU32(plaintext.data() + 4);
    uint32_t R0 = bytesToU32(plaintext.data() + 8);
    uint32_t R1 = bytesToU32(plaintext.data() + 12);
    
    // 16 раундов Фейстеля
    for (int round = 0; round < 16; round++) {
        uint32_t F0 = F(R0, roundKeys[2 * round], roundKeys[2 * round + 1]);
        uint32_t F1 = F(R1, roundKeys[2 * round + 1], roundKeys[2 * round]);
        
        // Фейстель
        uint32_t nextL0 = R0;
        uint32_t nextL1 = R1;
        R0 = L0 ^ F0;
        R1 = L1 ^ F1;
        L0 = nextL0;
        L1 = nextL1;
    }
    
    // Финальная перестановка
    std::array<uint8_t, BLOCK_SIZE> result;
    u32ToBytes(R0, result.data());
    u32ToBytes(R1, result.data() + 4);
    u32ToBytes(L0, result.data() + 8);
    u32ToBytes(L1, result.data() + 12);
    
    return result;
}

std::array<uint8_t, SEED::BLOCK_SIZE> SEED::decryptBlock(
    const std::array<uint8_t, BLOCK_SIZE>& ciphertext,
    const std::array<uint8_t, KEY_SIZE>& key) {
    
    uint32_t roundKeys[32];
    generateRoundKeys(key, roundKeys);
    
    // Разбиваем блок на 4 слова
    uint32_t L0 = bytesToU32(ciphertext.data());
    uint32_t L1 = bytesToU32(ciphertext.data() + 4);
    uint32_t R0 = bytesToU32(ciphertext.data() + 8);
    uint32_t R1 = bytesToU32(ciphertext.data() + 12);
    
    // 16 раундов Фейстеля в обратном порядке
    for (int round = 15; round >= 0; round--) {
        uint32_t F0 = F(R0, roundKeys[2 * round], roundKeys[2 * round + 1]);
        uint32_t F1 = F(R1, roundKeys[2 * round + 1], roundKeys[2 * round]);
        
        // Фейстель
        uint32_t nextL0 = R0;
        uint32_t nextL1 = R1;
        R0 = L0 ^ F0;
        R1 = L1 ^ F1;
        L0 = nextL0;
        L1 = nextL1;
    }
    
    // Финальная перестановка
    std::array<uint8_t, BLOCK_SIZE> result;
    u32ToBytes(R0, result.data());
    u32ToBytes(R1, result.data() + 4);
    u32ToBytes(L0, result.data() + 8);
    u32ToBytes(L1, result.data() + 12);
    
    return result;
}

// ==================== ПОТОКОВОЕ ШИФРОВАНИЕ ====================

std::vector<uint8_t> SEED::addPadding(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        // Если данные пустые, создаем полный блок padding
        return std::vector<uint8_t>(BLOCK_SIZE, static_cast<uint8_t>(BLOCK_SIZE));
    }
    
    size_t paddingLength = BLOCK_SIZE - (data.size() % BLOCK_SIZE);
    std::vector<uint8_t> padded = data;
    padded.resize(data.size() + paddingLength, static_cast<uint8_t>(paddingLength));
    return padded;
}

std::vector<uint8_t> SEED::removePadding(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::runtime_error("Cannot remove padding from empty data");
    }
    
    if (data.size() % BLOCK_SIZE != 0) {
        throw std::runtime_error("Data size must be multiple of block size");
    }
    
    uint8_t paddingLength = data.back();
    
    // Проверяем корректность padding
    if (paddingLength == 0 || paddingLength > BLOCK_SIZE) {
        throw std::runtime_error("Invalid padding length");
    }
    
    // Проверяем все байты padding
    size_t paddingStart = data.size() - paddingLength;
    for (size_t i = paddingStart; i < data.size(); ++i) {
        if (data[i] != paddingLength) {
            throw std::runtime_error("Invalid padding bytes");
        }
    }
    
    std::vector<uint8_t> unpadded(data.begin(), data.begin() + paddingStart);
    return unpadded;
}

std::vector<uint8_t> SEED::encrypt(
    const std::vector<uint8_t>& data,
    const std::array<uint8_t, KEY_SIZE>& key) {
    
    if (data.empty()) {
        return {};
    }
    
    // Добавляем padding
    auto paddedData = addPadding(data);
    std::vector<uint8_t> encrypted(paddedData.size());
    
    // Шифруем по блокам
    for (size_t i = 0; i < paddedData.size(); i += BLOCK_SIZE) {
        std::array<uint8_t, BLOCK_SIZE> block;
        std::copy_n(paddedData.begin() + i, BLOCK_SIZE, block.begin());
        
        auto encryptedBlock = encryptBlock(block, key);
        std::copy(encryptedBlock.begin(), encryptedBlock.end(), 
                  encrypted.begin() + i);
    }
    
    return encrypted;
}

std::vector<uint8_t> SEED::decrypt(
    const std::vector<uint8_t>& data,
    const std::array<uint8_t, KEY_SIZE>& key) {
    
    if (data.empty()) {
        return {};
    }
    
    if (data.size() % BLOCK_SIZE != 0) {
        throw std::runtime_error("Ciphertext size must be multiple of block size");
    }
    
    std::vector<uint8_t> decrypted(data.size());
    
    // Дешифруем по блокам
    for (size_t i = 0; i < data.size(); i += BLOCK_SIZE) {
        std::array<uint8_t, BLOCK_SIZE> block;
        std::copy_n(data.begin() + i, BLOCK_SIZE, block.begin());
        
        auto decryptedBlock = decryptBlock(block, key);
        std::copy(decryptedBlock.begin(), decryptedBlock.end(), 
                  decrypted.begin() + i);
    }
    
    // Удаляем padding
    return removePadding(decrypted);
}