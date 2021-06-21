//httpЭ��ṹ����
//
#ifndef MY_HTTP
#define MY_HTTP

#define HTTP_HEADER_NULL HttpHeader("null","null")


#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <ctime>
#include <stdio.h>



using namespace std;
//http���� header�ṹ
class HttpHeader{
private:
	string key;
	string value;
public:
	HttpHeader(string k, string v,vector<string> s);
	HttpHeader(char* k, char* v);
	HttpHeader(const char* k, const char* v);
	HttpHeader();
	~HttpHeader();
	string toString();
	string getKey();
	string getValue();
	void reSet(string k, string v);
	void reSet(const char* k, const char* v);
	bool toHeader(string x);
	bool toHeader(const char* x);
	bool operator==(HttpHeader a);
	bool isNull();

};

//http���ĵĻ���
class HttpMSG {
protected:
	string firstLine;
	vector<HttpHeader> header;
	string body;
public:
	HttpMSG(string f,vector<HttpHeader> h,string b);
	HttpMSG();
	~HttpMSG();
	void setFirstLine(string f);
	void setFirstLine(const char* f);
	void setHeader(vector<HttpHeader> h);
	void setBody(string b);
	void setBody(const char* b);
	bool addHeader(HttpHeader h);
	bool addHeader(string x);
	bool deleteHeaderByKey(string k);
	bool deleteHeaderByKey(const char* k);
	vector<HttpHeader> getAllHeader();
	HttpHeader getHeaderByKey(string k);
	string getFirstLine();
	string getBody();
	string toString();
	void addBody(string x);
	void addBody(const char* b);
	bool toMSG(string x);
	int setLength();
	int getLength();

};

//http��������
class HttpRequest :public HttpMSG{
private:
	string requestMethod;
	string URL;
	string httpVersion;
public:
	HttpRequest(string f, vector<HttpHeader> h, string b);
	HttpRequest();
	~HttpRequest();
	bool setAttribute();
	void setFirstLine(string f);
	void setFirstLine(const char* f);
	string getRequestMethod();
	string getURL();
	string getHttpVersion();
	bool setRequestMethod(string m);
	bool setURL(string u);
	bool setHttpVersion(string v);
	bool toMSG(string x);
	string getQueryString();

};

//http���ر�����
class HttpResponse: public HttpMSG {
private:
	string httpVersion;
	string statusCode;
	string status;
public:
	HttpResponse(string f, vector<HttpHeader> h, string b);
	HttpResponse();
	~HttpResponse();
	bool setAttribute();
	void setFirstLine(string f);
	void setFirstLine(const char* f);
	string getHttpVersion();
	string getStatusCode();
	string getStatus();
	bool setHttpVersion(string v);
	bool setStatusCode(string c);
	bool setStatus(string s);
	bool toMSG(string x);
};

//����ģ��
class HttpTemplate {
public:
	
	static HttpResponse http200();
	static HttpResponse http404();
	static HttpResponse http500();
	
};
//��ȡ��������ʱ���http��ʽ
string getDate();
//��鱨������
string checkContentType(string x);
//ͨ��url��ȡ���ڷ�������·��
string getPathByURL(string u);
#endif

