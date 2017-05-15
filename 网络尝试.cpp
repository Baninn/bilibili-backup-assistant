#include "WinHttpClient.h"

#include <fstream>
#include <string>
#include <Windows.h>
#include <iostream>
#include <fstream>

#include <io.h>
#include <direct.h> 

using namespace std;


void WELCOME(void)//输出欢迎信息
{
	printf("\n----------------------------------------\n");
	printf("                 欢迎使用                 \n");
	printf("\n----------------------------------------\n");
}


int file(char *fileName)//创建一个新文件夹
{
	char *tag;
	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			char buf[1000];
			char path[1000];
			strcpy(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy(path, buf);
			if (access(path, 6) == -1)
			{
				mkdir(path);  //创建成功返回0 不成功返回-1  
			}
		}
	}
	return 0;
}


string ws2s(const wstring &ws)//string和wstring相互转换
{
	const wchar_t* wpchar = ws.c_str();
	size_t returnVal = 0;
	size_t wSize = 2 * ws.size() + 1;
	char* pchar = new char[wSize];
	memset(pchar, 0, wSize);
	wcstombs(pchar, wpchar, wSize);
	string result = pchar;
	delete[] pchar;
	return result;
}


void replace_string(char * source_str, char * targ_str, char *val)
//将字符串中指定子字符串用指定字符串代替，targ_str 是被替换的，val是替换的字符串
{
	char temp_sstr[513], result[513];
	char *p;
	char *q;
	int len = 0; q = p = NULL;
	memset(temp_sstr, 0, sizeof(temp_sstr));//把temp_sstr清零
	memset(result, 0, sizeof(result));//把result清零
	strcpy(temp_sstr, source_str);//字符串复制
	p = q = temp_sstr;
	len = strlen(targ_str);
	while (q != NULL)
	{
		if ((q = strstr(p, targ_str)) != NULL)
			//判断字符串targ_str是否是p的子串，若不是，为null
		{
			strncat(result, p, q - p);//把p所指字符串的前q-p个字符添加到result结尾处
			strcat(result, val);//连接字符串
			strcat(result, "\0");
			q += len;
			p = q;
		}
		else
			//如果是，则该函数返回targ_str在p中首次出现的地址
			strcat(result, p);
	}
	strcpy(source_str, result);//字符串复制
}


void copy()//从指定路径复制到指定文件夹
{
	SHFILEOPSTRUCT fop;
	ZeroMemory(&fop, sizeof fop);
	fop.wFunc = FO_COPY;
	//下为测试路径
	fop.pFrom = _T("d:\\9137147");
	fop.pTo = _T("d:\\#xiangmu\\【4月】夏目友人帐 第六季 05\0");
	SHFileOperation(&fop);
}


int _tmain(int argc, _TCHAR* argv[])
{
	WELCOME();//输出欢迎信息
	printf("正在打开文件夹……\n");

	//运行一个外部程序
	ShellExecute(
		NULL,
		_T("open"),//打开
		_T("Explorer.exe"),//资源管理器
		_T("/select, G:\\Android\\data\\tv.danmaku.bili\\download\\6691886"),
		//选择，视频缓存文件所在路径
		NULL,
		SW_SHOWDEFAULT);
	printf("打开文件夹成功\n");
	//打开并选中该文件夹

	printf("正在解析网页源代码……\n");

	setlocale(LC_ALL, "");
	WinHttpClient client(L"http://www.jijidown.com/Video/Av1234567");//解析网站地址
	bool b = client.SendHttpRequest();
	//检测从服务器返回数据
	if (!b)
	{
		printf("error\n");
		exit(1);
	}

	wstring httpResponseHeader = client.GetResponseHeader();
	wstring httpResponseContent = client.GetResponseContent();
	wcout << httpResponseHeader << endl;
	//使用wcout输出wchar

	if (httpResponseContent.empty() == false)
	{
		string strTmp = ws2s(httpResponseContent);
		std::ofstream  ofsLogFile("test.dat", std::ios_base::trunc);
		ofsLogFile << strTmp << endl;
	}

	printf("解析成功\n");

	//打开文件读取文件内容到对象in中
	fstream in; 
	char c;
	in.open("test.dat", ios::in);
	while (!in.eof())
	{
		in >> c; 
		//从test.dat里面读取出一个整数，赋给in
		cout << c;
	}
	in.close();

	printf("\n标题如下\n");

	FILE*data;
	char str[200];
	char tte[200];
	data = fopen("test.dat", "rb");
	if (data == NULL)
	{
		printf("Error opening file\n");
	}
	else
	{
		while (fgets(str, 200, data))
		{
			//查找并输出<tittle>所在的行
			char substr[100] = "var Var_Bilibili_Title = ";
			char *p = str;
			char *q = substr;
			int flag = 0;
			for (; *(p + strlen(substr) - 1); p++)
			{
				for (q = substr; *p == *q&&*q; p++, q++);
				{
					if (!*q)
					{
						strcpy(tte, str);//tte为该行的一行字符
						replace_string(tte, "        var Var_Bilibili_Title = '", "");
						//去掉前面的字符
						replace_string(tte, "';", "\\");
						//把后面的字符替换为"\\"

						printf("%s", tte);//输出文件夹名
						file(tte);//调用函数创建文件夹
						//copy();从指定路径复制到指定文件夹

						break;
					}
				}
			}
		}
		fclose(data);
	}
	cin.get();
}