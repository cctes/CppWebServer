#include "myhttp.h"

HttpResponse::HttpResponse() {

}

HttpResponse::HttpResponse(string f, vector<HttpHeader> h, string b) {
	firstLine = f;
	header = h;
	body = b;
	setAttribute();
}

HttpResponse::~HttpResponse() {

}

string HttpResponse::getHttpVersion() {
	return httpVersion;
}

string HttpResponse::getStatus() {
	return status;
}

string HttpResponse::getStatusCode() {
	return statusCode;
}

bool HttpResponse::setHttpVersion(string v) {
	httpVersion = v;
	firstLine = httpVersion 
		+ " " + statusCode 
		+ " " + status 
		+ "\r\n";
	return true;
}

bool HttpResponse::setStatusCode(string c) {
	statusCode = c;
	firstLine = httpVersion
		+ " " + statusCode
		+ " " + status
		+ "\r\n";
	return true;
}

bool HttpResponse::setStatus(string s) {
	status = s;
	firstLine = httpVersion
		+ " " + statusCode
		+ " " + status
		+ "\r\n";
	return true;
}

bool HttpResponse::toMSG(string x) {
	HttpMSG::toMSG(x);
	return setAttribute();
}

bool HttpResponse::setAttribute() {
	string x = firstLine;
	if (x.find(" ") < 0) {
		return false;
	}
	httpVersion = x.substr(0, x.find(" "));
	x = x.substr(x.find(" ") + 1);
	if (x.find(" ") < 0) {
		return false;
	}
	statusCode = x.substr(0, x.find(" "));
	x = x.substr(x.find(" ") + 1);
	if (x.length() <= 2) {
		return false;
	}
	status = x.substr(0, x.find("\r\n"));
	return true;
}

void HttpResponse::setFirstLine(const char * f) {
	HttpMSG::setFirstLine(f);
	setAttribute();
}

void HttpResponse::setFirstLine(string f) {
	HttpMSG::setFirstLine(f);
	setAttribute();
}