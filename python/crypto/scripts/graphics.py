"""
@file graphics.py
@brief Визуализация результатов benchmark SEED
"""

import json
import matplotlib.pyplot as plt
import numpy as np
import os
import csv
from pathlib import Path
import sys

def ensure_directory(path):
    """Создает директорию если ее нет"""
    Path(path).mkdir(parents=True, exist_ok=True)

def load_benchmark_data():
    """Загружает данные из JSON файла"""
    # Путь к JSON файлу
    json_path = "../../../results/crypto/seed_multi_benchmark.json"
    
    if not os.path.exists(json_path):
        print(f"❌ Файл {json_path} не найден!")
        print("Запустите сначала benchmark: ./seed_benchmark")
        return None
    
    try:
        with open(json_path, 'r') as f:
            data = json.load(f)
        return data
    except Exception as e:
        print(f"❌ Ошибка при чтении JSON файла: {e}")
        return None

def create_performance_plots(data):
    """Создает графики производительности"""
    benchmarks = data['benchmarks']
    
    # Подготовка данных
    blocks = [b['blocks_processed'] for b in benchmarks]
    encryption_times = [b['timing']['encryption_time_ms'] for b in benchmarks]
    decryption_times = [b['timing']['decryption_time_ms'] for b in benchmarks]
    encryption_speeds = [b['timing']['encryption_speed_ops_sec'] / 1000 for b in benchmarks]  # K ops/sec
    decryption_speeds = [b['timing']['decryption_speed_ops_sec'] / 1000 for b in benchmarks]  # K ops/sec
    memory_usage = [b['memory']['usage_mb'] for b in benchmarks]
    encryption_throughput = [b['throughput']['encryption_mbps'] for b in benchmarks]
    decryption_throughput = [b['throughput']['decryption_mbps'] for b in benchmarks]
    
    # Создаем папку для графиков
    graphs_dir = "../../../results/crypto/graphs"
    ensure_directory(graphs_dir)
    
    # 1. График времени выполнения (отдельный)
    plt.figure(figsize=(10, 6))
    plt.plot(blocks, encryption_times, 'o-', label='Шифрование', linewidth=2, markersize=8, color='blue')
    plt.plot(blocks, decryption_times, 's-', label='Дешифрование', linewidth=2, markersize=8, color='red')
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Количество блоков (логарифмическая шкала)')
    plt.ylabel('Время (мс, логарифмическая шкала)')
    plt.title('Время выполнения SEED vs Размер данных')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(f'{graphs_dir}/execution_time.png', dpi=300)
    plt.close()
    
    # 2. График скорости (отдельный)
    plt.figure(figsize=(10, 6))
    plt.plot(blocks, encryption_speeds, 'o-', label='Шифрование', linewidth=2, markersize=8, color='green')
    plt.plot(blocks, decryption_speeds, 's-', label='Дешифрование', linewidth=2, markersize=8, color='orange')
    plt.xscale('log')
    plt.xlabel('Количество блоков (логарифмическая шкала)')
    plt.ylabel('Скорость (тыс. блоков/сек)')
    plt.title('Скорость обработки SEED vs Размер данных')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(f'{graphs_dir}/processing_speed.png', dpi=300)
    plt.close()
    
    # 3. График использования памяти (отдельный)
    plt.figure(figsize=(10, 6))
    bars = plt.bar(range(len(blocks)), memory_usage, color='purple', alpha=0.7)
    plt.xlabel('Размер данных')
    plt.ylabel('Использование памяти (МБ)')
    plt.title('Использование памяти SEED vs Размер данных')
    plt.xticks(range(len(blocks)), [f'{b//1000}K' for b in blocks], rotation=45)
    plt.grid(True, alpha=0.3, axis='y')
    
    for bar, mem in zip(bars, memory_usage):
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2., height + 0.5,
                f'{mem:.1f} MB', ha='center', va='bottom', fontsize=9)
    
    plt.tight_layout()
    plt.savefig(f'{graphs_dir}/memory_usage.png', dpi=300)
    plt.close()
    
    # 4. График пропускной способности (отдельный)
    plt.figure(figsize=(10, 6))
    x = np.arange(len(blocks))
    width = 0.35
    bars1 = plt.bar(x - width/2, encryption_throughput, width, label='Шифрование', color='blue', alpha=0.7)
    bars2 = plt.bar(x + width/2, decryption_throughput, width, label='Дешифрование', color='red', alpha=0.7)
    
    plt.xlabel('Размер данных')
    plt.ylabel('Пропускная способность (Мбит/сек)')
    plt.title('Пропускная способность SEED vs Размер данных')
    plt.xticks(x, [f'{b//1000}K' for b in blocks], rotation=45)
    plt.legend()
    plt.grid(True, alpha=0.3, axis='y')
    
    for bars in [bars1, bars2]:
        for bar in bars:
            height = bar.get_height()
            plt.text(bar.get_x() + bar.get_width()/2., height + 5,
                    f'{height:.0f}', ha='center', va='bottom', fontsize=8)
    
    plt.tight_layout()
    plt.savefig(f'{graphs_dir}/throughput.png', dpi=300)
    plt.close()
    
    # 5. Сводный график (2x2)
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Результаты Benchmark алгоритма SEED', fontsize=16, fontweight='bold')
    
    # 5.1 Время выполнения
    ax1 = axes[0, 0]
    ax1.plot(blocks, encryption_times, 'o-', label='Шифрование', linewidth=2, markersize=6, color='blue')
    ax1.plot(blocks, decryption_times, 's-', label='Дешифрование', linewidth=2, markersize=6, color='red')
    ax1.set_xscale('log')
    ax1.set_yscale('log')
    ax1.set_xlabel('Количество блоков')
    ax1.set_ylabel('Время (мс)')
    ax1.set_title('Время выполнения')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # 5.2 Скорость обработки
    ax2 = axes[0, 1]
    ax2.plot(blocks, encryption_speeds, 'o-', label='Шифрование', linewidth=2, markersize=6, color='green')
    ax2.plot(blocks, decryption_speeds, 's-', label='Дешифрование', linewidth=2, markersize=6, color='orange')
    ax2.set_xscale('log')
    ax2.set_xlabel('Количество блоков')
    ax2.set_ylabel('Скорость (тыс. блоков/сек)')
    ax2.set_title('Скорость обработки')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # 5.3 Использование памяти
    ax3 = axes[1, 0]
    bars = ax3.bar(range(len(blocks)), memory_usage, color='purple', alpha=0.7)
    ax3.set_xlabel('Размер данных')
    ax3.set_ylabel('Память (МБ)')
    ax3.set_title('Использование памяти')
    ax3.set_xticks(range(len(blocks)))
    ax3.set_xticklabels([f'{b//1000}K' for b in blocks], rotation=45)
    ax3.grid(True, alpha=0.3, axis='y')
    
    for bar, mem in zip(bars, memory_usage):
        height = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2., height + 0.3,
                f'{mem:.1f}', ha='center', va='bottom', fontsize=8)
    
    # 5.4 Пропускная способность
    ax4 = axes[1, 1]
    x = np.arange(len(blocks))
    width = 0.35
    ax4.bar(x - width/2, encryption_throughput, width, label='Шифрование', color='blue', alpha=0.7)
    ax4.bar(x + width/2, decryption_throughput, width, label='Дешифрование', color='red', alpha=0.7)
    
    ax4.set_xlabel('Размер данных')
    ax4.set_ylabel('Пропускная способность (Мбит/сек)')
    ax4.set_title('Пропускная способность')
    ax4.set_xticks(x)
    ax4.set_xticklabels([f'{b//1000}K' for b in blocks], rotation=45)
    ax4.legend()
    ax4.grid(True, alpha=0.3, axis='y')
    
    plt.tight_layout()
    plt.savefig(f'{graphs_dir}/summary_plot.png', dpi=300)
    plt.close()
    
    print(f"✅ Графики сохранены в папку: {graphs_dir}")

