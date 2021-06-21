//修改自https://github.com/gaojs/IOCPServer
//简单iocp

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <iostream>

#include "Controller.h"

#pragma comment(lib,"ws2_32")
#pragma comment(lib,"kernel32.lib")
#define IOCP_BUFF_SIZE 409600


using namespace std;

HANDLE g_hIOCP = 0; //全局的iocp 句柄
//char g_buffer[IOCP_BUFF_SIZE] = { 0 }; 

enum class IO_OPERATION
{
	IO_READ, IO_WRITE
};

struct IO_DATA
{//可以使用CONTAINING_RECORD，将OVERLAPPED转IO_DATA
	IO_OPERATION opCode;
	OVERLAPPED Overlapped;  //应该设为第一个，不明
	SOCKET client;  //客户的socket
	WSABUF wsabuf;
	int nBytes;
};


DWORD WINAPI worker(LPVOID context) {

	//在单个线程使用同一内存感觉不妥
	//当多个线程同时工作时，应该会导致内存覆盖的问题
	//故这里改为为每一个请求动态申请一块内存
	//后续可以改为使用环形缓存区来申请内存，提高效率
	//原文写成了全局变量，依旧不理解,估计是教学用
	//char g_buffer[IOCP_BUFF_SIZE] = { 0 }; 

	while (true)
	{
		DWORD dwIoSize = 0;
		void* lpCompletionKey = NULL;
		LPOVERLAPPED lpOverlapped = NULL;
		//唤醒函数，获取状态，当accept一个socket时由此函数返回唤醒当前线程
		/*
		BOOL WINAPI GetQueuedCompletionStatus(
			__in   HANDLE          CompletionPort,    // 这个就是我们建立的那个唯一的完成端口
			__out  LPDWORD         lpNumberOfBytes,   //这个是操作完成后返回的字节数
			__out  PULONG_PTR      lpCompletionKey,   // 这个是我们建立完成端口的时候绑定的那个自定义结构体参数
			__out  LPOVERLAPPED    *lpOverlapped,     // 这个是我们在连入Socket的时候一起建立的那个重叠结构
			__in   DWORD           dwMilliseconds     // 等待完成端口的超时时间，如果线程不需要做其他的事情，那就INFINITE就行了
		);*/
		BOOL bRet = GetQueuedCompletionStatus(g_hIOCP, &dwIoSize,
			(LPDWORD)&lpCompletionKey, (LPOVERLAPPED*)&lpOverlapped, INFINITE);

		//解析lpOverlapped 
		IO_DATA* lpIOContext = CONTAINING_RECORD(lpOverlapped, IO_DATA, Overlapped);
		if (dwIoSize == 0)
		{
			//连接失败，休眠
			int nRet = closesocket(lpIOContext->client);
			delete lpIOContext;
			continue;
		}

		//接收客户端数据
		//在主进程accept之后，绑定socket到iocp，设置opCode为read并调用wsarec来接受数据
		//接收数据完成后唤醒线程来处理,此时是由线程的read来处理
		//线程的read对接收的数据进行处理，完成之后将opCode设置为write，并调用wsasend进行发送数据
		//当send发送完毕后，iocp唤醒一个线程，该线程的write部分将opcode重新设置为read
		//并调用wasrec来接收数据，之后就再次回到read部分

		//在通信的过程中，通过对每个socket的OVERLAPPED的opcode进行不断设置，
		//从而告诉线程当前已经完成的是数据的读取操作还是发送操作，
		//每次读取到的数据或者一些该socket的信息通过OVERLAPPED进行传递
		//因此即使一个socket是由多个线程依次处理，但其信息依旧可以在线程之间共享
		//对于每一个socket而言，对其的处理可以抽象为是一个同步过程（存疑）
		//OVERLAPPED:重叠结构
		if (lpIOContext->opCode == IO_OPERATION::IO_READ)
		{// a read operation complete

			//处理报文
			//string msg = lpIOContext->wsabuf.buf;
			string res;
			Controller controller = Controller();
			HttpRequest request = HttpRequest();
			request.toMSG(lpIOContext->wsabuf.buf);
			HttpResponse response = controller.OneConcention(request);
			res = response.toString();

			ZeroMemory(&lpIOContext->Overlapped,
				sizeof(lpIOContext->Overlapped));

			//memcpy(g_buffer, res.c_str(), res.length());
			//lpIOContext->wsabuf.buf = g_buffer;

			//这一步必要性存疑，应该是不需要的
			//memset(lpIOContext->wsabuf.buf, 0, sizeof(char)*IOCP_BUFF_SIZE);
			memcpy(lpIOContext->wsabuf.buf, res.c_str(), res.length());

			lpIOContext->wsabuf.len = res.length();
			lpIOContext->opCode = IO_OPERATION::IO_WRITE;
			lpIOContext->nBytes = res.length();
			DWORD dwFlags = 0;
			DWORD nBytes = sizeof(char)*IOCP_BUFF_SIZE + 1;
			//这里的send是否立即返回？
			//如果不是立即返回，那是否意味着会存在两个线程同时对一个socket进行处理
			//
			int nRet = WSASend(lpIOContext->client,
				&lpIOContext->wsabuf, 1, &nBytes,
				dwFlags, &(lpIOContext->Overlapped), NULL);

			if (nRet == SOCKET_ERROR)
			{
				int nErr = WSAGetLastError();
				if (ERROR_IO_PENDING != nErr)
				{
					//不是很能理解为啥在判断一步，先不动
					nRet = closesocket(lpIOContext->client);
					delete lpIOContext->wsabuf.buf;
					delete lpIOContext;
					continue;
				}
			}
			//memset(g_buffer, NULL, sizeof(g_buffer));
		}
		//接收服务端的数据
		else if (lpIOContext->opCode == IO_OPERATION::IO_WRITE)
		{//a write operation complete
			// Write operation completed, so post Read operation.

			
			closesocket(lpIOContext->client);
			delete lpIOContext->wsabuf.buf;
			delete lpIOContext;
			//没有tcp复用，所以在传输完成之后直接关闭
			/*
			DWORD dwFlags = 0;
			DWORD nBytes = sizeof(g_buffer);
			lpIOContext->opCode = IO_OPERATION::IO_READ;
			lpIOContext->wsabuf.buf = g_buffer;
			lpIOContext->wsabuf.len = nBytes;
			lpIOContext->nBytes = nBytes;
			ZeroMemory(&lpIOContext->Overlapped,
				sizeof(lpIOContext->Overlapped));
			int nRet = WSARecv(lpIOContext->client,
				&lpIOContext->wsabuf, 1, &nBytes,
				&dwFlags, &(lpIOContext->Overlapped), NULL);
			if (nRet == SOCKET_ERROR)
			{
				int nErr = WSAGetLastError();
				if (ERROR_IO_PENDING != nErr)
				{
					int nRet = closesocket(lpIOContext->client);
					delete lpIOContext;
					continue;
				}
			}
			*/


		}
	}
	return 0;
}

