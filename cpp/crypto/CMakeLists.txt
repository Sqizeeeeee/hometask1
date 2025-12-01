cmake_minimum_required(VERSION 3.10)
project(seed_crypto_test)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включение отладочной информации
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Настройка путей
include_directories(include)

# Настройка компилятора
if(MSVC)
    add_compile_options(/W4 /DEBUG)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -g -O2)
endif()

# ==================== БИБЛИОТЕКА SEED ====================
add_library(seed_crypto STATIC
    src/seed.cpp
    src/seed_utils.cpp
    src/benchmark_utils.cpp
)

target_include_directories(seed_crypto PUBLIC include)

# ==================== ТЕСТ НА ДАННЫХ PAYSIM ====================
add_executable(seed_benchmark
    src/test_paysim.cpp
)

target_link_libraries(seed_benchmark seed_crypto)

# Настройка выходной директории - прямо в build
set_target_properties(seed_benchmark PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    OUTPUT_NAME "seed_benchmark"
)

# Создание необходимых директорий для результатов
add_custom_command(TARGET seed_benchmark POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/../../../results/crypto"
    COMMENT "Создание папки для результатов..."
)

# ==================== ИНФОРМАЦИЯ ====================
message(STATUS "=== Конфигурация SEED Crypto Benchmark ===")
message(STATUS "Проект: ${PROJECT_NAME}")
message(STATUS "Стандарт C++: ${CMAKE_CXX_STANDARD}")
message(STATUS "Компилятор: ${CMAKE_CXX_COMPILER}")
message(STATUS "Тип сборки: ${CMAKE_BUILD_TYPE}")
message(STATUS "Версия CMake: ${CMAKE_VERSION}")
message(STATUS "Исполняемый файл: seed_benchmark")
message(STATUS "Выходная папка: ${CMAKE_BINARY_DIR}")
message(STATUS "Папка результатов: ${CMAKE_SOURCE_DIR}/../../../results/crypto")

# Добавляем цель для запуска теста
add_custom_target(run_benchmark
    COMMAND ./seed_benchmark
    DEPENDS seed_benchmark
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Запуск SEED benchmark..."
)

# Добавляем цель для очистки результатов
add_custom_target(clean_results
    COMMAND rm -rf "${CMAKE_SOURCE_DIR}/../../../results/crypto/*.json"
    COMMENT "Очистка результатов benchmark..."
)

# Добавляем цель для очистки сборки
add_custom_target(clean_build
    COMMAND rm -rf "${CMAKE_BINARY_DIR}/*"
    COMMENT "Очистка папки сборки..."
)