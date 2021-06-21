//�޸���https://codeload.github.com/liushengxi13689209566/WebServer
#ifndef FCGI_H
#define FCGI_H

#include "fastcgi.h"
#include <winsock2.h> // all socket routines are defined in this file
#include <windows.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <string>
#pragma comment(lib,"ws2_32")
using namespace std;

typedef struct
{
	SOCKET sockfd_;    //��php-fpm ������ sockfd
	int requestId_; //record �������ID
	int flag_;      //������־��ǰ��ȡ�����Ƿ�Ϊhtml����

} FastCgi_t;

void FastCgi_init(FastCgi_t *c);

void FastCgi_finit(FastCgi_t *c);

//��������Id
void setRequestId(FastCgi_t *c, int requestId);

//����ͷ��
FCGI_Header makeHeader(int type, int request,
	int contentLength, int paddingLength);

//���ɷ��������������
FCGI_BeginRequestBody makeBeginRequestBody(int role, int keepConnection);

//���� PARAMS �� name-value body
int makeNameValueBody(char *name, int nameLen,
	char *value, int valueLen,
	unsigned char *bodyBuffPtr, int *bodyLen);


//����php-fpm������ɹ��򷵻ض�Ӧ���׽���������
int startConnect(FastCgi_t *c);

//���Ϳ�ʼ�����¼
int sendStartRequestRecord(FastCgi_t *c);

//��php-fpm����name-value������
int sendParams(FastCgi_t *c, char *name, char *value);
int sendParams(FastCgi_t *c, const char *name, const char *value);

//���ͽ���������Ϣ
int sendEndRequestRecord(FastCgi_t *c);

//ֻ��php-fpm �������ݣ����������ݴ�����������
string readFromPhp(FastCgi_t *c);

char *findStartHtml(char *content);

/*test*/
void getHtmlFromContent(FastCgi_t *c, char *content);

#endif

