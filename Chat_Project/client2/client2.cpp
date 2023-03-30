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
#include <windows.h>

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
	stmt->execute("CREATE TABLE chatingroom (num INT UNSIGNED PRIMARY KEY, sender_id VARCHAR(15) NOT NULL, receiver_id VARCHAR(15) NOT NULL);");
	cout << "Finished creating table" << endl;
	stmt->execute("CREATE TABLE message (id INT UNSIGNED PRIMARY KEY, sender_id VARCHAR(15) NOT NULL, receiver_id INT VARCHAR(15), time DATETIME NOT NULL, text VARCHAR(45));");
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

			cout << "1. �α���  2. ȸ������  3. ��й�ȣ ã��  4. ȸ�� Ż��  5. ȸ�� ���� ����: ";
			cin >> num;

			switch (num)
			{
			case '1':
			{
				pstmt = con->prepareStatement("SELECT * FROM user;");
				cout << "\n���̵� �Է� >> ";
				cin >> my_nick;

				cout << "\n�н����� �Է� >> ";
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
					cout << "\n�α��� ����!! \n";
					break;
				}
				else if (confirm == 1)
				{
					cout << "\n���̵� �Ǵ� ��й�ȣ�� ��ġ���� �ʽ��ϴ�!\n";
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
					cout << "\n����� ���̵� �Է� >> ";
					cin >> my_nick;
					//cout << result->getString("id") << " ";
					while (result->next())
					{
						if (result->getString("id") == my_nick)
						{
							cout << "\n���̵� �ߺ��˴ϴ�!\n�ٽ� �Է����ּ���. \n";
							my_nick = "";
						}
					}
					delete result;
				}

				while (1)
				{
					while (1)
					{
						cout << "\n����� ��й�ȣ �Է� >> ";
						cin >> password;
						cout << "\n��й�ȣ Ȯ��: ";
						cin >> pw_check;
						if (pw_check == password)
							break;
						else
						{
							cout << "\n��й�ȣ�� ��ġ���� �ʽ��ϴ�. \n�ٽ� �Է����ּ���. \n";
						}
					}

					break;
				}

				pstmt = con->prepareStatement("SELECT * FROM user;");
				while (phonenum == "")
				{
					result = pstmt->executeQuery();
					cout << "\n��ȭ��ȣ �Է� >> ";
					cin >> phonenum;

					while (result->next())
					{
						if (result->getString("phonenum") == phonenum)
						{
							cout << "\n�ߺ� ȸ�������� �Ұ����մϴ�!\n\n";
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

				cout << "\nȸ�� ���� ����!!\n";

				break;
			}

			case '3':
			{
				int error = 1;

				cout << "\n���̵� �Է����ּ���. \n";
				cin >> my_nick;
				cout << "\n��ȭ��ȣ�� �Է����ּ���. \n";
				cin >> phonenum;

				pstmt = con->prepareStatement("SELECT * FROM user;");
				result = pstmt->executeQuery();

				while (result->next())
				{
					if (my_nick == result->getString("id") && phonenum == result->getString("phonenum"))
					{
						cout << "\n��й�ȣ: " << result->getString(2) << endl << "5�� �� â�� �����ϴ�!" << endl;
						error = 0;
						Sleep(3000);
						break;
					}
				}

				if (error == 1)
				{
					cout << "\n���̵� �Ǵ� ��ȭ��ȣ�� �߸� �Է��ϼ̽��ϴ�. \n\n";
				}
				break;
			}

			case '4':
			{
				int error = 1;

				cout << "\n���̵� �Է����ּ���. \n";
				cin >> my_nick;
				cout << "\n��й�ȣ�� �Է����ּ���. \n";
				cin >> password;
				cout << "\n��ȭ��ȣ�� �Է����ּ���. \n";
				cin >> phonenum;

				pstmt = con->prepareStatement("SELECT * FROM user;");
				result = pstmt->executeQuery();

				while (result->next())
				{
					if (my_nick == result->getString("id") && password == result->getString("pw") && phonenum == result->getString("phonenum"))
					{
						char drop;
						cout << "\n���� Ż���Ͻðڽ��ϱ�? \n";
						cout << "\n1. ���  2. Ż�� \n";
						cin >> drop;

						switch (drop)
						{
						case '1':
						{
							cout << "\n��� �Ǿ����ϴ�. \n";
							break;
						}
						case '2':
						{
							pstmt = con->prepareStatement("DELETE FROM user WHERE id = ?");
							pstmt->setString(1, my_nick);
							result = pstmt->executeQuery();
							cout << "\nŻ�� �Ǿ����ϴ�. \n";
							break;
						}
						default:
							cout << "\n1, 2 �߿� �������ּ���.\n";
						}

						cout << "\n " << endl << endl;
						error = 0;
						Sleep(2000);
						break;
					}
				}
				if (error == 1)
				{
					cout << "\n������ �߸� �Է��ϼ̽��ϴ�. \n\n";
				}
				break;
			}

			case '5':
			{
				int error = 1;

				cout << "\n���̵� �Է����ּ���. \n";
				cin >> my_nick;
				cout << "\n��й�ȣ�� �Է����ּ���. \n";
				cin >> password;

				pstmt = con->prepareStatement("SELECT * FROM user;");
				result = pstmt->executeQuery();

				while (result->next())
				{
					if (my_nick == result->getString("id") && password == result->getString("pw"))
					{
						char update;
						cout << "\n� ������ �����Ͻðڽ��ϱ�? \n";
						cout << "\n1. ��й�ȣ  2. ��ȭ��ȣ \n";
						cin >> update;

						switch (update)
						{
						case '1':
						{
							while (1)
							{
								cout << "\n������ ��й�ȣ �Է� >> ";
								cin >> password;
								cout << "\n��й�ȣ Ȯ��: ";
								cin >> pw_check;
								if (pw_check == password)
									break;
								else
								{
									cout << "\n��й�ȣ�� ��ġ���� �ʽ��ϴ�. \n�ٽ� �Է����ּ���. \n";
								}
							}

							pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?");
							pstmt->setString(1, password);
							pstmt->setString(2, my_nick);
							result = pstmt->executeQuery();
							cout << "\n��й�ȣ�� �����Ǿ����ϴ�. \n";
							break;
						}
						case '2':
						{
							cout << "\n���ο� ��ȭ��ȣ �Է� >> ";
							cin >> phonenum;
							pstmt = con->prepareStatement("UPDATE user SET phonenum = ? WHERE id = ?");
							pstmt->setString(1, phonenum);
							pstmt->setString(2, my_nick);
							result = pstmt->executeQuery();
							cout << "\n��ȭ��ȣ�� �����Ǿ����ϴ�. \n";
							break;
						}
						default:
							cout << "\n1, 2 �߿� �������ּ���.\n";
						}

						cout << "\n " << endl << endl;
						error = 0;
						Sleep(2000);
						break;
					}
				}
				if (error == 1)
				{
					cout << "\n������ �߸� �Է��ϼ̽��ϴ�. \n\n";
				}
				break;
			}

			default:
			{
				cout << "�߸� �Է��ϼ̽��ϴ�. \n\n";
			}
			}
			num = 0;

			Sleep(2000);
			system("cls");
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

		cout << "\n!���� ��ȭ �Է� �� ���� ��ȭ ������ �ҷ��ɴϴ�. \n";
		cout << "!ȭ�� ���� �Է� �� ȭ���� ����ϴ�. \n";
		cout << "!���� + ���� �Է� �� ������ �˴ϴ�. \n";
		cout << "!���� �Է� �� �����մϴ�. \n\n";

		pstmt = con->prepareStatement("SELECT * FROM message WHERE text LIKE '!���� %' ORDER BY time DESC LIMIT 1;");
		result = pstmt->executeQuery();
		while (result->next())
		{
			string notice = result->getString(4);
			notice.erase(0, 6);
			cout << "[����]" << notice << "  �ۼ���: " << result->getString(2) << endl;
		}

		while (1)
		{
			string text;
			std::getline(cin, text);
			const char* buffer = text.c_str();

			if (text == "!����")
			{
				cout << "\nä���� �����մϴ�. \n";
				Sleep(2000);
				system("cls");
				exit(1);
			}
			else if (text == "!���� ��ȭ")
			{
				cout << "<<<<���� ��ȭ>>>>\n\n";

				pstmt = con->prepareStatement("SELECT * FROM message;");
				result = pstmt->executeQuery();

				while (result->next())
				{
					cout << "���� ���: " << result->getString(2) << "    �ð�: " << result->getString(5) << endl << "����: " << result->getString(4) << "\n\n";
				}
			}
			else if (text == "!ȭ�� ����")
			{
				Sleep(1000);
				system("cls");
				cout << "ȭ���� �����������ϴ�. \n\n";
			}
			else
			{
				send(client_sock, buffer, strlen(buffer), 0);
			}
		}

		th2.join();
		closesocket(client_sock);
	}

	WSACleanup();
	return 0;
}
