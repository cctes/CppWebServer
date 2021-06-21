
#ifndef PROCESSOR
#define PROCESSOR
#include "myhttp.h"

//�Զ��崦�����ӿ�
//route �ж��Ƿ��ɵ�ǰ����������
//������ʱ����deal
//deal ���崦��ĺ���
class Processor {
public:
	virtual bool route(HttpRequest x, HttpResponse r) = 0 {};
	virtual HttpResponse deal(HttpRequest x, HttpResponse r) = 0 {};
};

//ҳ�滺��ṹ��δʵ��
class Cache {
private:
	string url;
	string text;
public:
	string getURL();
	bool setURL(string u);
	string getText();
	bool setText(string t);
};

//��̬������
class DefaultProcessor{
private:
	vector<Cache> cache;
public:
	DefaultProcessor();
	~DefaultProcessor();
	HttpResponse route(HttpRequest x);

	HttpResponse deal(HttpRequest x);
};


#endif