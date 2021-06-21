//fcgi ʵ��
//�޸���https://codeload.github.com/liushengxi13689209566/WebServer
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "fastcgi.h"
#include "fcgi.h"


#include <assert.h>
#include <errno.h>

#define PARAMS_BUFF_LEN  2048 //��������buffer�Ĵ�С
#define CONTENT_BUFF_LEN 2048 //����buffer�Ĵ�С
#define bzero(a,b) memset(a,0,b)


void FastCgi_init(FastCgi_t *c)
{
	c->sockfd_ = 0;    //��php-fpm ������ sockfd
	c->flag_ = 0;      //record �������ID
	c->requestId_ = 0; //������־��ǰ��ȡ�����Ƿ�Ϊhtml����
}

void FastCgi_finit(FastCgi_t *c)
{
	closesocket(c->sockfd_);
}

void setRequestId(FastCgi_t *c, int requestId)
{
	c->requestId_ = requestId;
}

FCGI_Header makeHeader(int type, int requestId,
	int contentLength, int paddingLength)
{
	FCGI_Header header;

	header.version = FCGI_VERSION_1;

	header.type = (unsigned char)type;

	/* �����ֶα�������ID */
	header.requestIdB1 = (unsigned char)((requestId >> 8) & 0xff);
	header.requestIdB0 = (unsigned char)(requestId & 0xff);

	/* �����ֶα������ݳ��� */
	header.contentLengthB1 = (unsigned char)((contentLength >> 8) & 0xff);
	header.contentLengthB0 = (unsigned char)(contentLength & 0xff);

	/* ����ֽڵĳ��� */
	header.paddingLength = (unsigned char)paddingLength;

	/* �����ֽڸ�Ϊ 0 */
	header.reserved = 0;

	return header;
}

FCGI_BeginRequestBody makeBeginRequestBody(int role, int keepConnection)
{
	FCGI_BeginRequestBody body;

	/* �����ֽڱ������� php-fpm ���ݵĽ�ɫ */
	body.roleB1 = (unsigned char)((role >> 8) & 0xff);
	body.roleB0 = (unsigned char)(role & 0xff);

	/* ����0�����ӣ���������� */
	body.flags = (unsigned char)((keepConnection) ? FCGI_KEEP_CONN : 0);

	bzero(&body.reserved, sizeof(body.reserved));

	return body;
}

int makeNameValueBody(char *name, int nameLen,
	char *value, int valueLen,
	unsigned char *bodyBuffPtr, int *bodyLenPtr)
{
	/* ��¼ body �Ŀ�ʼλ�� */
	unsigned char *startBodyBuffPtr = bodyBuffPtr;

	/* ��� nameLen С��128�ֽ� */
	if (nameLen < 128)
	{
		*bodyBuffPtr++ = (unsigned char)nameLen; //nameLen��1���ֽڱ���
	}
	else
	{
		/* nameLen �� 4 ���ֽڱ��� */
		*bodyBuffPtr++ = (unsigned char)((nameLen >> 24) | 0x80);
		*bodyBuffPtr++ = (unsigned char)(nameLen >> 16);
		*bodyBuffPtr++ = (unsigned char)(nameLen >> 8);
		*bodyBuffPtr++ = (unsigned char)nameLen;
	}

	/* valueLen С�� 128 ����һ���ֽڱ��� */
	if (valueLen < 128)
	{
		*bodyBuffPtr++ = (unsigned char)valueLen;
	}
	else
	{
		/* valueLen �� 4 ���ֽڱ��� */
		*bodyBuffPtr++ = (unsigned char)((valueLen >> 24) | 0x80);
		*bodyBuffPtr++ = (unsigned char)(valueLen >> 16);
		*bodyBuffPtr++ = (unsigned char)(valueLen >> 8);
		*bodyBuffPtr++ = (unsigned char)valueLen;
	}

	/* �� name �е��ֽ���һ����body�е�buffer�� */
	for (int i = 0; i < strlen(name); i++)
	{
		*bodyBuffPtr++ = name[i];
	}

	/* �� value �е�ֵ��һ����body�е�buffer�� */
	for (int i = 0; i < strlen(value); i++)
	{
		*bodyBuffPtr++ = value[i];
	}

	/* ����� body �ĳ��� */
	*bodyLenPtr = bodyBuffPtr - startBodyBuffPtr;
	return 1;
}


