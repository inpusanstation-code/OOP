#include <iostream>
using namespace std;

int addArray(int arr[], int size);
void makeDouble(int arr[], int size);
void printArray(int arr[], int size);

int main() {
	int arr[5] = { 1, 2, 3, 4, 5 };
	int sum = addArray(arr, 5);
	cout << "Sum of array elements: " << sum << endl;

	makeDouble(arr, 5);
	cout << "Array elements after doubling: ";
	printArray(arr, 5);
}

int addArray(int arr[], int size) {
	int i, sum = 0;
	for (i = 0; i < size; i++) {
		sum += arr[i];
	}
	return sum;
}

void makeDouble(int arr[], int size) {
	int i;
	for (i = 0; i < size; i++) {
		arr[i] *= 2;
	}
}

void printArray(int arr[], int size) {
	int i;
	for (i = 0; i < size; i++) {
		cout << arr[i] << " ";
	}
	cout << endl;
}