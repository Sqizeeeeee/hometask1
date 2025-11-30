cmake_minimum_required(VERSION 3.10)
project(HoltWintersML)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включение отладочной информации
set(CMAKE_BUILD_TYPE Debug)

# Настройка путей
include_directories(include)

# Создание исполняемого файла
add_executable(holt_winters_main
    src/main_ml.cpp
    src/holt_winters.cpp
    src/metrics.cpp
    src/time_series.cpp
)

add_executable(first_tuning
    src/first_tuning.cpp
    src/holt_winters.cpp
    src/metrics.cpp
    src/time_series.cpp
)
add_executable(second_tuning
    src/second_tuning.cpp
    src/holt_winters.cpp
    src/metrics.cpp
    src/time_series.cpp
)

add_executable(third_tuning
    src/third_tuning.cpp
    src/holt_winters.cpp
    src/metrics.cpp
    src/time_series.cpp
)

add_executable(forth_tuning
    src/forth_tuning.cpp
    src/holt_winters.cpp
    src/metrics.cpp
    src/time_series.cpp
)


# Установка свойств компиляции
target_compile_features(holt_winters_main PRIVATE cxx_std_17)

# Настройка для разных компиляторов
if(MSVC)
    target_compile_options(holt_winters_main PRIVATE /W4)
else()
    target_compile_options(holt_winters_main PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Информация о проекте
message(STATUS "=== Конфигурация Holt-Winters ML ===")
message(STATUS "Проект: ${PROJECT_NAME}")
message(STATUS "Стандарт C++: ${CMAKE_CXX_STANDARD}")
message(STATUS "Компилятор: ${CMAKE_CXX_COMPILER}")
message(STATUS "Тип сборки: ${CMAKE_BUILD_TYPE}")