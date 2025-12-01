"""
@file comparison.py
@brief Сравнение производительности SEED на разных размерах данных
"""

import json
import matplotlib.pyplot as plt
import numpy as np
import os
from pathlib import Path

def load_and_compare():
    """Загружает и сравнивает результаты"""
    json_path = "../../../results/crypto/seed_multi_benchmark.json"
    
    if not os.path.exists(json_path):
        print(f"Файл {json_path} не найден!")
        return
    
    with open(json_path, 'r') as f:
        data = json.load(f)
    
    benchmarks = data['benchmarks']
    
    # Создаем графики для сравнения
    create_comparison_charts(benchmarks)

def create_comparison_charts(benchmarks):
    """Создает сравнительные графики"""
    graphs_dir = "../../../results/crypto/graphs"
    Path(graphs_dir).mkdir(parents=True, exist_ok=True)
    
    blocks = [b['blocks_processed'] for b in benchmarks]
    
    # 1. График масштабируемости
    plt.figure(figsize=(12, 8))
    
    # Скорость в зависимости от размера данных
    encryption_speeds = [b['timing']['encryption_speed_ops_sec'] / 1000 for b in benchmarks]
    decryption_speeds = [b['timing']['decryption_speed_ops_sec'] / 1000 for b in benchmarks]
    
    plt.subplot(2, 2, 1)
    plt.plot(blocks, encryption_speeds, 'o-', label='Шифрование', linewidth=2)
    plt.plot(blocks, decryption_speeds, 's-', label='Дешифрование', linewidth=2)
    plt.xscale('log')
    plt.xlabel('Размер данных (блоки)')
    plt.ylabel('Скорость (тыс. блоков/сек)')
    plt.title('Масштабируемость скорости')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # 2. Эффективность памяти
    memory_per_block = [b['memory']['bytes_per_block'] for b in benchmarks]
    
    plt.subplot(2, 2, 2)
    plt.plot(blocks, memory_per_block, 'o-', color='purple', linewidth=2)
    plt.xscale('log')
    plt.xlabel('Размер данных (блоки)')
    plt.ylabel('Память на блок (байты)')
    plt.title('Эффективность использования памяти')
    plt.grid(True, alpha=0.3)
    
    # 3. Время на операцию
    time_per_block_enc = [b['timing']['encryption_time_ms'] * 1000 / b['blocks_processed'] for b in benchmarks]  # микросекунды
    time_per_block_dec = [b['timing']['decryption_time_ms'] * 1000 / b['blocks_processed'] for b in benchmarks]
    
    plt.subplot(2, 2, 3)
    plt.plot(blocks, time_per_block_enc, 'o-', label='Шифрование', linewidth=2)
    plt.plot(blocks, time_per_block_dec, 's-', label='Дешифрование', linewidth=2)
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Размер данных (блоки)')
    plt.ylabel('Время на блок (мкс)')
    plt.title('Латентность на операцию')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # 4. Соотношение шифрование/дешифрование
    ratio = [e/d for e, d in zip(encryption_speeds, decryption_speeds)]
    
    plt.subplot(2, 2, 4)
    plt.plot(blocks, ratio, 'o-', color='red', linewidth=2)
    plt.axhline(y=1.0, color='gray', linestyle='--', alpha=0.5)
    plt.xscale('log')
    plt.xlabel('Размер данных (блоки)')
    plt.ylabel('Соотношение (шифр/дешифр)')
    plt.title('Соотношение производительности')
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig(f'{graphs_dir}/scalability_analysis.png', dpi=300)
    plt.close()
    
    print(f"✅ Сравнительные графики сохранены в: {graphs_dir}")

if __name__ == "__main__":
    load_and_compare()