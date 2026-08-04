#include <iostream>
#include <Python.h>
using namespace std;

int main() {
	int i, a, b, sum = 0;
	cout << "두 개의 정수 입력>>";
	cin >> a >> b;

	for (i = a; i <= b; i++) { // a에서 b까지 합 계산  // for 문으로 연산
		sum += i;
	}

	cout << a << "에서 " << b << "까지 합은 " << sum;



	i = a; // 1. 초기화 (while문 시작 전)		// while 문으로 전환
	while (i <= b) { // 2. 조건식 (i가 b 이하일 때 동안)
		sum += i;
		i++; // 3. 증감식 (반복문 내부 끝)
	}



	Py_Initialize();

    cout << "=== C++ 프로그램 시작 ===" << endl;

    // 2. C++ 문자열 안에 파이썬 코드를 작성하여 직접 실행
    // R"( ... )"는 C++의 Raw String Literal로, 따옴표나 줄바꿈을 자유롭게 쓸 수 있게 해줍니다.
    const char* python_code = R"(
	a, b = map(int, input("두 개의 정수 입력>> ").split())

	total_sum = 0
	for i in range(a, b + 1):
		total_sum += i

	print(f"[Python 영역 실행] {a}에서 {b}까지 합은 {total_sum}")
	)";

    // 3. 작성한 파이썬 코드를 C++ 내에서 실행
    PyRun_SimpleString(python_code);

    cout << "=== C++ 프로그램 종료 ===" << endl;

    // 4. Python 인터프리터 종료 및 메모리 해제
    Py_Finalize();




}