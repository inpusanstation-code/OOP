#include<iostream>
using namespace std;

int main() {
	int a = 5, b = 10, c;
	cout << "int a = 5, b = 10, c;" << endl;
	
	c = a + b / 3 * 3;
	cout << "a+b/3*3 = " << c <<endl;

	c= b << 2;
	cout << "b << 2 = " << c << endl; 

	c = a != b;
	cout << "a != b = " << c << endl;

	c = b% a;
	cout << "b% a = " << c << endl;
	
	c= (a > b) ? a : b;
	cout << "(a > b) ? a : b = " << c << endl;

	c = sizeof(a);
	cout << "sizeof(a) = " << c << endl;

	c = a++;
	cout << "c = a++; = " << c << endl;

	c= a += b;
	cout << "a += b = " << c << endl;

	c = a & b;
	cout << "a & b = " << c << endl;

	c = (a + b, a - b);
	cout << "c = (a + b, a - b); = " << c << endl;
}