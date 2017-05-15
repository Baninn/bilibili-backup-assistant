#include "WinHttpClient.h"

#include <fstream>
#include <string>
#include <Windows.h>
#include <iostream>
#include <fstream>

#include <io.h>
#include <direct.h> 

using namespace std;


void WELCOME(void)//�����ӭ��Ϣ
{
	printf("\n----------------------------------------\n");
	printf("                 ��ӭʹ��                 \n");
	printf("\n----------------------------------------\n");
}


int file(char *fileName)//����һ�����ļ���
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
				mkdir(path);  //�����ɹ�����0 ���ɹ�����-1  
			}
		}
	}
	return 0;
}


string ws2s(const wstring &ws)//string��wstring�໥ת��
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
//���ַ�����ָ�����ַ�����ָ���ַ������棬targ_str �Ǳ��滻�ģ�val���滻���ַ���
{
	char temp_sstr[513], result[513];
	char *p;
	char *q;
	int len = 0; q = p = NULL;
	memset(temp_sstr, 0, sizeof(temp_sstr));//��temp_sstr����
	memset(result, 0, sizeof(result));//��result����
	strcpy(temp_sstr, source_str);//�ַ�������
	p = q = temp_sstr;
	len = strlen(targ_str);
	while (q != NULL)
	{
		if ((q = strstr(p, targ_str)) != NULL)
			//�ж��ַ���targ_str�Ƿ���p���Ӵ��������ǣ�Ϊnull
		{
			strncat(result, p, q - p);//��p��ָ�ַ�����ǰq-p���ַ���ӵ�result��β��
			strcat(result, val);//�����ַ���
			strcat(result, "\0");
			q += len;
			p = q;
		}
		else
			//����ǣ���ú�������targ_str��p���״γ��ֵĵ�ַ
			strcat(result, p);
	}
	strcpy(source_str, result);//�ַ�������
}


void copy()//��ָ��·�����Ƶ�ָ���ļ���
{
	SHFILEOPSTRUCT fop;
	ZeroMemory(&fop, sizeof fop);
	fop.wFunc = FO_COPY;
	//��Ϊ����·��
	fop.pFrom = _T("d:\\9137147");
	fop.pTo = _T("d:\\#xiangmu\\��4�¡���Ŀ������ ������ 05\0");
	SHFileOperation(&fop);
}


int _tmain(int argc, _TCHAR* argv[])
{
	WELCOME();//�����ӭ��Ϣ
	printf("���ڴ��ļ��С���\n");

	//����һ���ⲿ����
	ShellExecute(
		NULL,
		_T("open"),//��
		_T("Explorer.exe"),//��Դ������
		_T("/select, G:\\Android\\data\\tv.danmaku.bili\\download\\6691886"),
		//ѡ����Ƶ�����ļ�����·��
		NULL,
		SW_SHOWDEFAULT);
	printf("���ļ��гɹ�\n");
	//�򿪲�ѡ�и��ļ���

	printf("���ڽ�����ҳԴ���롭��\n");

	setlocale(LC_ALL, "");
	WinHttpClient client(L"http://www.jijidown.com/Video/Av1234567");//������վ��ַ
	bool b = client.SendHttpRequest();
	//���ӷ�������������
	if (!b)
	{
		printf("error\n");
		exit(1);
	}

	wstring httpResponseHeader = client.GetResponseHeader();
	wstring httpResponseContent = client.GetResponseContent();
	wcout << httpResponseHeader << endl;
	//ʹ��wcout���wchar

	if (httpResponseContent.empty() == false)
	{
		string strTmp = ws2s(httpResponseContent);
		std::ofstream  ofsLogFile("test.dat", std::ios_base::trunc);
		ofsLogFile << strTmp << endl;
	}

	printf("�����ɹ�\n");

	//���ļ���ȡ�ļ����ݵ�����in��
	fstream in; 
	char c;
	in.open("test.dat", ios::in);
	while (!in.eof())
	{
		in >> c; 
		//��test.dat�����ȡ��һ������������in
		cout << c;
	}
	in.close();

	printf("\n��������\n");

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
			//���Ҳ����<tittle>���ڵ���
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
						strcpy(tte, str);//tteΪ���е�һ���ַ�
						replace_string(tte, "        var Var_Bilibili_Title = '", "");
						//ȥ��ǰ����ַ�
						replace_string(tte, "';", "\\");
						//�Ѻ�����ַ��滻Ϊ"\\"

						printf("%s", tte);//����ļ�����
						file(tte);//���ú��������ļ���
						//copy();��ָ��·�����Ƶ�ָ���ļ���

						break;
					}
				}
			}
		}
		fclose(data);
	}
	cin.get();
}