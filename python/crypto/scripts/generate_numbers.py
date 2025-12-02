import random
import csv
import os

def generate_numbers_to_path(output_path, count=1000000):
    """
    Генерирует 1 млн чисел и сохраняет по указанному пути
    """
    # Создаем директорию, если её нет
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    # Фиксируем seed для воспроизводимости
    random.seed(42)
    
    print(f"Генерация {count} чисел...")
    print(f"Файл будет сохранен: {output_path}")
    
    with open(output_path, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        
        # Записываем заголовок
        writer.writerow(['number'])
        
        # Генерируем и записываем числа
        for i in range(count):
            # Генерируем числа в диапазоне, подходящем для 32-битных int
            num = random.randint(0, 2147483647)  # от 0 до 2^31-1
            writer.writerow([num])
            
            # Прогресс (каждые 100к чисел)
            if i > 0 and i % 100000 == 0:
                print(f"  Сгенерировано: {i}")
    
    print(f"✓ Готово! Создан файл: {output_path}")
    print(f"  Размер файла: {os.path.getsize(output_path) / (1024*1024):.2f} MB")
    
    # Показываем первые 5 чисел для проверки
    print("\nПервые 5 чисел из файла:")
    with open(output_path, 'r') as f:
        lines = f.readlines()[:6]
        for line in lines[1:6]:
            print(f"  {line.strip()}")

if __name__ == "__main__":
    # Абсолютный путь к файлу
    # Из папки python/crypto/scripts в data/processed/1mln.csv
    output_file = "../../../data/processed/1mln.csv"  # Поднимаемся на 3 уровня вверх
    
    # Или можно указать абсолютный путь (замените на свой)
    # output_file = "/home/user/project/data/processed/1mln.csv"
    
    # Проверяем путь
    print(f"Текущая рабочая директория: {os.getcwd()}")
    print(f"Путь для сохранения: {output_file}")
    
    # Генерируем данные
    generate_numbers_to_path(output_file, 1000000)