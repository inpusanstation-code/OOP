#include <iostream>
using namespace std;

int bigger(int a, int b) {
	return (a > b) ? a : b;
}

bool isEven(int n) {
	return (n % 2 == 0);
}

bool dividedby3(int n) {
	return (n % 3 == 0);
}

int main() {
	int x, y;
	cout << "Enter two integers: ";
	cin >> x >> y;
	cout << "The bigger number is: " << bigger(x, y) << endl;
	if (isEven(x)) {
		cout << x << " is even." << endl;
	}
	else {
		cout << x << " is odd." << endl;
	}
	if (isEven(y)) {
		cout << y << " is even." << endl;
	}
	else {
		cout << y << " is odd." << endl;
	}
		
	if (dividedby3(x)) {
		cout << x << " is divisible by 3." << endl;
	}
	else {
		cout << x << " is not divisible by 3." << endl;
	}
	if (dividedby3(y)) {
		cout << y << " is divisible by 3." << endl;
	}
	else {
		cout << y << " is not divisible by 3." << endl;
	}
	return 0;
}