#include "myhttp.h"

HttpHeader::HttpHeader(string k, string v, vector<string> s){
	key = k;
	value = v;
	ss = s;
}

HttpHeader::HttpHeader(char * k, char * v){
	key = string(k);
	value = string(v);
}

HttpHeader::HttpHeader(const char * k, const char * v){
	key = string(k);
	value = string(v);
}

HttpHeader::~HttpHeader(){
}

string HttpHeader::toString(){
	string r = key + ":" + value + "\r\n";
	return r;
}

void HttpHeader::reSet(string k,string v){
	key = k;
	value = v;
}

void HttpHeader::reSet(const char* k, const char* v){
	key = string(k);
	value = string(v);
}

string HttpHeader::getKey() {
	return key;
}

string HttpHeader::getValue() {
	return value;
}

bool HttpHeader::toHeader(string x) {
	//简单的合法性检验
	int i = x.find("\r\n");
	if (i != x.length() - 2) {
		//return false;  //是否为最后   要不要加\r\n???
	}
	else if(i >= 0){
		x = x.substr(0, x.length() - 2);
	}
	else {
		return false;
	}
	i = x.find(":");
	if (i < 0) {
		return false;  //是否有 ：
	}
	else {
		key = x.substr(0, i);
		value = x.substr(i + 1);
		return true;
	}
}

bool HttpHeader::toHeader(const char* x) {
	return toHeader(string(x));
}

HttpHeader::HttpHeader() {

}
bool HttpHeader::operator==(HttpHeader a) {
	if (a.getKey() == key && a.getValue() == value) {
		return true;
	}
	else {
		return false;
	}
}

bool HttpHeader::isNull() {
	if (key == "null"&&value == "null") {
		return true;
	}
	else {
		return false;
	}
}