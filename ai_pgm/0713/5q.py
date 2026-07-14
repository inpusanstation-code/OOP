import tensorflow as tf
from tensorflow.keras import layers, models

# 5.1~5.5의 모든 요소를 TensorFlow 하나로 구현한 코드입니다.
model = models.Sequential([
    # 5.1 Conv2D 역할: 이미지의 시각적 특징 추출 (필터 32개)
    layers.Conv2D(32, (3, 3), activation='relu', input_shape=(28, 28, 1)),
    
    # 5.2 MaxPooling 역할: 특징 맵의 크기를 줄여 연산량 감소 및 위치 변화 극복
    layers.MaxPooling2D((2, 2)),
    
    # 추가 Conv2D: 더 복잡한 특징 추출 (필터 64개)
    layers.Conv2D(64, (3, 3), activation='relu'),
    
    # 5.3 Flatten 역할: 2D/3D 다차원 이미지를 1차원 벡터로 평평하게 펼침
    layers.Flatten(),
    
    # 5.4 Dense 역할: 펼쳐진 특징들을 결합하여 분류를 수행하는 일반 신경망 (노드 128개)
    layers.Dense(128, activation='relu'),
    
    # 5.5 Softmax 사용하는 이유: 최종 10개 클래스의 예측 결과를 
    # 합이 1(100%)이 되는 '확률'로 변환하여 출력 (노드 10개)
    layers.Dense(10, activation='softmax')
])

# 모델의 전체적인 구조를 한눈에 보여줍니다.
model.summary()