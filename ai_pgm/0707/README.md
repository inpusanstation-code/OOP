# Python Project 0707

이 저장소는 Python 기초 문법, 머신러닝, 데이터 시각화, 텍스트/음성 생성 예제를 함께 담은 학습용 프로젝트입니다.

## 프로젝트 구성

- [scikit_learn.py](scikit_learn.py): 선형 회귀 모델을 학습하고 그래프로 시각화하는 예제입니다.
- [iris_sklearn.py](iris_sklearn.py): scikit-learn의 붓꽃 데이터셋을 이용해 SVM 분류 모델을 학습하고 예측하는 예제입니다.
- [newsGeneration.py](newsGeneration.py): 경기 정보를 입력받아 뉴스 기사 문장을 생성하고, 음성 파일까지 만들어 재생하는 예제입니다.
- [news_Son.mp3](news_Son.mp3): 뉴스 생성 예제에서 만들어진 음성 파일입니다.
- [보조자료](보조자료) 및 [보조자료2](보조자료2): 학습 자료 폴더입니다.
- [파이썬-Express_학생용211105](파이썬-Express_학생용211105): 파이썬 Express 학습 자료와 예제 소스가 들어 있습니다.

## 실행 전 준비

다음 패키지가 필요할 수 있습니다.

```bash
pip install matplotlib scikit-learn pandas plotly gtts playsound
```

> `newsGeneration.py`는 `gTTS`를 사용하므로 인터넷 연결이 필요할 수 있습니다.

## 파일별 설명

### 1. scikit_learn.py

- `LinearRegression` 모델을 생성합니다.
- 키와 몸무게 예시 데이터를 학습 데이터로 사용합니다.
- 학습 후 회귀 계수와 절편을 출력합니다.
- `matplotlib`을 사용해 실제 데이터와 회귀선을 그래프로 표시합니다.

### 2. iris_sklearn.py

- `load_iris()`로 붓꽃 데이터셋을 불러옵니다.
- `iris.data`는 입력 특징값이고, `iris.target`은 정답 라벨입니다.
- `SVC` 모델을 이용해 분류를 학습합니다.
- 새로운 입력 데이터에 대해 예측 결과를 출력합니다.
- `plotly.express`를 사용해 3차원 산점도 그래프를 표시합니다.

### 3. newsGeneration.py

- 사용자에게 경기 장소, 시간, 상대 팀, 골 수, 도움 수를 입력받습니다.
- 입력값을 바탕으로 뉴스 기사 문장을 자동 생성합니다.
- 생성된 기사를 콘솔에 출력합니다.
- `gTTS`로 한국어 음성 파일을 만들고 `playsound`로 재생합니다.

## 실행 방법

### 선형 회귀 예제

```bash
python scikit_learn.py
```

### 붓꽃 분류 예제

```bash
python iris_sklearn.py
```

### 뉴스 생성 예제

```bash
python newsGeneration.py
```

## 참고 사항

- `iris_sklearn.py`는 Plotly 그래프를 브라우저로 열어 보여주므로 실행 환경에 따라 브라우저가 열릴 수 있습니다.
- `newsGeneration.py`에서 `os`를 사용하고 있으므로, 파일 상단에 `import os`가 필요합니다. 현재 코드에서는 누락되어 있어 실행 시 확인이 필요합니다.
- 이 프로젝트는 학습용 예제로서, 각 스크립트가 독립적으로 동작하도록 구성되어 있습니다.
