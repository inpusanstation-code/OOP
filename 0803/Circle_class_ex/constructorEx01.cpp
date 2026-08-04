#include <iostream>
#include <windows.h>

using namespace std;

class MyRectangle {
public:
    int width, height;
    MyRectangle();
    MyRectangle(int w, int h);
    MyRectangle(int length);
    bool isSquare();
};

MyRectangle::MyRectangle() {
    width = height = 1;
}

MyRectangle::MyRectangle(int w, int h) {
    width = w;
    height = h;
}

MyRectangle::MyRectangle(int length) {
    width = height = length;
}

bool MyRectangle::isSquare() {
    return width == height;
}

int main() {

    SetConsoleOutputCP(65001);
    
    MyRectangle rect1;
    MyRectangle rect2(2, 3);
    MyRectangle rect3(4);

    if (rect1.isSquare()) {
        cout << "정사각형입니다." << endl;
    }

    if (rect2.isSquare()) {
        cout << "rect2 is a square." << endl;
    }

    if (rect3.isSquare()) {
        cout << "rect3 is a square." << endl;
    }

    return 0;
}