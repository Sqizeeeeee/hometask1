"""
Подготовка Bitcoin данных для алгоритма шифрования SEED
Находится в: python/crypto/data_processing/
"""
import pandas as pd
import numpy as np
import os
import json
from pathlib import Path

def prepare_bitcoin_data():
    """Основная функция подготовки данных"""
    print("=== ПОДГОТОВКА BITCOIN ДАННЫХ ДЛЯ ШИФРОВАНИЯ SEED ===")
    
    # Пути
    script_dir = Path(__file__).parent
    crypto_dir = script_dir.parent
    python_dir = crypto_dir.parent
    project_root = python_dir.parent
    
    raw_file = project_root / 'data' / 'raw' / 'btcusd_1-min_data.csv'
    output_dir = project_root / 'data' / 'processed'
    output_dir.mkdir(exist_ok=True)
    
    print(f"Корень проекта: {project_root}")
    print(f"Исходный файл: {raw_file}")
    
    # Параметры
    SKIP_ROWS = 3600000  # Пропускаем 3.6 млн строк
    NUM_ROWS = 50000     # Берем 50к строк
    
    print(f"\n📖 Чтение данных...")
    print(f"   Пропуск: {SKIP_ROWS:,} строк")
    print(f"   Чтение: {NUM_ROWS:,} строк")
    
    # Чтение данных - ВАЖНО: header=None так как пропускаем заголовок
    try:
        df = pd.read_csv(
            raw_file,
            skiprows=SKIP_ROWS,  # Пропускаем SKIP_ROWS строк
            nrows=NUM_ROWS,
            header=None,  # Нет заголовка в этой части файла
            names=['Timestamp', 'Open', 'High', 'Low', 'Close', 'Volume']
        )
    except Exception as e:
        print(f"❌ Ошибка: {e}")
        return
    
    print(f"✅ Загружено {len(df):,} строк")
    
    # Проверяем первые строки
    print(f"\n🔍 Проверка данных (первые 3 строки):")
    print(df.head(3))
    
    # Сохраняем CSV
    csv_file = output_dir / 'bitcoin_50000.csv'
    df.to_csv(csv_file, index=False)
    print(f"\n💾 Полный набор: {csv_file.name}")
    print(f"   Размер: {csv_file.stat().st_size:,} байт")
    
    # Тестовый набор
    test_file = output_dir / 'bitcoin_test_1000.csv'
    df.head(1000).to_csv(test_file, index=False)
    print(f"🧪 Тестовый набор: {test_file.name}")
    print(f"   Строк: 1,000")
    
    # Подготовка данных для шифрования
    print("\n=== ДАННЫЕ ДЛЯ ШИФРОВАНИЯ ===")
    
    # Преобразуем данные в числа
    df['Close'] = pd.to_numeric(df['Close'], errors='coerce')
    df['Volume'] = pd.to_numeric(df['Volume'], errors='coerce')
    
    # 1. Цены Close
    close_prices = df['Close'].dropna().values
    close_int = []
    for price in close_prices:
        try:
            close_int.append(int(float(price) * 1000))
        except:
            continue
    
    # 2. Объемы
    volumes = df['Volume'].dropna().values
    volume_int = []
    for vol in volumes:
        try:
            volume_int.append(int(float(vol)))
        except:
            continue
    
    print(f"💰 Цены Close: {len(close_int):,} значений")
    if close_int:
        print(f"   Диапазон: {min(close_int):,} - {max(close_int):,}")
    
    print(f"📦 Объемы: {len(volume_int):,} значений")
    if volume_int:
        print(f"   Диапазон: {min(volume_int):,} - {max(volume_int):,}")
    
    # Сохраняем для C++
    def save_for_cpp(filename, data):
        filepath = output_dir / filename
        with open(filepath, 'w') as f:
            f.write(f"{len(data)}\n")
            for value in data:
                f.write(f"{value}\n")
        return filepath
    
    prices_file = save_for_cpp('crypto_close_prices.txt', close_int)
    volumes_file = save_for_cpp('crypto_volumes.txt', volume_int)
    
    print(f"\n✅ Файлы созданы:")
    print(f"   {prices_file.name}: {len(close_int):,} значений")
    print(f"   {volumes_file.name}: {len(volume_int):,} значений")
    
    # Метаданные
    metadata = {
        "files": ["bitcoin_50000.csv", "bitcoin_test_1000.csv", 
                 "crypto_close_prices.txt", "crypto_volumes.txt"],
        "data_ready": True
    }
    
    with open(output_dir / 'crypto_info.json', 'w') as f:
        json.dump(metadata, f, indent=2)
    
    print(f"\n🎉 Данные готовы для шифрования SEED!")
    print(f"📁 Папка: {output_dir}")

if __name__ == "__main__":
    prepare_bitcoin_data()