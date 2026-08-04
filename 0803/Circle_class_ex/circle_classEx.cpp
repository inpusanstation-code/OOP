#include <iostream>
#include <windows.h>
#include <fcntl.h>
#include <io.h>
using namespace std;

class Circle {
public :
	int radius;
	double getArea();

};

double Circle::getArea() {
	return 3.141592 * radius * radius;
}

void enableUtf16Console() {
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	_setmode(_fileno(stdout), _O_U16TEXT);
}

int main() {
	enableUtf16Console();

	Circle donut; 
	donut.radius = 1;
	double area = donut.getArea();
	wcout << L"도넛의 면적: " << area << L"\n";

	Circle pizza; 
	pizza.radius = 5;
	area = pizza.getArea();
	wcout << L"피자의 면적: " << area << L"\n";

}
