
#include <iostream>
#include <cstring>

using namespace std;
  

int main()
{
    char line[100];
    cout << "문자열을 입력하세요: ";
    cin.getline(line, 100, '\n');

    int count = 0;
    int i = 0;
    while (line[i] != '\0') {
        if (line[i] != '\0') {
            count++;
        }
        i++;
    }
    cout << "총 글자수 " << i << ", 문자 e의 개수는 " << count << "개" << endl; 

}
