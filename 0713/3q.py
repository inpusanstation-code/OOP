import numpy as np
import tensorflow as tf

# 원본 NumPy 배열 선언
array = np.array([
    [2, 4, 6],
    [8, 10, 12],
    [14, 16, 18]
])

# 3.1 Tensor로 변환
# tf.convert_to_tensor() 함수를 사용하여 NumPy 배열을 텐서로 변환합니다.
tensor = tf.convert_to_tensor(array)

# 3.2 shape 출력
# .shape 속성을 통해 텐서의 차원 크기(크기 구조)를 확인합니다.
tensor_shape = tensor.shape

# 3.3 dtype 출력
# .dtype 속성을 통해 텐서 내부 데이터의 타입(형태)을 확인합니다.
tensor_dtype = tensor.dtype

# 3.4 모든 값에 10을 더한 결과 출력
# 텐서에 상수를 더하면, 텐서플로우가 알아서 모든 원소에 연산을 적용(브로드캐스팅)합니다.
tensor_added = tensor + 10

# 결과 출력
print(f"3.1 변환된 텐서:\n{tensor}\n")
print(f"3.2 텐서의 구조 (shape): {tensor_shape}")
print(f"3.3 텐서의 데이터 타입 (dtype): {tensor_dtype}\n")
print(f"3.4 모든 값에 10을 더한 결과:\n{tensor_added}")