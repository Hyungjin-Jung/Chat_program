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

int main()
{
	// MySQL Connector/C++ 초기화
	sql::Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌
	sql::Connection* con;
	sql::Statement* stmt; // 추가!!
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

			cout << "1. 로그인  2. 회원가입  3. 비밀번호 찾기  4. 회원 탈퇴  5. 회원 정보 수정: ";
			cin >> num;

			switch (num)
			{
			case '1':
			{
				pstmt = con->prepareStatement("SELECT * FROM user;");
				cout << "\n아이디 입력 >> ";
				cin >> my_nick;

				cout << "\n패스워드 입력 >> ";
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
					cout << "\n로그인 성공!! \n";
					break;
				}
				else if (confirm == 1)
				{
					cout << "\n아이디 또는 비밀번호가 일치하지 않습니다!\n";
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
					cout << "\n사용할 아이디 입력 >> ";
					cin >> my_nick;
					//cout << result->getString("id") << " ";
					while (result->next())
					{
						if (result->getString("id") == my_nick)
						{
							cout << "\n아이디가 중복됩니다!\n다시 입력해주세요. \n";
							my_nick = "";
						}
					}
					delete result;
				}

				while (1)
				{
					while (1)
					{
						cout << "\n사용할 비밀번호 입력 >> ";
						cin >> password;
						cout << "\n비밀번호 확인: ";
						cin >> pw_check;
						if (pw_check == password)
							break;
						else
						{
							cout << "\n비밀번호가 일치하지 않습니다. \n다시 입력해주세요. \n";
						}
					}

					break;
				}

				pstmt = con->prepareStatement("SELECT * FROM user;");
				while (phonenum == "")
				{
					result = pstmt->executeQuery();
					cout << "\n전화번호 입력 >> ";
					cin >> phonenum;

					while (result->next())
					{
						if (result->getString("phonenum") == phonenum)
						{
							cout << "\n중복 회원가입은 불가능합니다!\n\n";
							phonenum = "";
						}
					}
					delete result;
				}
				result = pstmt->executeQuery();

				pstmt = con->prepareStatement("INSERT INTO user(id, pw, phonenum) VALUES(?,?,?)");
				pstmt->setString(1, my_nick); // 아이디
				pstmt->setString(2, password); // 비밀번호
				pstmt->setString(3, phonenum); // 전화번호
				pstmt->execute();
				delete pstmt;

				cout << "\n회원 가입 성공!!\n";

				break;
			}

			case '3':
			{
				int error = 1;

				cout << "\n아이디를 입력해주세요. \n";
				cin >> my_nick;
				cout << "\n전화번호를 입력해주세요. \n";
				cin >> phonenum;

				pstmt = con->prepareStatement("SELECT * FROM user;");
				result = pstmt->executeQuery();

				while (result->next())
				{
					if (my_nick == result->getString("id") && phonenum == result->getString("phonenum"))
					{
						cout << "\n비밀번호: " << result->getString(2) << endl << "5초 후 창이 닫힙니다!" << endl;
						error = 0;
						Sleep(3000);
						break;
					}
				}

				if (error == 1)
				{
					cout << "\n아이디 또는 전화번호를 잘못 입력하셨습니다. \n\n";
				}
				break;
			}

			case '4':
			{
				int error = 1;

				cout << "\n아이디를 입력해주세요. \n";
				cin >> my_nick;
				cout << "\n비밀번호를 입력해주세요. \n";
				cin >> password;
				cout << "\n전화번호를 입력해주세요. \n";
				cin >> phonenum;

				pstmt = con->prepareStatement("SELECT * FROM user;");
				result = pstmt->executeQuery();

				while (result->next())
				{
					if (my_nick == result->getString("id") && password == result->getString("pw") && phonenum == result->getString("phonenum"))
					{
						char drop;
						cout << "\n정말 탈퇴하시겠습니까? \n";
						cout << "\n1. 취소  2. 탈퇴 \n";
						cin >> drop;

						switch (drop)
						{
						case '1':
						{
							cout << "\n취소 되었습니다. \n";
							break;
						}
						case '2':
						{
							pstmt = con->prepareStatement("DELETE FROM user WHERE id = ?");
							pstmt->setString(1, my_nick);
							result = pstmt->executeQuery();
							cout << "\n탈퇴 되었습니다. \n";
							break;
						}
						default:
							cout << "\n1, 2 중에 선택해주세요.\n";
						}

						cout << "\n " << endl << endl;
						error = 0;
						Sleep(2000);
						break;
					}
				}
				if (error == 1)
				{
					cout << "\n정보를 잘못 입력하셨습니다. \n\n";
				}
				break;
			}

			case '5':
			{
				int error = 1;

				cout << "\n아이디를 입력해주세요. \n";
				cin >> my_nick;
				cout << "\n비밀번호를 입력해주세요. \n";
				cin >> password;

				pstmt = con->prepareStatement("SELECT * FROM user;");
				result = pstmt->executeQuery();

				while (result->next())
				{
					if (my_nick == result->getString("id") && password == result->getString("pw"))
					{
						char update;
						cout << "\n어떤 정보를 수정하시겠습니까? \n";
						cout << "\n1. 비밀번호  2. 전화번호 \n";
						cin >> update;

						switch (update)
						{
						case '1':
						{
							while (1)
							{
								cout << "\n수정할 비밀번호 입력 >> ";
								cin >> password;
								cout << "\n비밀번호 확인: ";
								cin >> pw_check;
								if (pw_check == password)
									break;
								else
								{
									cout << "\n비밀번호가 일치하지 않습니다. \n다시 입력해주세요. \n";
								}
							}

							pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?");
							pstmt->setString(1, password);
							pstmt->setString(2, my_nick);
							result = pstmt->executeQuery();
							cout << "\n비밀번호가 수정되었습니다. \n";
							break;
						}
						case '2':
						{
							cout << "\n새로운 전화번호 입력 >> ";
							cin >> phonenum;
							pstmt = con->prepareStatement("UPDATE user SET phonenum = ? WHERE id = ?");
							pstmt->setString(1, phonenum);
							pstmt->setString(2, my_nick);
							result = pstmt->executeQuery();
							cout << "\n전화번호가 수정되었습니다. \n";
							break;
						}
						default:
							cout << "\n1, 2 중에 선택해주세요.\n";
						}

						cout << "\n " << endl << endl;
						error = 0;
						Sleep(2000);
						break;
					}
				}
				if (error == 1)
				{
					cout << "\n정보를 잘못 입력하셨습니다. \n\n";
				}
				break;
			}

			default:
			{
				cout << "잘못 입력하셨습니다. \n\n";
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
			if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr)))	// connect는 정상 연결되면 0을 반환
			{
				cout << "Server Connect" << endl;
				send(client_sock, my_nick.c_str(), my_nick.length(), 0);
				break;
			}
			cout << "connecting..." << endl;
		}

		std::thread th2(chat_recv);

		cout << "\n!이전 대화 입력 시 이전 대화 내용을 불러옵니다. \n";
		cout << "!화면 정리 입력 시 화면을 지웁니다. \n";
		cout << "!공지 + 내용 입력 시 공지가 됩니다. \n";
		cout << "!종료 입력 시 종료합니다. \n\n";

		pstmt = con->prepareStatement("SELECT * FROM message WHERE text LIKE '!공지 %' ORDER BY time DESC LIMIT 1;");
		result = pstmt->executeQuery();
		while (result->next())
		{
			string notice = result->getString(4);
			notice.erase(0, 6);
			cout << "[공지]" << notice << "  작성자: " << result->getString(2) << endl;
		}

		while (1)
		{
			string text;
			std::getline(cin, text);
			const char* buffer = text.c_str();

			if (text == "!종료")
			{
				cout << "\n채팅을 종료합니다. \n";
				Sleep(2000);
				system("cls");
				exit(1);
			}
			else if (text == "!이전 대화")
			{
				cout << "<<<<이전 대화>>>>\n\n";

				pstmt = con->prepareStatement("SELECT * FROM message;");
				result = pstmt->executeQuery();

				while (result->next())
				{
					cout << "보낸 사람: " << result->getString(2) << "    시간: " << result->getString(5) << endl << "내용: " << result->getString(4) << "\n\n";
				}
			}
			else if (text == "!화면 정리")
			{
				Sleep(1000);
				system("cls");
				cout << "화면이 깨끗해졌습니다. \n\n";
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
