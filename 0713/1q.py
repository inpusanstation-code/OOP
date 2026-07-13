# 원본 학생들의 점수 리스트를 선언합니다.
scores = [78, 85, 92, 68, 95, 88, 74]

# 1.1 평균 점수 계산
# sum(scores)로 모든 점수의 합을 구하고, 
# len(scores)로 학생 수(원소 개수)를 나눠 평균을 계산합니다.
average_score = sum(scores) / len(scores)

# 소수점 둘째 자리까지 포맷팅하여 출력합니다.
print(f"1.1 평균 점수: {average_score:.2f}점")  

# 1.2 최고점과 최저점 출력
# max() 함수를 사용해 리스트 내의 가장 큰 값을 찾습니다.
highest_score = max(scores)

# min() 함수를 사용해 리스트 내의 가장 작은 값을 찾습니다.
lowest_score = min(scores)
print(f"1.2 최고점: {highest_score}점, 최저점: {lowest_score}점")

# 1.3 80점 이상만 새로운 리스트로 생성
# 리스트 컴프리헨션을 사용해 scores 리스트를 순회하며(score) 
# 조건(score >= 80)에 맞는 데이터만 필터링하여 새 리스트를 만듭니다.
above_80_scores = [score for score in scores if score >= 80]
print(f"1.3 80점 이상 점수 리스트: {above_80_scores}")

# 1.4 평균 이상인 학생 수 출력
# 리스트 컴프리헨션으로 평균(average_score) 이상인 점수들만 모은 뒤, 
# len() 함수로 그 개수를 세어 학생 수를 구합니다.
above_average_count = len([
    score for score in scores 
    if score >= average_score
])
print(f"1.4 평균 이상인 학생 수: {above_average_count}명")