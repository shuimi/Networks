#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <locale.h>
#include <string>
#include <windows.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

using namespace std;

#define PORT 666
#define SERVERADDR "127.0.0.1" // IP-адрес сервера

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

void main() {

	char buff[10 * 1014];

	// Шаг 1 - иницилизация библиотеки Winsock
	if (WSAStartup(0x202, (WSADATA*)&buff)) {
		cout << "WSASTARTUP ERROR: " << WSAGetLastError() << "\n";
		return;
	}

	// Шаг 2 - открытие сокета
	SOCKET my_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (my_sock == INVALID_SOCKET) {
		cout << "SOCKET() ERROR: " << WSAGetLastError() << "\n";
		WSACleanup();
		return;
	}

	// Шаг 3 - обмен сообщений с сервером
	HOSTENT* hst;
	sockaddr_in Daddr;
	Daddr.sin_family = AF_INET;
	Daddr.sin_port = htons(PORT);
	// определение IP-адреса узла
	if (inet_addr(SERVERADDR))
		Daddr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		if (hst = gethostbyname(SERVERADDR))
			Daddr.sin_addr.s_addr = ((unsigned long**)hst->h_addr_list)[0][0];
		else {
			cout << "Unknown Host: " << WSAGetLastError() << "\n";
			closesocket(my_sock);
			WSACleanup();
			return;
		}

	do {

		string command;

		cout << "$ ";
		cin >> command;

		if (command == "r") {
			data_request request;

			cout << "Name: "; cin >> request.Name;
			cout << "Term: "; cin >> request.Current_Term;
			cout << "Good Marks: "; cin >> request.Good_Marks;
			cout << "Bad Marks: "; cin >> request.Bad_Marks;

			sendto(my_sock, (char*)&request, sizeof(data_request), 0, (sockaddr*)&Daddr, sizeof(Daddr));

			// Прием сообщения с сервера
			sockaddr_in SRaddr;
			data_response response;
			int SRaddr_size = sizeof(SRaddr);
			int n = recvfrom(my_sock, (char*)&response, sizeof(response), 0, (sockaddr*)&SRaddr, &SRaddr_size);
			if (n == SOCKET_ERROR) {
				cout << "RECVFROM() ERROR:" << WSAGetLastError() << "\n";
				closesocket(my_sock);
				WSACleanup();
				return;
			}
			cout << response.FPP << "\n\n";
		}
		else if (command == "e") break;
		
	} while (true);

	closesocket(my_sock);
	WSACleanup();
}