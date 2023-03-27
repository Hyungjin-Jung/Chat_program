#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <sstream>

#define MAX_SIZE 1024
#define MAX_CLIENT 3

using std::cout;
using std::cin;
using std::endl;
using std::string;

SOCKET client_sock;
string my_nick;

int chat_recv()
{
	char buf[MAX_SIZE] = {};
	string msg;
	while (1)
	{
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0)	// 0 일 때 정상 종료, 0보다 크면 정상적으로 받음
		{
			msg = buf;
			string user;
			std::stringstream ss(msg);	// 문자열을 공백과 '\n'을 기준으로 여러 개의 다른 형식으로 차례대로 분리
			ss >> user;

			if (user != my_nick)
				cout << buf << endl;	// 채팅과 유저 이름이 들어 있음
		}
		else
		{
			cout << "Server OFF!!" << endl;
			return -1;
		}
	}
}

int main()
{
	WSADATA wsa;
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (!code)
	{
		cout << "사용할 닉네임 입력 >> ";
		cin >> my_nick;

		client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKADDR_IN client_addr = {};
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(7777);
		InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

		while (1)
		{
			if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr)))	// connect는 정상 연결되면 0을 반환
			{
				cout << "Server Connect" << endl;
				send(client_sock, my_nick.c_str(), my_nick.length(), 0);
				break;
			}
			cout << "connecting..." << endl;
		}

		std::thread th2(chat_recv);

		while (1)
		{
			string text;
			std::getline(cin, text);
			const char* buffer = text.c_str();
			send(client_sock, buffer, strlen(buffer), 0);
		}

		th2.join();
		closesocket(client_sock);
	}

	WSACleanup();
	return 0;
}
