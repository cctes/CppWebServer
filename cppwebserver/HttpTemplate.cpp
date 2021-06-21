#include "myhttp.h"

string getDate() {
	time_t now = time(nullptr);
	struct tm* info = new tm;
	gmtime_s(info, &now);
	const char* fmt = "%a, %d %b %Y %H:%M:%S GMT";
	char tstr[30];

	strftime(tstr, sizeof(tstr), fmt, info);
	return tstr;
}

HttpResponse HttpTemplate::http200() {
	HttpResponse x = HttpResponse();
	x.setFirstLine("HTTP/1.1 200 OK\r\n");
	x.addHeader("Date: " + getDate() + "\r\n");
	x.addHeader("Server: cppwebserver\r\n");
	//x.addHeader("Connection: close\r\n");

	return x;
}

HttpResponse HttpTemplate::http404() {
	HttpResponse x = HttpResponse();
	x.setFirstLine("HTTP/1.1 404 Not Found\r\n");
	x.addHeader("Date: " + getDate() + "\r\n");
	x.addHeader("Server: cppwebserver\r\n");
	//x.addHeader("Connection: close\r\n");

	return x;
}

HttpResponse HttpTemplate::http500() {
	//500 Internal Server Error
	HttpResponse x = HttpResponse();
	x.setFirstLine("HTTP/1.1 500 Internal Server Error\r\n");
	x.addHeader("Date: " + getDate() + "\r\n");
	x.addHeader("Server: cppwebserver\r\n");
	//x.addHeader("Connection: close\r\n");

	return x;
}

string checkContentType(string x) {
	string t;
	try{
		t = strrchr(x.c_str(), '.');
	}
	catch (exception &e) {
		return "Content-Type: text/plain; charset=UTF-8";
	}
	
	if (t == ".html"|| t == ".php") {
		return "Content-Type: text/html; charset=UTF-8";
	}
	else if (t == ".js") {
		return "Content-Type: application/x-javascript";
	}
	else if (t == ".css") {
		return "Content-Type: text/css";
	}
	else if (t == ".jpg") {
		return "Content-Type: image/jpeg";
	}
	else if (t == ".png") {
		return "Content-Type: image/png";
	}
	else if (t == ".gif") {
		return "Content-Type: image/gif";
	}
	else {
		return "Content-Type: text/plain; charset=UTF-8";
	}
	
}

string getPathByURL(string u) {
	string path;
	char* str;
	int i, j;
	_get_pgmptr(&str);
	path += str;
	//°Ü±Ê£¬¼ÇµÃ¸Ä
	path = path.substr(0, path.find_last_of("\\"));
	i = path.find("\\");
	while (i >= 0) {
		path = path.replace(i, 1, "/");
		i = path.find("\\");
	}
	i = u.find("/?");
	j = u.find("?");
	if (i >= 0) {
		u = u.substr(0, i);
	}
	else if (j >= 0) {
		u = u.substr(0, j);
	}
	else {
	}
	path += u;
	return path;
}