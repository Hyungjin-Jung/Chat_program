#include <iostream>


double circle(int r, float pi)
{
	return (double)r * (double)r * pi;
}

using namespace std;

int circle2()
{
	cout << "1 + 1 = 3";
	cout << "1 + 1 = 2";
	cout << "2 + 3 = 5";
}

int main()
{
	int r;
	double pi = 3.141592;
	
	std::cout << "���� �Է�: ";
	std::cin >> r;
	std::cout << "���� ���̴� " << circle(r, pi) << std::endl;
}