#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <locale.h>
#include <iostream>  
#define _WINSOCK_DEPRECATED_NO_WARNINGS  
// подавление предупреждений библиотеки winsock2
#include <winsock2.h> 
#include <string>
#include <windows.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable: 4996)  // подавление предупреждения 4996 

using namespace std;

#define SRV_HOST "localhost"  
#define SRV_PORT 1234 
#define CLNT_PORT 1235  

char TXT_ANSW[] = "I am your student\n";

const int name_length = 15;

struct data_request {
	char Name[name_length];
	int Current_Term;
	int Good_Marks;
	int Bad_Marks;
};

struct data_response {
	int FPP;
};

int main() {
	char buff[1024];

	if (WSAStartup(0x0202, (WSADATA*)&buff[0])) {
		cout << "Error WSAStartup \n" << WSAGetLastError();
		return -1;
	}
	
	SOCKET sock;
	
	hostent* host_info;
	sockaddr_in client_sin, server_sin; //adress info about client & server
	
	sock = socket(AF_INET, SOCK_STREAM, 0); //AF_INET - TCP/IP, SOCK_STREAM, 0 - без указания протокола - подключит TCP
	//домен, протокол и вид соединения

	//заполняем адресную структуру клиента
	client_sin.sin_family = AF_INET; //домен
	client_sin.sin_addr.s_addr = 0; 
	client_sin.sin_port = htons(CLNT_PORT); //порт клиента
	
	bind(sock, (sockaddr*)&client_sin, sizeof(client_sin)); //записываем инфу о клиенте в сокет s
	host_info = gethostbyname(SRV_HOST);	//по силмвольному имени получаем числовой адрес
	
	//заполняем адресную структуру сервера
	server_sin.sin_port = htons(SRV_PORT);
	server_sin.sin_family = AF_INET;
	((unsigned long*)&server_sin.sin_addr)[0] = ((unsigned long**)host_info->h_addr_list)[0][0];
	
	//записываем инфу о сервере в сокет s
	connect(sock, (sockaddr*)&server_sin, sizeof(server_sin));
	string command;
	
	do {
		cout << "$ ";
		cin >> command;

		if (command == "r") {
			data_request request;

			cout << "Name: "; cin >> request.Name;
			cout << "Term: "; cin >> request.Current_Term;
			cout << "Good Marks: "; cin >> request.Good_Marks;
			cout << "Bad Marks: "; cin >> request.Bad_Marks;

			send(sock, (char*)&request, sizeof(data_request), 0);

			data_response response;
			recv(sock, (char*)&response, sizeof(data_response), 0);
			cout << response.FPP << "\n\n";
		}
		else if (command == "e") {
			data_request request;
			request.Name[0] = 'e';
			request.Current_Term = 0;
			request.Good_Marks = 0;
			request.Bad_Marks = 0;
			send(sock, (char*)&request, sizeof(data_request), 0);
			Sleep(1000);
			break;
		}

	} while (true);

	closesocket(sock);
	WSACleanup();
	return 0;
}