def save_to_csv(data):
    """Сохраняет результаты в CSV файл"""
    benchmarks = data['benchmarks']
    csv_dir = "../../../results/crypto"
    ensure_directory(csv_dir)
    
    csv_file = f"{csv_dir}/seed_benchmark_results.csv"
    
    try:
        with open(csv_file, 'w', newline='', encoding='utf-8') as f:
            writer = csv.writer(f)
            writer.writerow(['blocks', 'data_size_mb', 
                            'encryption_time_ms', 'decryption_time_ms', 
                            'encryption_speed_kops', 'decryption_speed_kops',
                            'memory_mb', 'bytes_per_block',
                            'encryption_throughput_mbps', 'decryption_throughput_mbps'])
            
            for b in benchmarks:
                writer.writerow([
                    b['blocks_processed'],
                    b['data_size_mb'],
                    b['timing']['encryption_time_ms'],
                    b['timing']['decryption_time_ms'],
                    b['timing']['encryption_speed_ops_sec'] / 1000,
                    b['timing']['decryption_speed_ops_sec'] / 1000,
                    b['memory']['usage_mb'],
                    b['memory']['bytes_per_block'],
                    b['throughput']['encryption_mbps'],
                    b['throughput']['decryption_mbps']
                ])
        
        print(f"✅ CSV файл сохранен: {csv_file}")
        return True
    except Exception as e:
        print(f"❌ Ошибка при сохранении CSV: {e}")
        return False

