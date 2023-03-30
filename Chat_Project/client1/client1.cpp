#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>
#include <vector>
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
string phonenum = "";
const string username = "user"; // 데이터베이스 사용자
const string password1 = "1234"; // 데이터베이스 접속 비밀번호

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소

struct SOCKET_INFO
{
	SOCKET sck;
	string user;
};

std::vector<SOCKET_INFO> sck_list;
SOCKET_INFO server_sock;
int client_count = 0;

// 1. 소켓 초기화
// socket(), bind(), listen()
// 소켓을 생성하고, 주소를 묶어주고, 활성화 -> 대기상태
void server_init();

// 2. 클라이언트 추가
// accept(), recv()
// 연결을 설정하고 클라이언트가 전송한 닉네임을 받음
void add_client();

// 3. 클라이언트에게 msg 보내기
// send()
void send_msg(const char* msg);

// 4. 클라이언트에게 채팅 내용을 받음
// 퇴장했다면 (0을 반환) "퇴장했습니다." 공지 띄워줌
void recv_msg(int idx);

// 5. 소켓 달아줌
void del_client(int idx);


int main()
{
	WSADATA wsa;

	int code = WSAStartup(MAKEWORD(2, 2), &wsa);
	// winsock version 2.2 사용
	// winsock 초기화하는 함수
	// 실행 성공하면 0 반환, 실패하면 0 이외의 값 반환

	if (!code)	// 실행 성공
	{
		server_init();

		std::thread th1[MAX_CLIENT];

		for (int i = 0; i < MAX_CLIENT; i++)
		{
			th1[i] = std::thread(add_client);
		}

		while (1)
		{
			string text, msg = "";
			std::getline(cin, text); // 띄어쓰기까지 입력
			const char* buf = text.c_str();

			msg = server_sock.user + ':' + buf;

			send_msg(msg.c_str());
		}

		for (int i = 0; MAX_CLIENT; i++)
		{
			th1[i].join();
		}

		closesocket(server_sock.sck);

		WSACleanup();
		return 0;
	}

	// return 0;
}


/* 함수 구현부 */

// 1. 소켓 초기화 
void server_init()
{
	server_sock.sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//IPPROTO_TCP 대신 숫자 6을 써도 가능
	/*
	socket(int AF, int 통신타입, int PROTOCOL)
	- 주소 체계 형식
	- 통신 타입 설정
	- 어떤 프로토콜 사용할건지
	*/

	SOCKADDR_IN server_addr = {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(7777);	// htons() : 2바이트 이상의 변수에 대해 바이트 순서 체계를 바꿔주는것(short)
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// INADDR_ANY : localhost 를 의미, localhost = 127.0.0.1

	bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr));
	/*
	bind(SOCKET s, const sockaddr *name, int namelen);
	- socket()으로 만들어준 소켓
	- 소켓과 연결할 주소 정보를 담고 있는 구조체
		(코드에서는 server_addr)
	- 두번째 매개변수의 크기
	*/

	listen(server_sock.sck, SOMAXCONN);
	/*
	listen(SOCKET s, int backlog);
	*/

	server_sock.user = "server";

	// 서버가 커지면 나올 문구

	cout << "Server ON!!" << endl;
}

// 2. 클라이언트 연결 & 추가
void add_client()
{
	SOCKADDR_IN addr = {};
	int addrsize = sizeof(addr);
	char buf[MAX_SIZE] = {};

	ZeroMemory(&addr, addrsize);	// addr 0으로 초기화


	SOCKET_INFO new_client = {};

	new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
	/*
	SOCKET accept(SOCKET s, sockaddr *addr, &addrsize;
	- socket()으로 만들어준 소켓
	- client의 주소 정보를 저장할 구조체
	- 2번 매개변수의 크기
	*/

	recv(new_client.sck, buf, MAX_SIZE, 0);
	/*
	recv(SOCKET s, const char * buf, int len, int flags)
	- accept()으로 만들어준 소켓 (통신을 위한 소켓)
	- 데이터를 받을 변수
	- 두번쪠 매개변수의 길이
	- flag
	*/

	new_client.user = string(buf);
	// buf를 string으로 변환해서 담아줌

	string msg = "[공지]" + new_client.user + "님이 입장했습니다. ";
	cout << msg << endl;

	sck_list.push_back(new_client);

	std::thread th(recv_msg, client_count);

	client_count++;
	cout << "[공지] 현재 접속자 수: " << client_count << "명" << endl;
	send_msg(msg.c_str());

	th.join();	// 쓰레드 끝냄
}

void send_msg(const char* msg)
{
	for (int i = 0; i < client_count; i++)
	{
		send(sck_list[i].sck, msg, MAX_SIZE, 0);
	}
}

void recv_msg(int idx)
{
	char buf[MAX_SIZE] = {};
	string msg = "";

	while (1)
	{
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0)
		{
			sql::Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌
			sql::Connection* con;
			sql::Statement* stmt; // 추가!!
			sql::PreparedStatement* pstmt;
			sql::ResultSet* result;

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
			delete stmt;

			pstmt = con->prepareStatement("INSERT INTO message(sender_id, receiver_id, text) VALUES(?,?,?)");

			// 만약 정상적으로 받았다면
			msg = sck_list[idx].user + " : " + buf;
			cout << msg << endl;
			send_msg(msg.c_str());

			pstmt->setString(1, sck_list[idx].user); // sender_id
			pstmt->setString(2, ""); // receiver_id
			pstmt->setString(3, buf); // text
			pstmt->execute();

			delete pstmt;
		}
		else
		{
			msg = "[공지]" + sck_list[idx].user + "님이 퇴장했습니다.";
			cout << msg << endl;
			send_msg(msg.c_str());

			del_client(idx);
			return;
		}
	}
}

void del_client(int idx)
{
	closesocket(sck_list[idx].sck);
	client_count--;
}
