#include <iostream>
#include <string>

using namespace std;

// Student 클래스 정의
class Student {
public:
    string name;   // 이름 변수
    int stuNum;    // 학번 변수

    // 입력과 출력을 담당하는 멤버 함수
    void inputAndPrint() {
        cout << "이름과 학번을 입력하세요: ";
        // cin 연산자는 '>>'를 연달아 사용해야 합니다.
        cin >> name >> stuNum;

        cout << "\n=== 입력 결과 ===" << endl;
        cout << "이름: " << name << endl;
        cout << "학번: " << stuNum << endl;
    }
};

int main() {
    Student student;         // Student 객체 생성
    student.inputAndPrint(); // 멤버 함수 호출

    return 0;
}