int GetCpuCoreCount()
{
	SYSTEM_INFO sysInfo = { 0 };
	GetSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
}

int main()
{
	init();
	//char *g_buffer;
	int nThreadCount = GetCpuCoreCount() * 2;
	cout << "nThreadCount=" << nThreadCount << endl;

	WSADATA wsaData = { 0 };
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	cout << "WSAStartup() nRet=" << nRet << endl;
	if (nRet != NO_ERROR)
	{
		int nErr = WSAGetLastError();
		return 1;
	}
	SOCKET hSocket = WSASocket(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	cout << "WSASocket() hSocket=" << hex << hSocket << endl;

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(PORT.c_str()));
	server.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	nRet = bind(hSocket, (sockaddr*)&server, sizeof(server));
	cout << "bind() nRet=" << nRet << endl;
	nRet = listen(hSocket, nThreadCount);
	cout << "listen() nRet=" << nRet << endl;

	g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
		NULL, 0, nThreadCount); //dwCompletionKey, 0)
	cout << "CreateIOCP() hIOCP=" << hex << g_hIOCP << endl;
	for (int i = 0; i < nThreadCount; ++i)
	{
		HANDLE hThread = 0;
		DWORD dwThreadId = 0;
		hThread = CreateThread(NULL, 0, worker, 0, 0, &dwThreadId);
		cout << "CreateThread() hThread=" << hex << hThread;
		cout << ", dwThreadId=" << dec << dwThreadId << endl;
		if (hThread)
		{
			CloseHandle(hThread);
		}
	}


	//主循环
	while (hSocket)
	{
		SOCKET hClient = accept(hSocket, NULL, NULL);
		HANDLE hIocpTemp = CreateIoCompletionPort((HANDLE)hClient,
			g_hIOCP, hClient, 0); //dwNumberOfConcurrentThreads
		if (hIocpTemp == NULL)
		{
			int nErr = WSAGetLastError();
			cout << "Bind Socket2IOCP Failed! nErr=" << nErr << endl;
			nRet = closesocket(hClient);
			cout << "closesocket() nRet=" << nRet << endl;
			break; //不退出，会一直失败
		}
		else
		{ //post a recv request
			//在这里先进行一次接收
			IO_DATA* data = new IO_DATA;
			memset(data, 0, sizeof(IO_DATA));
			data->nBytes = 0;
			data->opCode = IO_OPERATION::IO_READ;
			//为每个连接申请一块内存
			//不太清楚每次连接使用的内存大小 主要是发送的步分
			//先试用定长
			data->wsabuf.buf = new char[IOCP_BUFF_SIZE];
			data->wsabuf.len = sizeof(char)*IOCP_BUFF_SIZE;
			data->client = hClient;
			DWORD nBytes = sizeof(char)*IOCP_BUFF_SIZE, dwFlags = 0;
			int nRet = WSARecv(hClient, &data->wsabuf, 1,
				&nBytes, &dwFlags, &(data->Overlapped), NULL);
			if (nRet == SOCKET_ERROR)
			{
				int nErr = WSAGetLastError();
				if (ERROR_IO_PENDING != nErr)
				{
					//cout << "WASRecv Failed! nErr=" << nErr << endl;
					nRet = closesocket(hClient);
					//cout << "closesocket() nRet=" << nRet << endl;
					delete data;
				}
			}

		}
	}
	nRet = closesocket(hSocket);
	cout << "closesocket() nRet=" << nRet << endl;
	nRet = WSACleanup();
	cout << "WSACleanup() nRet=" << nRet << endl;
	system("pause");
	return 0;
}
