#include <iostream>

double circle(int r, float pi)
{
	return (double)r * (double)r * pi;
}

int main()
{
	int r;
	double pi = 3.141592;
	
	std::cout << "반지름 입력: ";
	std::cin >> r;
	std::cout << "원의 넓이는 " << circle(r, pi) << std::endl;
}