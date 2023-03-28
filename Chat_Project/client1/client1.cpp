#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <sstream>
#include <stdlib.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h> // 추가!!
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

#define MAX_SIZE 1024
#define MAX_CLIENT 10

using std::cout;
using std::cin;
using std::endl;
using std::string;

SOCKET client_sock;
string my_nick = "";
string password = "";
int phonenum = 0;
const string username = "user"; // 데이터베이스 사용자
const string password1 = "1234"; // 데이터베이스 접속 비밀번호

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소

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

void sign_in()		// 로그인
{

}

void sign_up()		// 회원가입
{

}

int main()
{
	// MySQL Connector/C++ 초기화
	sql::Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌
	sql::Connection* con;
	sql::Statement* stmt; // 추가!!
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	string ok_password = "";

	WSADATA wsa;
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);

	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password1);
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}

	con->setSchema("ceemychat");	// ceemychat이라는 스키마 실행
	stmt = con->createStatement(); // 추가!!
	stmt->execute("set names euckr"); // 추가!!
	if (stmt) { delete stmt; stmt = nullptr; } // 추가!!
	// 데이터베이스 쿼리 실행
	/*
	stmt->execute("DROP TABLE IF EXISTS user");
	stmt->execute("DROP TABLE IF EXISTS chatingroom");
	stmt->execute("DROP TABLE IF EXISTS message");
	stmt = con->createStatement();
	stmt->execute("CREATE TABLE user (id VARCHAR(15) PRIMARY KEY, pw VARCHAR(10) NOT NULL,phonenum INT NOT NULL);");
	cout << "Finished creating table" << endl;
	stmt->execute("CREATE TABLE chatingroom (num INT UNSIGNED PRIMARY KEY, sender_id INT UNSIGNED NOT NULL, receiver_id INT UNSIGNED NOT NULL);");
	cout << "Finished creating table" << endl;
	stmt->execute("CREATE TABLE message (id INT UNSIGNED PRIMARY KEY, sender_id INT UNSIGNED NOT NULL, receiver_id INT UNSIGNED NOT NULL, time DATETIME NOT NULL, text VARCHAR(45));");
	cout << "Finished creating table" << endl;
	*/
	delete stmt;


	if (!code)
	{
		int num = 0;
		int ok = 1;

		cout << "1. 로그인 \t 2. 회원가입 : ";
		cin >> num;

		switch (num)
		{
		case 1:
		{
			pstmt = con->prepareStatement("SELECT * FROM user;");
			result = pstmt->executeQuery();

			while (result->next())
			{
				cout << "아이디 입력 >> ";
				cin >> my_nick;
				cout << "패스워드 입력 >> ";
				cin >> password;

				if (my_nick == result->getString("id"), password == result->getString("pw"))
				{
					cout << "로그인 성공!! \n";
					break;
				}
				else
				{
					cout << "아이디 Or 비밀번호가 일치하지 않습니다!\n 다시 입력해주세요. \n";
					cout << "비밀번호 찾기를 하시겠습니까? \n1. 찾는다. 2. 재시도한다.";
					cin >> num;
					if (num == 1)
					{
						cout << "아이디를 입력해주세요. \n";
						cin >> my_nick;
						cout << "전화번호를 입력해주세요. \n";
						cin >> phonenum;

						while (result->next())
						{
							if (my_nick == result->getString("id"), phonenum == result->getInt("phonenum"))
							{
								cout << result->getString(1) << result->getString(2).c_str() << result->getInt(3);
								break;
							}
						}
					}
				}
			}
			pstmt->execute();
		}

		case 2:
		{
			pstmt = con->prepareStatement("SELECT * FROM user;");
			result = pstmt->executeQuery();

			while (ok)
			{
				cout << "사용할 아이디 입력 >> ";
				cin >> my_nick;

				//cout << result->getString("id") << " ";

				while (result->next())
				{
					if (my_nick != result->getString("id"))
					{
						cout << "사용 가능한 아이디입니다! \n";
						break;
					}
					else if (my_nick == result->getString("id"))
					{
						cout << "아이디가 중복됩니다!\n 다시 입력해주세요. \n";
					}
				}
			}

			while (1)
			{
				while (1)
				{
					cout << "사용할 비밀번호 입력 >> ";
					cin >> password;
					cout << "비밀번호 확인: ";
					cin >> ok_password;
					if (ok_password == password)
						break;
					else
					{
						cout << "비밀번호가 일치하지 않습니다. \n 다시 처음부터 입력해주세요. \n";
					}
				}

				break;
			}

			while (result->next())
			{
				cout << "전화번호 입력 >> ";
				cin >> phonenum;

				cout << result->getString("phonenum") << " ";
				if (my_nick == result->getString("phonenum"))
				{
					cout << "중복 회원가입은 불가능합니다!\n";
					break;
				}
			}

			pstmt = con->prepareStatement("INSERT INTO user(id, pw, phonenum) VALUES(?,?,?)");
			pstmt->setString(1, my_nick); // 아이디
			pstmt->setString(2, password); // 비밀번호
			pstmt->setInt(3, phonenum); // 전화번호
			pstmt->execute();

		}

		default:
		{
			cout << "잘못 입력하셨습니다. \n";
		}
		}


		// please create database "ceemychat" ahead of time
		con->setSchema("ceemychat");
		// connector에서 한글 출력을 위한 셋팅
		stmt = con->createStatement(); // 추가!!
		stmt->execute("set names euckr"); // 추가!!
		if (stmt) { delete stmt; stmt = nullptr; } // 추가!!

		client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKADDR_IN client_addr = {};
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(7777);
		InetPton(AF_INET, TEXT("127.0.0.1:3306"), &client_addr.sin_addr);

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
