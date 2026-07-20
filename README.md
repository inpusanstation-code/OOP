# 🧠 Python으로 배우는 AI 딥러닝

> 파이썬을 활용한 딥러닝 개념 정리 및 실습 레포지토리입니다.  
> 기초부터 실전 프로젝트까지 단계별로 학습할 수 있도록 구성했습니다.

![Python](https://img.shields.io/badge/Python-3.10%2B-blue?logo=python)
![TensorFlow](https://img.shields.io/badge/TensorFlow-2.x-orange?logo=tensorflow)
![PyTorch](https://img.shields.io/badge/PyTorch-2.x-red?logo=pytorch)
![License](https://img.shields.io/badge/License-MIT-green)

---

## 📚 목차

1. [프로젝트 소개](#-프로젝트-소개)
2. [학습 로드맵](#-학습-로드맵)
3. [폴더 구조](#-폴더-구조)
4. [환경 설정](#-환경-설정)
5. [주요 내용](#-주요-내용)
6. [실습 프로젝트](#-실습-프로젝트)
7. [참고 자료](#-참고-자료)

---

## 🎯 프로젝트 소개

이 레포지토리는 **딥러닝을 처음 접하는 분**부터 **실전 프로젝트를 구현하고 싶은 분**까지 단계적으로 학습할 수 있도록 구성된 스터디 자료입니다.

- ✅ 개념 설명 + 코드 실습 병행
- ✅ Jupyter Notebook 기반 실습
- ✅ TensorFlow / PyTorch 양쪽 예제 제공
- ✅ 실전 데이터셋 활용 프로젝트 포함

---

## 🗺️ 학습 로드맵

```
📌 1단계: Python & 수학 기초
    └── NumPy, Pandas, Matplotlib
    └── 선형대수, 미적분, 확률/통계

📌 2단계: 머신러닝 기초
    └── 지도학습 / 비지도학습
    └── Scikit-learn 실습

📌 3단계: 딥러닝 기초
    └── 퍼셉트론, 역전파
    └── 활성화 함수, 손실 함수, 옵티마이저

📌 4단계: 주요 아키텍처
    └── CNN (이미지 분류)
    └── RNN / LSTM (시계열, 자연어)
    └── Transformer / Attention

📌 5단계: 실전 프로젝트
    └── 이미지 분류기
    └── 감성 분석
    └── 생성 모델 (GAN, VAE)
```


## ⚙️ 환경 설정

### 1. 레포지토리 클론

```bash
git clone https://github.com/your-username/python-deeplearning-study.git
cd python-deeplearning-study
```

### 2. 가상환경 생성 및 활성화

```bash
# venv 사용
python -m venv venv
source venv/bin/activate        # macOS/Linux
venv\Scripts\activate           # Windows

# 또는 conda 사용
conda create -n dl-study python=3.10
conda activate dl-study
```

### 3. 패키지 설치

```bash
pip install -r requirements.txt
```

### 4. Jupyter Notebook 실행

```bash
jupyter notebook
```

### 주요 라이브러리

| 라이브러리 | 버전 | 용도 |
|---|---|---|
| numpy | 1.26+ | 수치 연산 |
| pandas | 2.0+ | 데이터 처리 |
| matplotlib / seaborn | latest | 시각화 |
| scikit-learn | 1.4+ | 머신러닝 |
| tensorflow | 2.15+ | 딥러닝 프레임워크 |
| torch | 2.2+ | 딥러닝 프레임워크 |
| transformers | 4.38+ | NLP / Transformer 모델 |

---

## 📖 주요 내용

### 1️⃣ 딥러닝 기초

- 신경망의 구조: 입력층 → 은닉층 → 출력층
- 순전파(Forward Propagation)와 역전파(Backpropagation)
- 주요 활성화 함수: ReLU, Sigmoid, Softmax
- 손실 함수: MSE, Cross-Entropy
- 옵티마이저: SGD, Adam, RMSProp

### 2️⃣ CNN (합성곱 신경망)

- 합성곱 연산, 풀링, 패딩의 개념
- MNIST 손글씨 분류 실습
- CIFAR-10 이미지 분류 실습
- 전이학습(Transfer Learning): ResNet, VGG 활용

### 3️⃣ RNN / LSTM

- 순환 신경망의 작동 원리
- 장기 의존성 문제와 LSTM의 해결 방법
- 텍스트 감성 분석 실습

### 4️⃣ Transformer

- Self-Attention 메커니즘 이해
- BERT를 활용한 파인튜닝
- HuggingFace Transformers 라이브러리 활용법

---

## 🚀 실습 프로젝트

### Project 1. 이미지 분류기
- 데이터셋: CIFAR-10
- 모델: CNN + 전이학습 (ResNet18)
- 목표 정확도: 90%+

### Project 2. 감성 분석
- 데이터셋: IMDB Movie Reviews
- 모델: LSTM / BERT Fine-tuning
- 평가 지표: Accuracy, F1-Score

### Project 3. 손글씨 생성 (GAN)
- 데이터셋: MNIST
- 모델: DCGAN
- 목표: 실제같은 손글씨 이미지 생성

---

## 📎 참고 자료

**도서**
- 밑바닥부터 시작하는 딥러닝 (사이토 고키)
- 케라스 창시자에게 배우는 딥러닝 (프랑소와 숄레)
- PyTorch로 배우는 자연어 처리 (Delip Rao)

**온라인 강의**
- [모두를 위한 딥러닝 - 김성훈 교수](https://hunkim.github.io/ml/)
- [CS231n - Stanford (CNN)](http://cs231n.stanford.edu/)
- [Fast.ai 실전 딥러닝](https://www.fast.ai/)

**공식 문서**
- [TensorFlow Docs](https://www.tensorflow.org/learn)
- [PyTorch Tutorials](https://pytorch.org/tutorials/)
- [HuggingFace Docs](https://huggingface.co/docs)

---

## 🤝 기여 방법

1. 이 레포지토리를 Fork 합니다
2. 새 브랜치를 생성합니다 (`git checkout -b feature/새기능`)
3. 변경 사항을 커밋합니다 (`git commit -m 'Add: 새로운 실습 추가'`)
4. 브랜치에 Push 합니다 (`git push origin feature/새기능`)
5. Pull Request를 생성합니다

---

## 📄 라이선스

이 프로젝트는 [MIT License](LICENSE)를 따릅니다.

---

<p align="center">
  ⭐ 도움이 되셨다면 Star를 눌러주세요! ⭐
</p>
