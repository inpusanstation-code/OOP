import numpy as np
import pandas as pd

# 소수점 출력을 첫째 자리까지만 나오게 설정합니다.
pd.options.display.float_format = '{:.1f}'.format

# 원본 NumPy 2차원 배열 선언
data = np.array([
    [85, 90, 88],
    [70, 80, 75],
    [95, 98, 100],
    [60, 72, 68]
])

# 2.1 & 2.2 DataFrame 변환 및 열 이름(국어, 영어, 수학) 지정
# pd.DataFrame의 columns 매개변수를 사용하여 
# 생성과 동시에 열 이름을 지정합니다.
df = pd.DataFrame(data, columns=['국어', '영어', '수학'])

# 2.3 학생별 평균을 새로운 열에 추가
# df.mean(axis=1)을 통해 가로(행) 방향으로 평균을 구한 뒤, 
# '평균'이라는 새 열에 대입합니다.
df['평균'] = df.mean(axis=1)

# 2.4 평균이 가장 높은 학생 출력
# df['평균'].idxmax()는 평균 열에서 가장 큰 값을 가진 
# '행의 인덱스(번호)'를 반환합니다.
# df.loc[인덱스]를 사용해 해당 학생의 모든 데이터
# (국어, 영어, 수학, 평균 점수)를 보기 좋게 출력합니다.
best_student_index = df['평균'].idxmax()
best_student = df.loc[best_student_index]

# 결과 출력
print("=== 2.1 ~ 2.3 결과 (DataFrame) ===")
print(df)
print("\n" + "="*40)
print(f"2.4 평균이 가장 높은 학생 (인덱스 {best_student_index}번 학생):")
print(best_student)