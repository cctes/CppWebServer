//定制化开发的例子
#pragma once
#include"Controller.h"

class myProcessor :public Processor {

	HttpResponse deal(HttpRequest x,HttpResponse r) override;
	bool route(HttpRequest x,HttpResponse r) override;
};
