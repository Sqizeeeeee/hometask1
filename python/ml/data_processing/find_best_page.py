import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Читаем полный датасет (только первые 1000 строк для скорости)
df_sample = pd.read_csv('../../data/raw/Wiki_Page_views.csv', nrows=1000)

def analyze_page(row):
    """Анализирует ряд на пригодность для прогнозирования"""
    page_name = row[0]
    traffic = row[1:].astype(float)
    
    # Убираем NaN и бесконечные значения
    traffic = traffic[np.isfinite(traffic)]
    
    if len(traffic) < 700:  # Минимум 700 дней данных
        return None
    
    # Критерии качества:
    mean_traffic = traffic.mean()
    std_traffic = traffic.std()
    cv = std_traffic / mean_traffic  # Коэффициент вариации
    
    # Ищем стабильные但不是 постоянные данные
    if mean_traffic > 100 and cv < 1.0:  # Не слишком изменчивые
        return {
            'page': page_name,
            'mean': mean_traffic,
            'std': std_traffic,
            'cv': cv,
            'min': traffic.min(),
            'max': traffic.max()
        }
    return None

print("Поиск лучшей страницы...")
good_pages = []

for idx, row in df_sample.iterrows():
    if idx == 0:  # Пропускаем заголовок
        continue
        
    result = analyze_page(row)
    if result:
        good_pages.append(result)

# Сортируем по стабильности (низкий CV)
good_pages.sort(key=lambda x: x['cv'])

print("\nТоп-5 лучших страниц:")
for i, page in enumerate(good_pages[:5]):
    print(f"{i+1}. {page['page']}")
    print(f"   Среднее: {page['mean']:.1f}, CV: {page['cv']:.3f}")
    print(f"   Диапазон: {page['min']:.0f}-{page['max']:.0f}\n")