int startConnect(FastCgi_t *c)
{
	WSADATA  foo;
	WSAStartup(0x0202, &foo);
	const char *ip = "127.0.0.1";

	//linux �汾������
	/*
	int rc;
	int sockfd;
	struct sockaddr_in server_address;

	
	bzero(&server_address, sizeof(server_address));

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd > 0);
	
	server_address.sin_family = AF_INET;
	//inet_pton(AF_INET,ip, server_address.sin_addr.S_un.S_addr);
	server_address.sin_addr.S_un.S_addr = inet_addr(ip);
	server_address.sin_port = htons(9000);
	rc = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
	assert(rc >= 0);
	*/




	SOCKET s = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(9000);

	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	//inet_pton(AF_INET, ip, addr.sin_addr.S_un.S_addr);

	int i = connect(s, (const sockaddr*)&addr, sizeof(SOCKADDR_IN));

	c->sockfd_ = s;
	if (i != NO_ERROR) {
		return FCGI_ERROR;
	}
	return FCGI_NOERROR;
}
int sendStartRequestRecord(FastCgi_t *c)
{
	int rc;
	FCGI_BeginRequestRecord beginRecord;

	beginRecord.header = makeHeader(FCGI_BEGIN_REQUEST, c->requestId_, sizeof(beginRecord.body), 0);
	beginRecord.body = makeBeginRequestBody(FCGI_RESPONDER, 0);

	rc = send(c->sockfd_, (char *)&beginRecord, sizeof(beginRecord),0);
	if (rc != sizeof(beginRecord)) {
		return FCGI_ERROR;
	}

	return FCGI_NOERROR;
}

int sendParams(FastCgi_t *c, char *name, char *value)
{
	int rc;

	unsigned char bodyBuff[PARAMS_BUFF_LEN];

	bzero(bodyBuff, sizeof(bodyBuff));

	/* ���� body �ĳ��� */
	int bodyLen;

	/* ���� PARAMS �������ݵ� body */
	makeNameValueBody(name, strlen(name), value, strlen(value), bodyBuff, &bodyLen);

	FCGI_Header nameValueHeader;
	nameValueHeader = makeHeader(FCGI_PARAMS, c->requestId_, bodyLen, 0);
	/*8 �ֽڵ���Ϣͷ*/

	int nameValueRecordLen = bodyLen + FCGI_HEADER_LEN;
	char *nameValueRecord = (char *)malloc(nameValueRecordLen * sizeof(char));

	/* ��ͷ��body������һ��buffer ��ֻ�����һ��write */
	memcpy(nameValueRecord, (char *)&nameValueHeader, FCGI_HEADER_LEN);
	memcpy(nameValueRecord + FCGI_HEADER_LEN, bodyBuff, bodyLen);

	rc = send(c->sockfd_, nameValueRecord, nameValueRecordLen,0);
	free(nameValueRecord);
	if (rc != nameValueRecordLen) {
		return FCGI_ERROR;
	}

	return FCGI_NOERROR;
}

int sendParams(FastCgi_t *c, const char *name, const char *value) {
	char cname[100];//�㹻��
	char cvalue[100];
	strcpy_s(cname, name);
	strcpy_s(cvalue, value);
	return sendParams(c,cname,cvalue);
}

int sendEndRequestRecord(FastCgi_t *c)
{
	int rc;

	FCGI_Header endHeader;
	endHeader = makeHeader(FCGI_PARAMS, c->requestId_, 0, 0);

	rc = send(c->sockfd_, (char *)&endHeader, FCGI_HEADER_LEN,0);
	assert(rc == FCGI_HEADER_LEN);

	return 1;
}

