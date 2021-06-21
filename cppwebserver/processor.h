
#ifndef PROCESSOR
#define PROCESSOR
#include "myhttp.h"

//自定义处理器接口
//route 判断是否由当前处理器处理
//返回真时调用deal
//deal 具体处理的函数
class Processor {
public:
	virtual bool route(HttpRequest x, HttpResponse r) = 0 {};
	virtual HttpResponse deal(HttpRequest x, HttpResponse r) = 0 {};
};

//页面缓存结构，未实现
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

//静态处理器
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