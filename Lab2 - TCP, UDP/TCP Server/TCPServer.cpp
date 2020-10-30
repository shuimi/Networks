#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <locale.h>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>
#pragma comment (lib, "Ws2_32.lib")  
using namespace std;

#define SRV_PORT 1234
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

int main() {

	char buff[1024];
	if (WSAStartup(0x0202, (WSADATA*)&buff[0])) {
		cout << "Error WSAStartup \n" << WSAGetLastError();   // Ошибка!
		return -1;
	}

	SOCKET sock, sock_new;

	sockaddr_in sin, from_sin;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(SRV_PORT);
	
	bind(sock, (sockaddr*)&sin, sizeof(sin));
	
	string msg, msg1;

	listen(sock, 3); //создаёт очередь клиентов, 3 штуки

	while (true) {
		int from_length = sizeof(from_sin);
		sock_new = accept(sock, (sockaddr*)&from_sin, &from_length); //приняли клиента
		cout << "|server_log| new connected client! \n" << endl;

		while (true) {

			data_request request;
			recv(sock_new, (char*)&request, sizeof(data_request), 0);
			if (request.Name[0] == 'e' && request.Current_Term == 0) break;

			cout << "Name: " << request.Name << "\n";
			cout << "Term: " << request.Current_Term << "\n";
			cout << "Good Marks: " << request.Good_Marks << "\n";
			cout << "Bad Marks: " << request.Bad_Marks << "\n";

			data_response response;
			response.FPP = Freebie_Power(request);
			cout << response.FPP << "\n";

			send(sock_new, (char*)&response, sizeof(data_response), 0);
		
		}
		
		closesocket(sock_new);
		cout << "|server_log| client is lost\n";
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}