def print_summary_statistics(data):
    """Выводит сводную статистику"""
    benchmarks = data['benchmarks']
    metadata = data['metadata']
    
    print("\n" + "="*70)
    print("📊 СВОДНАЯ СТАТИСТИКА BENCHMARK SEED")
    print("="*70)
    print(f"Алгоритм: SEED")
    print(f"Платформа: {metadata['platform']}")
    print(f"Дата теста: {metadata['timestamp']}")
    print(f"Размер блока: {metadata['block_size_bytes']} байт")
    print(f"Размер ключа: {metadata['key_size_bytes']} байт")
    print("-"*70)
    
    # Заголовок таблицы
    print(f"{'Размер':<8} {'Данные (МБ)':<12} {'Шифр (мс)':<12} {'Скорость (K/sec)':<16} {'Память (МБ)':<12} {'Throughput (Mbps)':<16}")
    print("-"*70)
    
    for b in benchmarks:
        blocks_k = b['blocks_processed'] // 1000
        data_mb = b['data_size_mb']
        enc_time = b['timing']['encryption_time_ms']
        enc_speed = b['timing']['encryption_speed_ops_sec'] / 1000
        memory = b['memory']['usage_mb']
        throughput = b['throughput']['encryption_mbps']
        
        print(f"{blocks_k:>4}K     {data_mb:>10.3f}     {enc_time:>10.1f}     {enc_speed:>14.1f}     {memory:>10.1f}     {throughput:>14.1f}")
    
    print("="*70)
    
    # Расчет средних значений
    avg_speed = np.mean([b['timing']['encryption_speed_ops_sec'] / 1000 for b in benchmarks])
    avg_throughput = np.mean([b['throughput']['encryption_mbps'] for b in benchmarks])
    
    print(f"\n📈 СРЕДНИЕ ПОКАЗАТЕЛИ:")
    print(f"  Средняя скорость шифрования: {avg_speed:.1f} тыс. блоков/сек")
    print(f"  Средняя пропускная способность: {avg_throughput:.1f} Мбит/сек")
    print("="*70)

def main():
    """Основная функция"""
    print("📈 SEED Benchmark Results Visualizer")
    print("-" * 40)
    
    # Загрузка данных
    data = load_benchmark_data()
    if data is None:
        sys.exit(1)
    
    # Создание графиков
    create_performance_plots(data)
    
    # Сохранение в CSV
    save_to_csv(data)
    
    # Вывод статистики
    print_summary_statistics(data)
    
    print("\n✅ Визуализация завершена успешно!")
    print("   Графики: results/crypto/graphs/")
    print("   CSV данные: results/crypto/seed_benchmark_results.csv")

if __name__ == "__main__":
    main()