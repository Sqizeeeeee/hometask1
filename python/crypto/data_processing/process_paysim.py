#!/usr/bin/env python3
"""
Обработка датасета PaySim для тестирования SEED шифрования
"""

import pandas as pd
import numpy as np
import os
from pathlib import Path

def process_paysim():
    # Пути
    project_root = Path(__file__).parent.parent.parent.parent
    raw_data_path = project_root / 'data' / 'raw' / 'PS_20174392719_1491204439457_log.csv'
    processed_dir = project_root / 'data' / 'processed'
    processed_dir.mkdir(exist_ok=True)
    
    print(f"Чтение файла: {raw_data_path}")
    
    # Читаем данные (только первые 1M строк для скорости)
    df = pd.read_csv(raw_data_path, nrows=1000000)
    print(f"Загружено строк: {len(df)}")
    print(f"Колонки: {df.columns.tolist()}")
    print("\nПервые 5 строк:")
    print(df.head())
    
    # ====== 1. 32-битные данные (amount) ======
    print("\n1. Подготовка 32-битных данных...")
    # Берем amount, умножаем на 100 для 2 знаков после запятой
    amounts = df['amount'].dropna().values
    amounts_int = (amounts * 100).astype(np.uint32)
    
    # Сохраняем
    amounts_path = processed_dir / 'paysim_32bit.csv'
    pd.DataFrame({'value': amounts_int}).to_csv(amounts_path, index=False)
    print(f"   Сохранено: {amounts_path}")
    print(f"   Записей: {len(amounts_int)}")
    print(f"   Примеры: {amounts_int[:5]}")
    
    # ====== 2. 64-битные данные (oldbalance, newbalance) ======
    print("\n2. Подготовка 64-битных данных...")
    # Берем балансы
    oldbalance = df['oldbalanceOrg'].dropna().values
    newbalance = df['newbalanceOrig'].dropna().values
    
    # Умножаем на 100 и преобразуем в uint64
    oldbalance_int = (oldbalance * 100).astype(np.uint64)
    newbalance_int = (newbalance * 100).astype(np.uint64)
    
    # Сохраняем оба набора
    balances_path = processed_dir / 'paysim_64bit_old.csv'
    pd.DataFrame({'value': oldbalance_int}).to_csv(balances_path, index=False)
    print(f"   Сохранено: {balances_path}")
    print(f"   Записей: {len(oldbalance_int)}")
    
    balances_path = processed_dir / 'paysim_64bit_new.csv'
    pd.DataFrame({'value': newbalance_int}).to_csv(balances_path, index=False)
    print(f"   Сохранено: {balances_path}")
    print(f"   Записей: {len(newbalance_int)}")
    
    # ====== 3. 8-битные данные (step, type) ======
    print("\n3. Подготовка 8-битных данных...")
    # step: час транзакции (0-743)
    steps = df['step'].values
    steps_8bit = steps.astype(np.uint8)  # Оборачиваем в 0-255
    
    # type: категориальный -> числовой
    type_mapping = {'CASH_OUT': 1, 'PAYMENT': 2, 'CASH_IN': 3, 'TRANSFER': 4, 'DEBIT': 5}
    types_numeric = df['type'].map(type_mapping).fillna(0).astype(np.uint8)
    
    # Сохраняем
    steps_path = processed_dir / 'paysim_8bit_step.csv'
    pd.DataFrame({'value': steps_8bit}).to_csv(steps_path, index=False)
    print(f"   Сохранено: {steps_path}")
    
    types_path = processed_dir / 'paysim_8bit_type.csv'
    pd.DataFrame({'value': types_numeric}).to_csv(types_path, index=False)
    print(f"   Сохранено: {types_path}")
    
    # ====== 4. Статистика ======
    print("\n4. Статистика данных:")
    print(f"   Amounts (32-bit):")
    print(f"     Min: {amounts_int.min()}, Max: {amounts_int.max()}")
    print(f"     Mean: {amounts_int.mean():.0f}, Std: {amounts_int.std():.0f}")
    
    print(f"\n   Old Balance (64-bit):")
    print(f"     Min: {oldbalance_int.min()}, Max: {oldbalance_int.max()}")
    
    print(f"\n   Steps (8-bit):")
    print(f"     Unique values: {np.unique(steps_8bit).size}")
    
    # ====== 5. Создаем тестовый файл для C++ ======
    print("\n5. Создание тестового файла для C++...")
    test_data = {
        'files': [
            'paysim_8bit_step.csv',
            'paysim_8bit_type.csv',
            'paysim_32bit.csv',
            'paysim_64bit_old.csv',
            'paysim_64bit_new.csv'
        ],
        'data_info': {
            'dataset': 'PaySim Synthetic Financial Transactions',
            'source': 'https://www.kaggle.com/datasets/ealaxi/paysim1',
            'rows_processed': len(df),
            'samples_8bit': len(steps_8bit),
            'samples_32bit': len(amounts_int),
            'samples_64bit': len(oldbalance_int),
            'data_types': '8-bit, 32-bit, 64-bit integers',
            'description': 'Financial amounts and balances for SEED encryption testing'
        }
    }
    
    import json
    info_path = processed_dir / 'paysim_test_info.json'
    with open(info_path, 'w') as f:
        json.dump(test_data, f, indent=2)
    
    print(f"   Сохранено: {info_path}")
    print("\n✅ Обработка завершена!")

if __name__ == "__main__":
    process_paysim()