import matplotlib.pyplot as plt

x = [1, 2, 3, 4]
y = [10, 20, 25, 30]

# 1. 도화지(fig)와 축(ax)을 명시적으로 분리하여 생성
# fig, ax = plt.subplots(figsize=(6, 4))

# 가로로 1행, 세로로 2열 ➡️ 총 2개의 방을 나란히 만들어라!
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 4))

# 2. ax라는 축 객체 위에 그래프를 그림
ax1.plot(x, y, color='green', linestyle='--')
ax2.plot(x, y, color='blue', linestyle='-.')
ax1.set_title("First Subplot")
ax2.set_title("Second Subplot")
ax1.set_xlabel("X-axis")
ax2.set_xlabel("X-axis")
ax1.set_ylabel("Y-axis")
ax2.set_ylabel("Y-axis")
ax1.grid(True)
ax2.grid(True)

plt.show()