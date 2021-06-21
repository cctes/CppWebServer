#include "Controller.h"

#define BUFF_SIZE 40960

//测试   cli模式
HttpResponse phpCLI(HttpRequest x) {
	string s = x.toString();
	string httpReq = base64_encode(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
	string tpath = getPathByURL(x.getURL());
	string initPath = getPathByURL("/init.php");
	string payload = PHP_PATH
		+ "php.exe -B include('"
		+ initPath
		+ "');include('"
		+ tpath
		+ "');exit(); "
		+ httpReq;
	
	FILE *fp;
	static char buff[BUFF_SIZE]; 
	char line[BUFF_SIZE];
	memset(buff, 0, BUFF_SIZE); 
	if ((fp = _popen(payload.c_str(), "r")) == NULL) {
		return HttpTemplate::http404();
	}
	else {
		HttpResponse h = HttpTemplate::http200();
		string b;

		while (fgets(line, BUFF_SIZE, fp) != NULL) {
			strcat_s(buff, line);
		};
		b = buff;
		h.setBody(b);
		h.addHeader(checkContentType(tpath));
		return h;
	}
	
}

//fastcgi
HttpResponse phpFastCGI(HttpRequest x) {
	int error = 0;
	string tpath = getPathByURL(x.getURL());
	FastCgi_t *c;
	c = (FastCgi_t *)malloc(sizeof(FastCgi_t));
	FastCgi_init(c);
	setRequestId(c, 1); 
	startConnect(c);
	sendStartRequestRecord(c);

	sendParams(c, "SCRIPT_FILENAME", tpath.c_str());
	if (x.getRequestMethod() == "GET") {
		sendParams(c, "REQUEST_METHOD", "GET");
		sendParams(c, "CONTENT_LENGTH", "0"); 
		sendParams(c, "CONTENT_TYPE", "text/html");
		sendParams(c, "QUERY_STRING", x.getQueryString().c_str());

		sendEndRequestRecord(c);
	}
	else if (x.getRequestMethod() == "POST") {
		string len = to_string(x.getLength());
		char Hbuf[8];

		sendParams(c, "REQUEST_METHOD", "POST");
		sendParams(c, "CONTENT_LENGTH", len.c_str()); 
		sendParams(c, "CONTENT_TYPE", "application/x-www-form-urlencoded");

		sendEndRequestRecord(c);

		/*FCGI_Header makeHeader(int type, int requestId,
						   int contentLength, int paddingLength)*/
						   //制造头为了发 body
		FCGI_Header t = makeHeader(FCGI_STDIN, c->requestId_, stoi(len), 0);
		memcpy(&t, Hbuf, FCGI_HEADER_LEN);
		send(c->sockfd_, Hbuf, sizeof(t), 0);

		/*发送正式的 body */
		send(c->sockfd_, x.getBody().c_str(), stoi(len), 0);

		//制造头告诉　body　结束　
		FCGI_Header endHeader;
		endHeader = makeHeader(FCGI_STDIN, c->requestId_, 0, 0);
		memset(&Hbuf, 0, sizeof(Hbuf));
		memcpy(&endHeader, Hbuf, FCGI_HEADER_LEN);
		send(c->sockfd_, Hbuf, sizeof(endHeader), 0);
	}
	else {
		FastCgi_finit(c);
		return HttpTemplate::http500();
	}
	
	string phpRes = readFromPhp(c);

	try{ 
		phpRes = phpRes.substr(phpRes.find("\r\n\r\n") + 4); 
	}
	catch(exception &e){
		phpRes = "";
	}
	
	HttpResponse r = HttpTemplate::http200();
	r.setBody(phpRes);
	r.addHeader(checkContentType(tpath));

	FastCgi_finit(c);
	return r;
}


BaseController::BaseController() {
	DP = DefaultProcessor();
}

BaseController::~BaseController() {
	for (vector<Processor*>::iterator it = SP.begin();
		it != SP.end(); ++it) {
		delete(*it);
	}
	SP.clear();
}

HttpResponse BaseController::OneConcention(HttpRequest x) {
	
	HttpResponse r = HttpResponse();

	string xxx = strrchr(getPathByURL(x.getURL()).c_str(), '.');

	if (xxx == ".php") {
		//r = phpCLI(x);    //cli处理
		r = phpFastCGI(x);  //cgi处理
	}
	else {
		r = DP.route(x);    //静态处理
	}
	
	//自定义处理
	for (vector<Processor*>::iterator it = SP.begin();
		it != SP.end(); ++it) {
		if ((*it)->route(x,r)) {
			r = (*it)->deal(x, r);
		}
	}
	//设定长度
	r.setLength();
	//连接状态   暂定不复用
	r.addHeader("Connection: close\r\n");
	return r;
}

bool BaseController::RegisterSProcessor(Processor *p) {
	SP.push_back(p);
	return true;
}


