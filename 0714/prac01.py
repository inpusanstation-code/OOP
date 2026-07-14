import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# -------------------------------------------------------------
# 1. NumPy를 활용한 가상 데이터 생성 (Data Generation)
# -------------------------------------------------------------
# 실험 결과의 일관성을 위해 랜덤 시드 고정
np.random.seed(42)

# 2026년 1월 1일부터 100일간의 날짜 배열 생성 (Pandas 기능 활용)
date_range = pd.date_range(start='2026-01-01', periods=100)

# NumPy의 정규분포 랜덤 함수를 이용해 100일간의 일일 수익률 생성 (평균 0.1%, 표준편차 2%)
daily_returns = np.random.normal(loc=0.001, scale=0.02, size=100)

# 초기 가격 1000에서 시작하여 누적 곱(cumprod)으로 가상 주가 흐름 계산
# (1 + 수익률)을 계속 곱해나가는 방식입니다.
price_movement = 1000 * np.cumprod(1 + daily_returns)


# -------------------------------------------------------------
# 2. Pandas를 활용한 데이터 분석 및 가공 (Data Analysis)
# -------------------------------------------------------------
# 생성한 데이터로 판다스 데이터프레임(DataFrame) 생성
df = pd.DataFrame({
    'Close': price_movement,
    'Return': daily_returns
}, index=date_range)

# [Pandas 핵심 기능 1] 이동평균선(Moving Average) 계산
# 최근 5일간의 주가 평균을 구해 'MA5' 컬럼에 추가 (데이터 부드럽게 만들기)
df['MA5'] = df['Close'].rolling(window=5).mean()

# [Pandas 핵심 기능 2] 데이터 필터링 (조건 검색)
# 수익률이 3%(0.03) 이상 폭등한 날만 골라내기
boom_days = df[df['Return'] >= 0.03]

print("=== 상위 5개 데이터 확인 ===")
print(df.head()) # 데이터프레임의 앞 5줄 출력

print(f"\n💡 3% 이상 폭등한 날의 수: {len(boom_days)}일")


# -------------------------------------------------------------
# 3. Matplotlib를 활용한 데이터 시각화 (Data Visualization)
# -------------------------------------------------------------
# 그래프 크기 설정 (가로 12인치, 세로 6인치)
plt.figure(figsize=(12, 6))

# [라인 플롯] 실제 종가(Close)와 5일 이동평균선(MA5) 그리기
plt.plot(df.index, df['Close'], label='Close Price', color='blue', linewidth=1.5)
plt.plot(df.index, df['MA5'], label='5-Day Moving Average (MA5)', color='orange', linestyle='--')

# [산점도 플롯] 폭등한 날만 차트 위에 빨간 점으로 강조 표시
plt.scatter(boom_days.index, boom_days['Close'], color='red', label='Boom Day (Return >= 3%)', zorder=5)

# 차트 데코레이션 (제목, 축 이름, 그리드, 범례)
plt.title('Virtual Stock Price Movement & Analysis (2026)', fontsize=16, fontweight='bold')
plt.xlabel('Date', fontsize=12)
plt.ylabel('Price', fontsize=12)
plt.grid(True, linestyle=':', alpha=0.6) # 점선 그리드 추가
plt.legend(loc='upper left') # 범례 표시 위치

# 그래프 출력
plt.show()