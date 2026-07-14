import pandas as pd
import numpy as np

# 샘플 데이터 생성 (3명의 수학, 영어 점수)
data = {
    '수학': [80, 90, 70],
    '영어': [95, 85, 90]
}
df = pd.DataFrame(data)

# 1. 각 과목별(열별) 평균 구하기 (기본값)
print(df.mean())
# 출력: 수학 80.0, 영어 90.0

# 2. 각 학생별(행별) 평균 구하기 (axis=1 설정)
print(df.mean(axis=1))

