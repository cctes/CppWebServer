#include "myhttp.h"
HttpRequest::HttpRequest(string f, vector<HttpHeader> h, string b){
	firstLine = f;
	header = h;
	body = b;
	setAttribute();
}



HttpRequest::HttpRequest() {

}

HttpRequest::~HttpRequest() {

}

bool HttpRequest::setRequestMethod(string m) {
	requestMethod = m;
	firstLine = requestMethod
		+ " " + URL
		+ " " + httpVersion
		+ "\r\n";
	return true;
}

bool HttpRequest::setURL(string u) {
	URL = u;
	firstLine = requestMethod
		+ " " + URL
		+ " " + httpVersion
		+ "\r\n";
	return true;
}

bool HttpRequest::setHttpVersion(string v) {
	httpVersion = v;
	firstLine = requestMethod
		+ " " + URL
		+ " " + httpVersion
		+ "\r\n";
	return true;
}

string HttpRequest::getRequestMethod() {
	return requestMethod;
}

string HttpRequest::getURL() {
	return URL;
}

string HttpRequest::getHttpVersion() {
	return httpVersion;
}

bool HttpRequest::toMSG(string x) {
	HttpMSG::toMSG(x);
	return setAttribute();

}

bool HttpRequest::setAttribute() {
	string x = firstLine;
	if (x.find(" ") < 0) {
		return false;
	}
	requestMethod = x.substr(0, x.find(" "));
	x = x.substr(x.find(" ")+1);
	if (x.find(" ") < 0) {
		return false;
	}
	URL = x.substr(0, x.find(" "));
	x = x.substr(x.find(" ") + 1);
	if (x.length() <= 2) {
		return false;
	}
	httpVersion = x.substr(0, x.find("\r\n"));
	return true;
}

void HttpRequest::setFirstLine(const char * f) {
	HttpMSG::setFirstLine(f);
	setAttribute();
}

void HttpRequest::setFirstLine(string f) {
	HttpMSG::setFirstLine(f);
	setAttribute();
}


string HttpRequest::getQueryString() {
	string s = URL;
	int i = -1;
	int j = -1;
	i = s.find("?");
	if (i >= 0) {
		s = s.substr(i+1);
	}
	else {
		s = "";
	}
	return s;
}
