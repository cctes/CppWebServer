//简单多线程，弃用
/*
#include <iostream>
#include <string>
#include <Windows.h>
#include <stdlib.h>	  
#include <stdio.h>	
#include <assert.h>
#include "Controller.h"
#include "config.h"


#pragma comment(lib,"ws2_32")



using namespace std;



static DWORD CALLBACK oneConnection(LPVOID lp) {
	SOCKET fd = (SOCKET)(LPVOID)lp;
	char *buffer = new char[40960];
	while (true) {
		int i = recv(fd, buffer, 40959, 0);
		if (i <= 0) {
			break;
		}
		buffer[i] = '\0';
		HttpRequest x = HttpRequest();
		x.toMSG(buffer);
		Controller c = Controller();
		HttpResponse r = c.OneConcention(x);
		string rs = r.toString();
		send(fd, rs.c_str(),rs.length() , 0);
		closesocket(fd);
	}
	delete(buffer);

	return 0;
}


void netWorkFunc() {
	WSADATA  foo;
	WSAStartup(0x0202, &foo);


	SOCKET s = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(PORT.c_str()));
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	int i = bind(s, (const sockaddr*)&addr, sizeof(SOCKADDR_IN));
	if (i != NO_ERROR) {
		std::cout << "server start faild" << std::endl;
		closesocket(s);
		exit(-1);
	}

	listen(s, 200);
	while (true)
	{
		i = sizeof(SOCKADDR_IN);
		SOCKET connect = accept(s, (sockaddr*)&addr, &i);
		if (connect != INVALID_SOCKET) {

			HANDLE h = CreateThread(NULL, 0, oneConnection, (LPVOID)connect, 0, NULL);
			if (h == NULL) {
				std::cout << "thread create err" << std::endl;
				break;
			}
			//ok
		}
		else {
			std::cout << "a connect err" << std::endl;
		}

	}



}



int mainxx()
{
	init();
	SetConsoleOutputCP(65001);
	
	phpCGIProcess();
	netWorkFunc();
	return 0;

}

*/