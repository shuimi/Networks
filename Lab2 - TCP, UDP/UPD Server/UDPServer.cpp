#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <locale.h>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

using namespace std;

#define PORT 666 // порт сервера
const string sHELLO = "Hello, STUDENT\n";

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

int Freebie_Power(data_request Current_Request) {
	int Unpecented = Current_Request.Current_Term * (Current_Request.Good_Marks - Current_Request.Bad_Marks);
	if (Unpecented < 0) return 0;
	else if ((216 / 100) * Unpecented > 100) return 100;
	else return (216 / 100) * Unpecented;
}

int main(int argc, char* argv[]) {
	char buff[1024];

	// шаг 1 - подключение библиотеки 
	if (WSAStartup(0x202, (WSADATA*)&buff[0])) {
		cout << "WSAStartup error: " << WSAGetLastError();
		return -1;
	}
	// шаг 2 - создание сокета
	SOCKET Lsock;
	Lsock = socket(AF_INET, SOCK_DGRAM, 0);
	if (Lsock == INVALID_SOCKET) {
		cout << "SOCKET() ERROR: " << WSAGetLastError();
		WSACleanup();
		return -1;
	}
	// шаг 3 - связывание сокета с локальным адресом 
	sockaddr_in Laddr;
	Laddr.sin_family = AF_INET;
	Laddr.sin_addr.s_addr = INADDR_ANY; // или 0 (любой IP адрес)
	Laddr.sin_port = htons(PORT);
	if (bind(Lsock, (sockaddr*)&Laddr, sizeof(Laddr))) {
		cout << "BIND ERROR:" << WSAGetLastError();
		closesocket(Lsock);
		WSACleanup();
		return -1;
	}
	// шаг 4 обработка пакетов, присланных клиентами
	while (true) {

		data_request request;
		sockaddr_in Caddr;
		int Caddr_size = sizeof(Caddr);
		int bsize = recvfrom(Lsock, (char*)&request, sizeof(request), 0, (sockaddr*)&Caddr, &Caddr_size);
		
		if (bsize == SOCKET_ERROR) cout << "RECVFROM() ERROR:" << WSAGetLastError();
		
		// Определяем IP-адрес клиента и прочие атрибуты
		HOSTENT* hst;
		hst = gethostbyaddr((char*)&Caddr.sin_addr, 4, AF_INET);
		cout << "NEW DATAGRAM!\n" << ((hst) ? hst->h_name : "Unknown host") << "\n" <<
		inet_ntoa(Caddr.sin_addr) << "\n" << ntohs(Caddr.sin_port) << '\n';

		cout << "Name: " << request.Name << "\n";
		cout << "Term: " << request.Current_Term << "\n";
		cout << "Good Marks: " << request.Good_Marks << "\n";
		cout << "Bad Marks: " << request.Bad_Marks << "\n";
		
        // посылка датаграммы клиенту
		data_response response; 
		response.FPP = Freebie_Power(request);
		sendto(Lsock, (char*)&response, sizeof(response), 0, (sockaddr*)&Caddr, sizeof(Caddr));
	}
	return 0;
}