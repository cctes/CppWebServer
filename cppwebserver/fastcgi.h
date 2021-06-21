
#ifndef FASTCGI_H
#define FASTCGI_H


#define FCGI_ERROR -1
#define FCGI_NOERROR 0

typedef struct
{
	unsigned char version;     //�汾
	unsigned char type;        //��������
	unsigned char requestIdB1; //����id
	unsigned char requestIdB0;
	unsigned char contentLengthB1; //���ݳ���
	unsigned char contentLengthB0;
	unsigned char paddingLength; //����ֽڳ���
	unsigned char reserved;      //�����ֽ�
} FCGI_Header;                   //��Ϣͷ

//�����͵�������� 65536
#define FCGI_MAX_LENGTH 0xffff

//���� FCGI_Header ����
#define FCGI_HEADER_LEN 8

//FCGI�İ汾
#define FCGI_VERSION_1 1

// FCGI_Header �� type �ľ���ֵ
#define FCGI_BEGIN_REQUEST 1 //��ʼ����
#define FCGI_ABORT_REQUEST 2 //�쳣��ֹ����
#define FCGI_END_REQUEST 3   //������ֹ����
#define FCGI_PARAMS 4        //���ݲ���
#define FCGI_STDIN 5         //POST ���ݴ���
#define FCGI_STDOUT 6        //������Ӧ����
#define FCGI_STDERR 7        //�������
#define FCGI_DATA 8
#define FCGI_GET_VALUES 9
#define FCGI_GET_VALUES_RESULT 10
#define FCGI_UNKNOWN_TYPE 11 //֪ͨ webserver ������ type ����������
#define FCGI_MAXTYPE (FCGI_UNKNOWN_TYPE)

//�յ����� ID
#define FCGI_NULL_REQUEST_ID 0

/*******************************************/

typedef struct
{
	unsigned char roleB1; //webserver ������php-fpm ���ݵĽ�ɫ������ȡֵ������
	unsigned char roleB0;
	unsigned char flags;       //ȷ�� php-fpm ������һ������֮���Ƿ�ر�
	unsigned char reserved[5]; //�����ֶ�
} FCGI_BeginRequestBody;       //��ʼ������

typedef struct
{
	FCGI_Header header;         //��Ϣͷ
	FCGI_BeginRequestBody body; //��ʼ������
} FCGI_BeginRequestRecord;      //������Ϣ--��ʼ

//webserver ���� php-fpm ���ݵĽ�ɫ(����php-fpm��ʲô)
#define FCGI_KEEP_CONN 1  //���Ϊ0����������Ӧ�þ͹رգ����򲻹ر�
#define FCGI_RESPONDER 1  //����http������������Ϣ��������http��Ӧ����������webserver��PARAMS��������
#define FCGI_AUTHORIZER 2 //������֤�Ļ������http����δ��֤����ر�����
#define FCGI_FILTER 3     //����web server �еĶ��������������������˺��http��Ӧ

/*******************************************/

typedef struct
{
	unsigned char appStatusB3; //����״̬��0Ϊ����
	unsigned char appStatusB2;
	unsigned char appStatusB1;
	unsigned char appStatusB0;
	unsigned char protocolStatus; //Э��״̬
	unsigned char reserved[3];
} FCGI_EndRequestBody; //������Ϣ��

typedef struct
{
	FCGI_Header header;       //����ͷ
	FCGI_EndRequestBody body; //������
} FCGI_EndRequestRecord;      //����������Ϣ

//���ֽ���״̬
#define FCGI_REQUEST_COMPLETE 0 //��������
#define FCGI_CANT_MPX_XONN 1
#define FCGI_OVERLOADED 2       //�ܾ�������Ӧ�ø�����
#define FCGI_UNKNOWN_ROLE 3     //webserver ָ����һ��Ӧ�ò���ʶ��Ľ�ɫ

#define FCGI_MAX_CONNS "FCGI_MAX_CONNS" //�ɽ��ܵĲ���������·�����ֵ

#define FCGI_MAX_REQS "FCGI_MAX_REQS" //�ɽ��ܲ�����������ֵ

#define FCGI_MPXS_CONNS "FCGI_MPXS_CONNS" //�Ƿ��·���ã���״ֵ̬Ҳ��ͬ

/*******************************************/

typedef struct
{
	unsigned char type;
	unsigned char reserved[7];
} FCGI_UnknownTypeBody;

typedef struct
{
	FCGI_Header header;
	FCGI_UnknownTypeBody body;
} FCGI_UnKnownTypeRecord;



#endif
