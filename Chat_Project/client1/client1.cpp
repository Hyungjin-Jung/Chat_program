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
string phonenum = "";
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

int main()
{
	// MySQL Connector/C++ �ʱ�ȭ
	sql::Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ ��
	sql::Connection* con;
	sql::Statement* stmt; // �߰�!!
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	string pw_check = "";

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
	stmt->execute("CREATE TABLE user (id VARCHAR(15) PRIMARY KEY, pw VARCHAR(10) NOT NULL,phonenum STRING NOT NULL);");
	cout << "Finished creating table" << endl;
	stmt->execute("CREATE TABLE chatingroom (num INT UNSIGNED PRIMARY KEY, sender_id INT UNSIGNED NOT NULL, receiver_id INT UNSIGNED NOT NULL);");
	cout << "Finished creating table" << endl;
	stmt->execute("CREATE TABLE message (id INT UNSIGNED PRIMARY KEY, sender_id INT UNSIGNED NOT NULL, receiver_id INT UNSIGNED NOT NULL, time DATETIME NOT NULL, text VARCHAR(45));");
	cout << "Finished creating table" << endl;
	*/
	delete stmt;


	if (!code)
	{
		char num;
		int confirm = 1;
		string nick_check;

		while (confirm)
		{
			my_nick = "";
			phonenum = "";

			cout << "1. �α���  2. ȸ������  3. ��й�ȣ ã�� : ";
			cin >> num;

			switch (num)
			{
			case '1':
			{
				pstmt = con->prepareStatement("SELECT * FROM user;");
				cout << "���̵� �Է� >> ";
				cin >> my_nick;

				cout << "�н����� �Է� >> ";
				cin >> password;

				result = pstmt->executeQuery();

				while (result->next())
				{
					if (my_nick == result->getString("id") && password == result->getString("pw"))
					{
						confirm = 0;
					}
				}
				if (confirm == 0)
				{
					cout << "�α��� ����!! \n";
					break;
				}
				else if (confirm == 1)
				{
					cout << "���̵� �Ǵ� ��й�ȣ�� ��ġ���� �ʽ��ϴ�!\n";
				}
				delete result;
				//pstmt->execute();
				delete pstmt;
				break;
			}

			case '2':
			{
				pstmt = con->prepareStatement("SELECT * FROM user;");
				while (my_nick == "")
				{
					result = pstmt->executeQuery();
					cout << "����� ���̵� �Է� >> ";
					cin >> my_nick;
					//cout << result->getString("id") << " ";
					while (result->next())
					{
						if (result->getString("id") == my_nick)
						{
							cout << "���̵� �ߺ��˴ϴ�!\n �ٽ� �Է����ּ���. \n";
							my_nick = "";
						}
					}
					delete result;
				}
				result = pstmt->executeQuery();

				while (1)
				{
					while (1)
					{
						cout << "����� ��й�ȣ �Է� >> ";
						cin >> password;
						cout << "��й�ȣ Ȯ��: ";
						cin >> pw_check;
						if (pw_check == password)
							break;
						else
						{
							cout << "��й�ȣ�� ��ġ���� �ʽ��ϴ�. \n �ٽ� ó������ �Է����ּ���. \n";
						}
					}

					break;
				}

				pstmt = con->prepareStatement("SELECT * FROM user;");
				while (phonenum == "")
				{
					result = pstmt->executeQuery();
					cout << "��ȭ��ȣ �Է� >> ";
					cin >> phonenum;

					while (result->next())
					{
						if (result->getString("phonenum") == phonenum)
						{
							cout << "�ߺ� ȸ�������� �Ұ����մϴ�!\n";
							phonenum = "";
						}
					}
					delete result;
				}
				result = pstmt->executeQuery();

				pstmt = con->prepareStatement("INSERT INTO user(id, pw, phonenum) VALUES(?,?,?)");
				pstmt->setString(1, my_nick); // ���̵�
				pstmt->setString(2, password); // ��й�ȣ
				pstmt->setString(3, phonenum); // ��ȭ��ȣ
				pstmt->execute();
				delete pstmt;

				break;
			}

			case '3':
			{
				int error = 1;

				cout << "���̵� �Է����ּ���. \n";
				cin >> my_nick;
				cout << "��ȭ��ȣ�� �Է����ּ���. \n";
				cin >> phonenum;

				pstmt = con->prepareStatement("SELECT * FROM user;");
				result = pstmt->executeQuery();

				while (result->next())
				{
					if (my_nick == result->getString("id") && phonenum == result->getString("phonenum"))
					{
						cout << "\n���̵�: " << result->getString(1) << "  ��й�ȣ: " << result->getString(2).c_str() << "  ��ȭ��ȣ: " << result->getString(3) << endl << endl;
						error = 0;
						break;
					}
				}
				if (error == 1)
				{
					cout << "\n���̵� �Ǵ� ��ȭ��ȣ�� �߸� �Է��ϼ̽��ϴ�. \n\n";
				}
				break;
			}

			default:
			{
				cout << "�߸� �Է��ϼ̽��ϴ�. \n\n";
			}
			}
			num = 0;
		}


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
