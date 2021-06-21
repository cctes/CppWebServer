#include "processor.h"
#include <fstream>


/*HttpResponse Processor::deal(HttpRequest x, HttpResponse r) {
	return r;
}

bool Processor::route(HttpRequest x, HttpResponse r) {
	return false;
}*/

string Cache::getText() {
	return text;
}

string Cache::getURL() {
	return url;
}

bool Cache::setText(string t) {
	text = t;
	return true;
}

bool Cache::setURL(string u) {
	url = u;
	return true;
}



DefaultProcessor::DefaultProcessor() {

}

DefaultProcessor::~DefaultProcessor() {

}

HttpResponse DefaultProcessor::route(HttpRequest x) {
	string path = getPathByURL(x.getURL());

	//·��ƴ����ɣ���ȡ�ļ�
	FILE *file;
	errno_t err = fopen_s(&file,path.c_str(), "rb+");
	if (err != 0) {
		//�ļ�������
		return HttpTemplate::http404();
	}
	else {
		//�ļ�����
		HttpResponse h = HttpTemplate::http200();
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
		//����ļ�����,�趨ͷ��
		h.addHeader(checkContentType(path));
		fclose(file);
		return h;
	}
}

HttpResponse DefaultProcessor::deal(HttpRequest x) {
	return HttpResponse();
}
