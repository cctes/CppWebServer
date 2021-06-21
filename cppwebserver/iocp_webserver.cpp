//�޸���https://github.com/gaojs/IOCPServer
//��iocp

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

HANDLE g_hIOCP = 0; //ȫ�ֵ�iocp ���
//char g_buffer[IOCP_BUFF_SIZE] = { 0 }; 

enum class IO_OPERATION
{
	IO_READ, IO_WRITE
};

struct IO_DATA
{//����ʹ��CONTAINING_RECORD����OVERLAPPEDתIO_DATA
	IO_OPERATION opCode;
	OVERLAPPED Overlapped;  //Ӧ����Ϊ��һ��������
	SOCKET client;  //�ͻ���socket
	WSABUF wsabuf;
	int nBytes;
};


DWORD WINAPI worker(LPVOID context) {

	//�ڵ����߳�ʹ��ͬһ�ڴ�о�����
	//������߳�ͬʱ����ʱ��Ӧ�ûᵼ���ڴ渲�ǵ�����
	//�������ΪΪÿһ������̬����һ���ڴ�
	//�������Ը�Ϊʹ�û��λ������������ڴ棬���Ч��
	//ԭ��д����ȫ�ֱ��������ɲ����,�����ǽ�ѧ��
	//char g_buffer[IOCP_BUFF_SIZE] = { 0 }; 

	while (true)
	{
		DWORD dwIoSize = 0;
		void* lpCompletionKey = NULL;
		LPOVERLAPPED lpOverlapped = NULL;
		//���Ѻ�������ȡ״̬����acceptһ��socketʱ�ɴ˺������ػ��ѵ�ǰ�߳�
		/*
		BOOL WINAPI GetQueuedCompletionStatus(
			__in   HANDLE          CompletionPort,    // ����������ǽ������Ǹ�Ψһ����ɶ˿�
			__out  LPDWORD         lpNumberOfBytes,   //����ǲ�����ɺ󷵻ص��ֽ���
			__out  PULONG_PTR      lpCompletionKey,   // ��������ǽ�����ɶ˿ڵ�ʱ��󶨵��Ǹ��Զ���ṹ�����
			__out  LPOVERLAPPED    *lpOverlapped,     // ���������������Socket��ʱ��һ�������Ǹ��ص��ṹ
			__in   DWORD           dwMilliseconds     // �ȴ���ɶ˿ڵĳ�ʱʱ�䣬����̲߳���Ҫ�����������飬�Ǿ�INFINITE������
		);*/
		BOOL bRet = GetQueuedCompletionStatus(g_hIOCP, &dwIoSize,
			(LPDWORD)&lpCompletionKey, (LPOVERLAPPED*)&lpOverlapped, INFINITE);

		//����lpOverlapped 
		IO_DATA* lpIOContext = CONTAINING_RECORD(lpOverlapped, IO_DATA, Overlapped);
		if (dwIoSize == 0)
		{
			//����ʧ�ܣ�����
			int nRet = closesocket(lpIOContext->client);
			delete lpIOContext;
			continue;
		}

		//���տͻ�������
		//��������accept֮�󣬰�socket��iocp������opCodeΪread������wsarec����������
		//����������ɺ����߳�������,��ʱ�����̵߳�read������
		//�̵߳�read�Խ��յ����ݽ��д������֮��opCode����Ϊwrite��������wsasend���з�������
		//��send������Ϻ�iocp����һ���̣߳����̵߳�write���ֽ�opcode��������Ϊread
		//������wasrec���������ݣ�֮����ٴλص�read����

		//��ͨ�ŵĹ����У�ͨ����ÿ��socket��OVERLAPPED��opcode���в������ã�
		//�Ӷ������̵߳�ǰ�Ѿ���ɵ������ݵĶ�ȡ�������Ƿ��Ͳ�����
		//ÿ�ζ�ȡ�������ݻ���һЩ��socket����Ϣͨ��OVERLAPPED���д���
		//��˼�ʹһ��socket���ɶ���߳����δ���������Ϣ���ɿ������߳�֮�乲��
		//����ÿһ��socket���ԣ�����Ĵ�����Գ���Ϊ��һ��ͬ�����̣����ɣ�
		//OVERLAPPED:�ص��ṹ
		if (lpIOContext->opCode == IO_OPERATION::IO_READ)
		{// a read operation complete

			//������
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

			//��һ����Ҫ�Դ��ɣ�Ӧ���ǲ���Ҫ��
			//memset(lpIOContext->wsabuf.buf, 0, sizeof(char)*IOCP_BUFF_SIZE);
			memcpy(lpIOContext->wsabuf.buf, res.c_str(), res.length());

			lpIOContext->wsabuf.len = res.length();
			lpIOContext->opCode = IO_OPERATION::IO_WRITE;
			lpIOContext->nBytes = res.length();
			DWORD dwFlags = 0;
			DWORD nBytes = sizeof(char)*IOCP_BUFF_SIZE + 1;
			//�����send�Ƿ��������أ�
			//��������������أ����Ƿ���ζ�Ż���������߳�ͬʱ��һ��socket���д���
			//
			int nRet = WSASend(lpIOContext->client,
				&lpIOContext->wsabuf, 1, &nBytes,
				dwFlags, &(lpIOContext->Overlapped), NULL);

			if (nRet == SOCKET_ERROR)
			{
				int nErr = WSAGetLastError();
				if (ERROR_IO_PENDING != nErr)
				{
					//���Ǻ������Ϊɶ���ж�һ�����Ȳ���
					nRet = closesocket(lpIOContext->client);
					delete lpIOContext->wsabuf.buf;
					delete lpIOContext;
					continue;
				}
			}
			//memset(g_buffer, NULL, sizeof(g_buffer));
		}
		//���շ���˵�����
		else if (lpIOContext->opCode == IO_OPERATION::IO_WRITE)
		{//a write operation complete
			// Write operation completed, so post Read operation.

			
			closesocket(lpIOContext->client);
			delete lpIOContext->wsabuf.buf;
			delete lpIOContext;
			//û��tcp���ã������ڴ������֮��ֱ�ӹر�
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


	//��ѭ��
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
			break; //���˳�����һֱʧ��
		}
		else
		{ //post a recv request
			//�������Ƚ���һ�ν���
			IO_DATA* data = new IO_DATA;
			memset(data, 0, sizeof(IO_DATA));
			data->nBytes = 0;
			data->opCode = IO_OPERATION::IO_READ;
			//Ϊÿ����������һ���ڴ�
			//��̫���ÿ������ʹ�õ��ڴ��С ��Ҫ�Ƿ��͵Ĳ���
			//�����ö���
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
