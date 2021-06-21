#include "myhttp.h"

HttpMSG::HttpMSG(string f, vector<HttpHeader> h, string b) {
	firstLine = f;
	header = h;
	body = b;
}

HttpMSG::HttpMSG(){
	firstLine = "";
	body = "";
}

HttpMSG::~HttpMSG(){
}

void HttpMSG::setFirstLine(string f) {
	firstLine = f;
}

void HttpMSG::setFirstLine(const char* f) {
	firstLine = string(f);
}

void HttpMSG::setHeader(vector<HttpHeader> h) {
	header = h;
}

void HttpMSG::setBody(string b) {
	body = b;
}

void HttpMSG::setBody(const char* b) {
	body = string(b);
}

void HttpMSG::addBody(string b) {
	body += b;
}

void HttpMSG::addBody(const char*b) {
	body += string(b);
}

bool HttpMSG::addHeader(HttpHeader h) {
	int s = header.size();
	if (s <= 0) {
		header.push_back(h);
		return true;
	}
	for (int i = 0; i < s; i++) {
		if (header[i].getKey() == h.getKey()) {
			return false;
		}
	}
	header.push_back(h);
	return true;
}

bool HttpMSG::addHeader(string x) {
	HttpHeader h = HttpHeader();
	h.toHeader(x);
	return addHeader(h);
}

bool HttpMSG::deleteHeaderByKey(string k) {
	for (vector<HttpHeader>::iterator it = header.begin(); it != header.end(); ++it) {
		if (it->getKey() == k) {
			header.erase(it);
			return true;
		}
	}
	return false;
}

bool HttpMSG::deleteHeaderByKey(const char* k) {
	return HttpMSG::deleteHeaderByKey(string(k));
}

vector<HttpHeader> HttpMSG::getAllHeader() {
	return header;
}

HttpHeader HttpMSG::getHeaderByKey(string k) {
	
	int s = header.size();
	if (s <= 0) {
		return HTTP_HEADER_NULL;
	}
	for (int i = 0; i <= s; i++) {
		if (header[i].getKey() == k) {
			return header[i];
		}
	}
	return HTTP_HEADER_NULL;
}

string HttpMSG::getFirstLine() {
	return firstLine;
}

string HttpMSG::getBody() {
	return body;
}

string HttpMSG::toString() {
	string r;
	r += firstLine;
	for (vector<HttpHeader>::iterator i = header.begin(); i != header.end(); ++i) {
		r += i->toString();
	}
	r += "\r\n";
	r += body;
	return r;
}

bool HttpMSG::toMSG(string x) {
	if (x.find("\r\n") < 0) {
		return false;
	}
	firstLine = x.substr(0,x.find("\r\n")+2);
	x = x.substr(x.find("\r\n")+2);

	int i = x.find("\r\n");
	while (i != x.find("\r\n\r\n")) {
		HttpHeader j = HttpHeader();
		j.toHeader(x.substr(0, i+2));
		header.push_back(j);
		x = x.substr(i + 2);
		i = x.find("\r\n");
	}
	//处理最后一个
	HttpHeader j = HttpHeader();
	j.toHeader(x.substr(0, i + 2));
	header.push_back(j);
	x = x.substr(i + 4);

	body = x;
	return true;
}

int HttpMSG::setLength() {
	int len = getBody().length();
	addHeader("Content-Length: "+to_string(len) + "\r\n");
	return len;
}

int HttpMSG::getLength() {
	return getBody().length();
}