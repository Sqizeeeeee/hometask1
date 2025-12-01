import json
import matplotlib.pyplot as plt
import numpy as np
import os

# Создаем папку для графиков
os.makedirs('../../results/graphs', exist_ok=True)

def plot_time_complexity():
    """Строит график временной сложности"""
    print("Построение графика временной сложности...")
    
    # Загружаем данные из JSON
    with open('../../results/time_complexity.json', 'r') as f:
        data = json.load(f)
    
    sizes = data['time_complexity']['data_sizes']
    train_times = data['time_complexity']['training_times_ms']
    pred_times = data['time_complexity']['prediction_times_ms']
    
    # Создаем график
    plt.figure(figsize=(12, 5))
    
    # График времени обучения
    plt.subplot(1, 2, 1)
    plt.plot(sizes, train_times, 'bo-', linewidth=2, markersize=8, label='Измерения')
    
    # Линейная аппроксимация O(n)
    z = np.polyfit(sizes, train_times, 1)
    p = np.poly1d(z)
    plt.plot(sizes, p(sizes), 'r--', linewidth=1, label=f'Линейная аппроксимация')
    
    plt.xlabel('Размер данных (n)')
    plt.ylabel('Время обучения (мс)')
    plt.title('Временная сложность обучения\nHolt-Winters')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # График времени прогноза
    plt.subplot(1, 2, 2)
    plt.plot(sizes, pred_times, 'go-', linewidth=2, markersize=8, label='Измерения')
    
    # Линейная аппроксимация O(n)
    z_pred = np.polyfit(sizes, pred_times, 1)
    p_pred = np.poly1d(z_pred)
    plt.plot(sizes, p_pred(sizes), 'r--', linewidth=1, label=f'Линейная аппроксимация')
    
    plt.xlabel('Размер данных (n)')
    plt.ylabel('Время прогноза (мс)')
    plt.title('Временная сложность прогноза\nHolt-Winters')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('../../results/graphs/time_complexity.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    print(f"Среднее время обучения на точку: {z[0]:.4f} мс/точку")
    print(f"Среднее время прогноза на точку: {z_pred[0]:.4f} мс/точку")

def plot_memory_complexity():
    """Строит график использования памяти"""
    print("\nПостроение графика использования памяти...")
    
    # Загружаем данные из JSON
    with open('../../results/memory_complexity.json', 'r') as f:
        data = json.load(f)
    
    sizes = data['memory_complexity']['data_sizes']
    memory_kb = data['memory_complexity']['memory_kb']
    
    # Создаем график
    plt.figure(figsize=(8, 6))
    
    plt.plot(sizes, memory_kb, 'mo-', linewidth=2, markersize=8, label='Теоретическая оценка')
    
    # Линейная аппроксимация O(n)
    z = np.polyfit(sizes, memory_kb, 1)
    p = np.poly1d(z)
    plt.plot(sizes, p(sizes), 'r--', linewidth=1, label=f'Линейная аппроксимация')
    
    plt.xlabel('Размер данных (n)')
    plt.ylabel('Использование памяти (КБ)')
    plt.title('Пространственная сложность Holt-Winters\nO(n)')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('../../results/graphs/memory_complexity.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    print(f"Среднее использование памяти: {z[0]:.4f} КБ/точку")

def create_complexity_summary():
    """Создает сводку по сложности алгоритма"""
    print("\n=== СВОДКА ПО СЛОЖНОСТИ АЛГОРИТМА ===")
    
    # Загружаем данные
    with open('../../results/time_complexity.json', 'r') as f:
        time_data = json.load(f)
    
    with open('../../results/memory_complexity.json', 'r') as f:
        memory_data = json.load(f)
    
    sizes = time_data['time_complexity']['data_sizes']
    train_times = time_data['time_complexity']['training_times_ms']
    pred_times = time_data['time_complexity']['prediction_times_ms']
    memory_kb = memory_data['memory_complexity']['memory_kb']
    
    # Аппроксимации
    z_train = np.polyfit(sizes, train_times, 1)
    z_pred = np.polyfit(sizes, pred_times, 1)
    z_mem = np.polyfit(sizes, memory_kb, 1)
    
    # Сохраняем сводку в JSON
    summary = {
        "algorithm_complexity": {
            "algorithm": "Triple Exponential Smoothing (Holt-Winters)",
            "time_complexity": {
                "training": "O(n)",
                "prediction": "O(n)",
                "average_training_time_per_point_ms": round(z_train[0], 4),
                "average_prediction_time_per_point_ms": round(z_pred[0], 4)
            },
            "space_complexity": {
                "complexity": "O(n)",
                "average_memory_per_point_kb": round(z_mem[0], 4),
                "constant_factors": {
                    "seasonal_components": 7,
                    "level_trend_components": 2
                }
            },
            "conclusion": "Алгоритм имеет линейную сложность O(n) по времени и памяти, что делает его эффективным для больших временных рядов"
        }
    }
    
    with open('../../results/complexity_summary.json', 'w', encoding='utf-8') as f:
        json.dump(summary, f, indent=2, ensure_ascii=False)
    
    print("✅ Сводка сохранена в results/complexity_summary.json")

if __name__ == "__main__":
    print("=== ВИЗУАЛИЗАЦИЯ ПРОИЗВОДИТЕЛЬНОСТИ HOLT-WINTERS ===")
    
    plot_time_complexity()
    plot_memory_complexity()
    create_complexity_summary()
    
    print("\n=== ГРАФИКИ СОЗДАНЫ ===")
    print("📊 time_complexity.png - временная сложность")
    print("📊 memory_complexity.png - пространственная сложность")
    print("📄 complexity_summary.json - сводка по сложности")