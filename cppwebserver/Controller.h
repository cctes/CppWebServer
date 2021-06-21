//控制器，每个请求的处理总单元

#ifndef CONTROLLER
#define CONTROLLER
#define SET_UTF8 "SetConsoleOutputCP(65001)&"
#include "processor.h"
#include "base64.h"
#include "fcgi.h"
#include "config.h"


class BaseController {
protected:
	DefaultProcessor DP;
	vector<Processor*> SP;
public:
	BaseController();
	~BaseController();
	HttpResponse OneConcention(HttpRequest x);
	bool RegisterSProcessor(Processor *p);

};


class Controller : public BaseController {
public:
	Controller();
};




#endif