//�����ļ�������ȫ�����ñ������Լ���ʼ������

#pragma once
#include <iostream>
#include <string>
#include <fstream>

using namespace std;


extern string PHP_PATH;
extern string PORT;

static bool readConfigFile(const char * cfgfilepath, const string & key, string & value);
void phpCGIProcess();
void init();





