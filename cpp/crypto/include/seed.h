#ifndef SEED_H
#define SEED_H

#include <cstdint>
#include <vector>
#include <array>

/**
 * @class SEED
 * @brief 128-битный блочный шифр SEED
 */
class SEED {
public:
    static constexpr size_t BLOCK_SIZE = 16;  // 128 бит
    static constexpr size_t KEY_SIZE = 16;    // 128 бит
    static constexpr size_t ROUNDS = 16;
    
    // ==================== ОСНОВНЫЕ МЕТОДЫ ====================
    
    /**
     * @brief Шифрует один блок данных (128 бит)
     */
    static std::array<uint8_t, BLOCK_SIZE> encryptBlock(
        const std::array<uint8_t, BLOCK_SIZE>& plaintext,
        const std::array<uint8_t, KEY_SIZE>& key);
    
    /**
     * @brief Дешифрует один блок данных (128 бит)
     */
    static std::array<uint8_t, BLOCK_SIZE> decryptBlock(
        const std::array<uint8_t, BLOCK_SIZE>& ciphertext,
        const std::array<uint8_t, KEY_SIZE>& key);
    
    // ==================== ПОТОКОВОЕ ШИФРОВАНИЕ ====================
    
    /**
     * @brief Шифрует поток данных (добавляет padding)
     */
    static std::vector<uint8_t> encrypt(
        const std::vector<uint8_t>& data,
        const std::array<uint8_t, KEY_SIZE>& key);
    
    /**
     * @brief Дешифрует поток данных (удаляет padding)
     */
    static std::vector<uint8_t> decrypt(
        const std::vector<uint8_t>& data,
        const std::array<uint8_t, KEY_SIZE>& key);
    
private:
    // Вспомогательные методы для padding
    static std::vector<uint8_t> addPadding(const std::vector<uint8_t>& data);
    static std::vector<uint8_t> removePadding(const std::vector<uint8_t>& data);
};

#endif // SEED_H