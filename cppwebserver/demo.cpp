#include "demo.h"


//һ���򵥵����ӣ��滻����404ҳ��

//ץȡ����404ҳ��
bool myProcessor::route(HttpRequest x, HttpResponse r) {
	if (r.getStatusCode() == "404") {
		return true;
	}
	else {
		return false;
	}
}

//������ҳ���滻���Զ����
HttpResponse myProcessor::deal(HttpRequest x, HttpResponse r) {
	HttpResponse h = r;

	string path = "C:/MyProject/cppweb/cppwebserver/Debug/404.html";

	FILE *file;
	errno_t err = fopen_s(&file, path.c_str(), "rb+");
	if (err != 0) {
		//�ļ�������
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
		//����ļ�����,�趨ͷ��
		h.addHeader(checkContentType(path));
		fclose(file);
		return h;
	}

}