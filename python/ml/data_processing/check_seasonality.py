import pandas as pd
import matplotlib.pyplot as plt
from statsmodels.tsa.seasonal import seasonal_decompose

# Загружаем обработанные данные
ts_df = pd.read_csv('../../../data/processed/time_series.csv', parse_dates=['date'])
ts_df = ts_df.set_index('date')

print("=== АНАЛИЗ СЕЗОННОСТИ ===")
print(f"Размер данных: {len(ts_df)} дней")
print(f"Пропущенные значения: {ts_df['traffic'].isnull().sum()}")
print(f"Минимальный трафик: {ts_df['traffic'].min()}")
print(f"Максимальный трафик: {ts_df['traffic'].max()}")
print(f"Средний трафик: {ts_df['traffic'].mean():.2f}")

# Проверяем сезонность (недельная)
result = seasonal_decompose(ts_df['traffic'], model='additive', period=7)

# Визуализируем компоненты
fig, axes = plt.subplots(4, 1, figsize=(12, 10))
result.observed.plot(ax=axes[0], title='Исходный ряд')
result.trend.plot(ax=axes[1], title='Тренд')
result.seasonal.plot(ax=axes[2], title='Сезонность (недельная)')
result.resid.plot(ax=axes[3], title='Остатки')
plt.tight_layout()
plt.savefig('../../../results/ml/graphs/seasonal_decomposition.png', dpi=300)
plt.close()

print("График сезонности сохранен: results/ml/graphs/seasonal_decomposition.png")