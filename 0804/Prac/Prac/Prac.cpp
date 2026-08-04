#include <iostream>
using namespace std;
int adder(int a, int b) {
	int sum;
	sum = a + b;
	return sum;
}

int main() {
	
	int n = adder(12345, 23456);
	cout << 12345 << " + " << 23456 << " = " << n << endl;
	int a, b;
	cout << "Enter two numbers: ";
	cin >> a >> b;
	n = adder(a, b);
	cout << a << " + " << b << " = " << n << endl;
	cout << "Hello World!" << endl;


}
