import matplotlib.pyplot as plt  # 일반적으로 pylab보다 pyplot을 더 많이 씁니다.
from sklearn import linear_model

# 1. 선형 회귀 모델 생성
reg = linear_model.LinearRegression()

# 2. 데이터 준비 (X: 키, y: 몸무게 예시 데이터로 보입니다)
X = [[174], [152], [138], [128], [186]]     # 학습 예제 (독립 변수)
y = [71, 55, 46, 38, 88]                    # 정답 (종속 변수)

# 3. 모델 학습
reg.fit(X, y)

print('회귀 계수(기울기):', reg.coef_)
print('회귀 절편:', reg.intercept_)

# ----------------------------------------------------------------
# 🛠️ 여기서부터 그래픽 시각화 코드입니다.
# ----------------------------------------------------------------

# ① 실제 데이터는 점(Scatter)으로 찍기 (색상은 파란색)
plt.scatter(X, y, color='blue', label='Actual Data')

# ② 모델이 예측한 값(회귀선) 구하기
# 학습했던 X를 그대로 모델에 넣어서 예측값(y_pred)을 뽑아냅니다.
y_pred = reg.predict(X)

# ③ 예측된 회귀선을 직선(Plot)으로 그리기 (색상은 빨간색)
plt.plot(X, y_pred, color='red', linewidth=2, label='Regression Line')

# ④ 그래프 이쁘게 꾸미기 (제목 및 축 이름)
plt.title('Linear Regression Result')
plt.xlabel('X (e.g., Height)')
plt.ylabel('y (e.g., Weight)')
plt.legend() # 범례 표시 (Actual Data, Regression Line 텍스트 박스)
plt.grid(True) # 배경에 격자 눈금 추가

# ⑤ 화면에 그래프 띄우기
plt.show()