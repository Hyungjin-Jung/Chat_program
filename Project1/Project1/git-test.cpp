#include <iostream>

double circle(int r, float pi)
{
	return (double)r * (double)r * pi;
}

int main()
{
	int r;
	double pi = 3.141592;
	
	std::cout << "������ �Է�: ";
	std::cin >> r;
	std::cout << "���� ���̴� " << circle(r, pi) << std::endl;
}