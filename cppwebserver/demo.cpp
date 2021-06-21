#include "demo.h"


//一个简单的例子，替换所有404页面

//抓取所有404页面
bool myProcessor::route(HttpRequest x, HttpResponse r) {
	if (r.getStatusCode() == "404") {
		return true;
	}
	else {
		return false;
	}
}

//将返回页面替换成自定义的
HttpResponse myProcessor::deal(HttpRequest x, HttpResponse r) {
	HttpResponse h = r;

	string path = "C:/MyProject/cppweb/cppwebserver/Debug/404.html";

	FILE *file;
	errno_t err = fopen_s(&file, path.c_str(), "rb+");
	if (err != 0) {
		//文件不存在
		return HttpTemplate::http404();
	}
	else {
		string b;
		char *text;
		fseek(file, 0, SEEK_END);
		long lSize = ftell(file);
		text = (char*)malloc(lSize + 1);
		rewind(file);
		fread(text, sizeof(char), lSize, file);
		//text[lSize] = '\0';
		b.assign(text, lSize);
		h.setBody(b);
		//检查文件类型,设定头部
		h.addHeader(checkContentType(path));
		fclose(file);
		return h;
	}

}