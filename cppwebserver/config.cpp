#include"config.h"

string PHP_PATH;
string PORT;

static bool readConfigFile(const char * cfgfilepath, const string & key, string & value)
{
	fstream cfgFile;
	cfgFile.open(cfgfilepath);
	if (!cfgFile.is_open())
	{
		cout << "can not open cfg file!" << endl;
		return false;
	}
	char tmp[1000];
	while (!cfgFile.eof())
	{
		cfgFile.getline(tmp, 1000);
		string line(tmp);
		size_t pos = line.find('=');
		if (pos == string::npos) {
			continue;
		};
		string tmpKey = line.substr(0, pos);
		if (key == tmpKey)
		{
			value = line.substr(pos + 1);
			return true;
		}
	}
	return false;
}

void phpCGIProcess() {
	string cmd = "start /b " + PHP_PATH + "/php-cgi.exe -b 127.0.0.1:9000";
	system(cmd.c_str());
}


void init() {
	PHP_PATH = "C:/Hacktool/App/phpstudy_pro/Extensions/php/php7.3.4nts";
	PORT = "20000";
	//debug打不开文件?
	readConfigFile("./config","PHP_PATH",PHP_PATH);
	readConfigFile("./config","PORT",PORT);
	phpCGIProcess();
}