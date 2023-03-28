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
#include <cppconn/statement.h> // �߰�!!
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
const string username = "user"; // �����ͺ��̽� �����
const string password1 = "1234"; // �����ͺ��̽� ���� ��й�ȣ

const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ�

int chat_recv()
{
	char buf[MAX_SIZE] = {};
	string msg;
	while (1)
	{
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0)	// 0 �� �� ���� ����, 0���� ũ�� ���������� ����
		{
			msg = buf;
			string user;
			std::stringstream ss(msg);	// ���ڿ��� ����� '\n'�� �������� ���� ���� �ٸ� �������� ���ʴ�� �и�
			ss >> user;

			if (user != my_nick)
				cout << buf << endl;	// ä�ð� ���� �̸��� ��� ����
		}
		else
		{
			cout << "Server OFF!!" << endl;
			return -1;
		}
	}
}

void sign_in()		// �α���
{

}

void sign_up()		// ȸ������
{

}

int main()
{
	// MySQL Connector/C++ �ʱ�ȭ
	sql::Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ ��
	sql::Connection* con;
	sql::Statement* stmt; // �߰�!!
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

	con->setSchema("ceemychat");	// ceemychat�̶�� ��Ű�� ����
	stmt = con->createStatement(); // �߰�!!
	stmt->execute("set names euckr"); // �߰�!!
	if (stmt) { delete stmt; stmt = nullptr; } // �߰�!!
	// �����ͺ��̽� ���� ����
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

		cout << "1. �α��� \t 2. ȸ������ : ";
		cin >> num;

		switch (num)
		{
		case 1:
		{
			pstmt = con->prepareStatement("SELECT * FROM user;");
			result = pstmt->executeQuery();

			while (result->next())
			{
				cout << "���̵� �Է� >> ";
				cin >> my_nick;
				cout << "�н����� �Է� >> ";
				cin >> password;

				if (my_nick == result->getString("id"), password == result->getString("pw"))
				{
					cout << "�α��� ����!! \n";
					break;
				}
				else
				{
					cout << "���̵� Or ��й�ȣ�� ��ġ���� �ʽ��ϴ�!\n �ٽ� �Է����ּ���. \n";
					cout << "��й�ȣ ã�⸦ �Ͻðڽ��ϱ�? \n1. ã�´�. 2. ��õ��Ѵ�.";
					cin >> num;
					if (num == 1)
					{
						cout << "���̵� �Է����ּ���. \n";
						cin >> my_nick;
						cout << "��ȭ��ȣ�� �Է����ּ���. \n";
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
				cout << "����� ���̵� �Է� >> ";
				cin >> my_nick;

				//cout << result->getString("id") << " ";

				while (result->next())
				{
					if (my_nick != result->getString("id"))
					{
						cout << "��� ������ ���̵��Դϴ�! \n";
						break;
					}
					else if (my_nick == result->getString("id"))
					{
						cout << "���̵� �ߺ��˴ϴ�!\n �ٽ� �Է����ּ���. \n";
					}
				}
			}

			while (1)
			{
				while (1)
				{
					cout << "����� ��й�ȣ �Է� >> ";
					cin >> password;
					cout << "��й�ȣ Ȯ��: ";
					cin >> ok_password;
					if (ok_password == password)
						break;
					else
					{
						cout << "��й�ȣ�� ��ġ���� �ʽ��ϴ�. \n �ٽ� ó������ �Է����ּ���. \n";
					}
				}

				break;
			}

			while (result->next())
			{
				cout << "��ȭ��ȣ �Է� >> ";
				cin >> phonenum;

				cout << result->getString("phonenum") << " ";
				if (my_nick == result->getString("phonenum"))
				{
					cout << "�ߺ� ȸ�������� �Ұ����մϴ�!\n";
					break;
				}
			}

			pstmt = con->prepareStatement("INSERT INTO user(id, pw, phonenum) VALUES(?,?,?)");
			pstmt->setString(1, my_nick); // ���̵�
			pstmt->setString(2, password); // ��й�ȣ
			pstmt->setInt(3, phonenum); // ��ȭ��ȣ
			pstmt->execute();

		}

		default:
		{
			cout << "�߸� �Է��ϼ̽��ϴ�. \n";
		}
		}


		// please create database "ceemychat" ahead of time
		con->setSchema("ceemychat");
		// connector���� �ѱ� ����� ���� ����
		stmt = con->createStatement(); // �߰�!!
		stmt->execute("set names euckr"); // �߰�!!
		if (stmt) { delete stmt; stmt = nullptr; } // �߰�!!

		client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKADDR_IN client_addr = {};
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(7777);
		InetPton(AF_INET, TEXT("127.0.0.1:3306"), &client_addr.sin_addr);

		while (1)
		{
			if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr)))	// connect�� ���� ����Ǹ� 0�� ��ȯ
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
