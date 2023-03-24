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
	cout << "2 + 4 = 6";
}

int main()
{
	int r;
	double pi = 3.141592;
	
<<<<<<< HEAD
	std::cout << "Radius: ";
	std::cin >> r;
	std::cout << "Area: " << circle(r, pi) << std::endl;
=======
	std::cout << "���� �Է�: ";
	std::cin >> r;
	std::cout << "���� ���̴� " << circle(r, pi) << std::endl;
>>>>>>> 7d14839f8d6b5892aa5e45a7cb135f79171c9992
}