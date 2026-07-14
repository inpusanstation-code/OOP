from sklearn.datasets import load_iris  # scikit-learn에서 붓꽃 데이터셋을 불러오는 모듈
import pandas as pd  # 데이터를 표 형태로 다루기 위한 pandas 모듈
from sklearn import svm  # 분류 모델 중 SVM(지원 벡터 머신) 클래스
import plotly.express as px  # 데이터 시각화를 위한 plotly 모듈


iris = load_iris()  # 붓꽃 데이터셋을 로드한다. iris 객체에는 데이터와 레이블 정보가 들어 있다.
df = pd.DataFrame(data=iris.data)  # iris.data를 DataFrame으로 변환해 표 형태로 확인할 수 있게 만든다.
s = svm.SVC(gamma=0.1, C=1.0, kernel='linear')  # SVM 분류기 모델을 생성한다.
s.fit(iris.data, iris.target)  # 학습 데이터와 정답 레이블을 사용해 모델을 학습시킨다.

new_data = [[5.0, 3.5, 1.5, 0.2], [6.0, 4.0, 2.0, 0.3]]  # 새로운 데이터 포인트를 정의한다.

res=s.predict(new_data)  # 학습된 모델을 사용해 새로운 데이터 포인트의 레이블을 예측한다.

#print("입력데이터:" + str(iris.data))  # 입력 데이터(꽃받침 길이, 너비, 꽃잎 길이, 너비)를 출력한다.
#print("정답 레이블:" + str(iris.target))  # 각 샘플의 정답 레이블(0, 1, 2)을 출력한다.
#print("품종 이름:" + str(iris.target_names))  # 레이블 번호에 해당하는 품종 이름을 출력한다.
#print("특성 이름:" + str(iris.feature_names))  # 각 특성의 이름을 출력한다.
#print("데이터셋 설명:" + str(iris.DESCR))  # 데이터셋에 대한 설명 문서를 출력한다.
print("새로운 2개 샘플의 부류는:" + str(res))  # 새로운 데이터 포인트에 대한 예측 결과를 출력한다.

df = px.data.iris()
# petal_length를 제외하여 3차원 공간 구성
fig = px.scatter_3d(df, x='sepal_length', y='sepal_width', z='petal_width', color='species')
fig.show(renderer="browser")