string readFromPhp(FastCgi_t *c)
{
	FCGI_Header responderHeader;
	char Hbuf[128];
	char content[CONTENT_BUFF_LEN];

	int contentLen;
	char tmp[8]; //�����ݴ�padding�ֽ�
	int ret;

	/* �Ƚ�ͷ�� 8 ���ֽڶ����� */
	while (recv(c->sockfd_, Hbuf, FCGI_HEADER_LEN,0) > 0)
	{
		memcpy(&responderHeader, Hbuf, FCGI_HEADER_LEN);

		if (responderHeader.type == FCGI_STDOUT)
		{
			/* ��ȡ���ݳ��� */
			contentLen = (responderHeader.contentLengthB1 << 8) + (responderHeader.contentLengthB0);
			bzero(content, CONTENT_BUFF_LEN);

			/* ��ȡ��ȡ���� */
			ret = recv(c->sockfd_, content, contentLen,0);
			// printf("ret ==  %d\n", ret);

			assert(ret == contentLen);

			/*test*/
			// printf("content == %s \n", content);

			/* ������䲿�� */
			if (responderHeader.paddingLength > 0)
			{
				ret = recv(c->sockfd_, tmp, responderHeader.paddingLength,0);
				assert(ret == responderHeader.paddingLength);
			}
		} //end of type FCGI_STDOUT
		else if (responderHeader.type == FCGI_STDERR)
		{
			contentLen = (responderHeader.contentLengthB1 << 8) + (responderHeader.contentLengthB0);
			bzero(content, CONTENT_BUFF_LEN);

			ret = recv(c->sockfd_, content, contentLen,0);
			assert(ret == contentLen);

			fprintf(stdout, "error:%s\n", content);

			/* ������䲿�� */
			if (responderHeader.paddingLength > 0)
			{
				ret = recv(c->sockfd_, tmp, responderHeader.paddingLength,0);
				assert(ret == responderHeader.paddingLength);
			}
		} // end of type FCGI_STDERR
		else if (responderHeader.type == FCGI_END_REQUEST)
		{
			FCGI_EndRequestBody endRequest;
			
			ret = recv(c->sockfd_, Hbuf, sizeof(endRequest),0);
			memcpy(&endRequest, Hbuf, sizeof(endRequest));
			assert(ret == sizeof(endRequest));
		}
		memset(&Hbuf, 0, sizeof(Hbuf));
	}
	string con = content;
	return con;
}

char *findStartHtml(char *p)
{
	enum
	{
		S_NOPE,
		S_CR,
		S_CRLF,
		S_CRLFCR,
		S_CRLFCRLF
	} state = S_NOPE;

	for (char *content = p; *content != '\0'; content++) //״̬��
	{
		switch (state)
		{
		case S_NOPE:
			if (*content == '\r')
				state = S_CR;
			break;
		case S_CR:
			state = (*content == '\n') ? S_CRLF : S_NOPE;
			break;
		case S_CRLF:
			state = (*content == '\r') ? S_CRLFCR : S_NOPE;
			break;
		case S_CRLFCR:
			state = (*content == '\n') ? S_CRLFCRLF : S_NOPE;
			break;
		case S_CRLFCRLF:
			return content;
		}
	}
	// fprintf(stderr, "%%%%%%%%%%RETURN NULL!!!!!\n");
	return p;
}
void getHtmlFromContent(FastCgi_t *c, char *content)
{
	/* ����html���ݿ�ʼλ�� */
	char *pt;

	/* ��ȡ����content��html���� */
	if (c->flag_ == 1)
	{
		printf("%s", content);
	}
	else
	{
		if ((pt = findStartHtml(content)) != NULL)
		{
			c->flag_ = 1;
			for (char *i = pt; *i != '\0'; i++)
			{
				printf("%c", *i);
			}
		}
	}
}