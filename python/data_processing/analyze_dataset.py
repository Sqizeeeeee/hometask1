import pandas as pd
import matplotlib.pyplot as plt
import os
from datetime import datetime, timedelta

# Создаем папку для результатов
os.makedirs('../../results/graphs', exist_ok=True)

# Читаем данные
df = pd.read_csv('../../data/processed/selected_page.csv', header=None)

# Первый элемент - название страницы, остальные - данные по дням
page_name = df.iloc[0, 0]
traffic_data = df.iloc[0, 1:].values

# Создаем даты (начиная с 2018-01-01)
start_date = datetime(2018, 1, 1)
dates = [start_date + timedelta(days=i) for i in range(len(traffic_data))]

# Создаем DataFrame
ts_df = pd.DataFrame({
    'date': dates,
    'traffic': traffic_data
})

# Переименовываем страницу для удобства
page_name_english = "Back to the Future (Japanese Wikipedia)"
ts_df['page_name'] = page_name_english

# Сохраняем обработанные данные
ts_df.to_csv('../../data/processed/time_series.csv', index=False)

# Создаем график
plt.figure(figsize=(12, 6))
plt.plot(ts_df['date'], ts_df['traffic'])
plt.title(f'Website Traffic: {page_name_english}')
plt.xlabel('Date')
plt.ylabel('Daily Page Views')
plt.xticks(rotation=45)
plt.tight_layout()
plt.savefig('../../results/graphs/traffic_overview.png', dpi=300, bbox_inches='tight')
plt.close()

print("=== ДАННЫЕ ПОДГОТОВЛЕНЫ ===")
print(f"Страница: {page_name_english}")
print(f"Период: {ts_df['date'].min().strftime('%Y-%m-%d')} до {ts_df['date'].max().strftime('%Y-%m-%d')}")
print(f"Дней данных: {len(ts_df)}")
print(f"Файл данных: data/processed/time_series.csv")
print(f"График: results/graphs/traffic_overview.png")