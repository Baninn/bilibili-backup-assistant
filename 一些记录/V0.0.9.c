#include <stdio.h>
#include <curl\curl.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>

const char POSTURL[]="http://172.16.154.130:69/cgi-bin/srun_portal";  //定义POST地址
char *LOGOUT = "username=0&mac=&type=2&action=logout&ac_id=1";//注销发送参数 

/*下面处理返回数据来自STACKOVERFLOW的例子*/ 

struct string 
{//接受返回数据的结构体 
  char *ptr;
  size_t len;
};

void init_string(struct string *s) 
{//处理结构体的函数 
  s->len = 0;//先把结构体长度设置为0 
  s->ptr = malloc(s->len+1);//动态分配长度为1的内存空间给结构体里字符数组指针ptr 
  if (s->ptr == NULL) 
  {//如果分配失败 
    fprintf(stderr, "\n            内存错误！            \n");
    exit(EXIT_FAILURE);
  } 
  s->ptr[0] = '\0';//先把第0位设置为空 
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{//处理返回数据 
  size_t new_len = s->len + size*nmemb;//新长度等于=1+size*nmemb 
  s->ptr = realloc(s->ptr, new_len+1);//动态分配内存使长度达到2+size*nmemb 
  if (s->ptr == NULL) 
  {//如果分配失败 
    fprintf(stderr, "\n            内存错误!            \n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);//
  s->ptr[new_len] = '\0';//动态分配的存储字符数组 
  s->len = new_len;//设置结构体的里的长度等于新长度 

  return size*nmemb;
}

/*上面处理返回数据来自STACKOVERFLOW的例子*/ 

typedef struct login_info
{//登陆数据结构体 
	char username[20];
	char password[20];
	char acid; 
	char auto_login;	
}info;

void HTTP_POST(char * post,int * state) 
{//发送登陆或者注销函数 
	CURL *curl= curl_easy_init();
	if(!(curl))
  	{//如果非正常初始化 
	 	printf("\nLibcurl初始化失败，请重新打开本程序重试!\n");
	 	system("pause"); 
	 	exit(-1); 
	}
	CURLcode res;
	struct string s;
    init_string(&s);
	curl_easy_setopt(curl, CURLOPT_URL, POSTURL); //设置POST地址
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,post);//设置POST参数
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,1L);//超时设置成1s
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s); 
	while(1)
    {
		res = curl_easy_perform(curl);//启动libcurl 
		if (res != CURLE_OK)
		{
			printf("\n----------------------------------------\n");
			printf("\n              *** 错误: ***             \n");
			printf("\n  无法获取您的信息,请检查您的网络连接!  \n");
			printf("\n----------------------------------------\n");
			char input;
			while(1)
			{
				printf("\n     您是否要重试？<1.重试/2.退出>:     \n");
				char input=getch();
				if(input=='1')
				{ 
				   	break;
				}
			   	if(input=='2')
			   		exit (0);
			   	else
				{
					printf("\n----------------------------------------\n");
					printf("\n             输入错误请重试!            \n"); 
					printf("\n----------------------------------------\n");
				}
			}
		}
		if(res == CURLE_OK)
			break;
	}
	printf("\n----------------------------------------\n");
	printf("\n----------------------------------------\n");
	printf("\n           服务器反馈信息:              \n");
	printf("\n----------------------------------------\n");
	(char *)s.ptr;
	for(;*s.ptr!='\0';s.ptr++) 
	{
		if(*s.ptr=='Y'&&*(s.ptr+1)=='o')
		{//不在线情况 
			printf("\n         您不在线，登出失败         \n");
			printf("\n----------------------------------------\n");
			*state=0; 
			break;
		} 
		else if(*s.ptr=='t'&&*(s.ptr+2)=='o')
		{//注销成功情况
			printf("\n              注销成功              \n");
			printf("\n----------------------------------------\n");
			*state=0; 
			break;
		}
		else if(*s.ptr=='n'&&*(s.ptr+2)=='o')
		{//登陆成功情况
			printf("\n              登陆成功              \n");
			printf("\n----------------------------------------\n");
			*state=1; 
			break;
		} 
		else if(*s.ptr=='P'&&*(s.ptr+1)=='a') 
		{//密码错误情况
			printf("\n        密码错误，请检查输入。      \n");
			printf("\n----------------------------------------\n");
			*state=-1; 
			break;
		} 
		else if(*s.ptr=='U'&&*(s.ptr+1)=='s')
		{//用户名错误情况
			printf("\n      用户名不存在，请检查输入。    \n");
			printf("\n----------------------------------------\n");
			*state=-2; 
			break;
		} 
		else if(*s.ptr=='I'&&*(s.ptr+1)=='N') 
		{//ACID错误情况
			printf("\n              ACID错误              \n");
			printf("\n----------------------------------------\n");
			*state=-3; 
			break;
		}
		else if(*s.ptr=='m')
		{//参数错误情况 
			printf("\n              缺少参数              \n");
			printf("\n----------------------------------------\n");
			break;
		} 
	}
	printf("\n----------------------------------------\n");
	curl_easy_cleanup(curl);
}

void HTTP_GET_INFO(int * state)
{//获取当前登陆状态函数   
	CURL *curl= curl_easy_init();
	if(!(curl))
  	{//如果非正常初始化 
	 	printf("\nLibcurl初始化失败，请重新打开本程序重试!\n");
	 	system("pause"); 
	 	exit(-1); 
	}
	CURLcode res;
	printf("\n----------------------------------------\n");
	printf("\n-------正在获取您的信息，请稍后:--------\n"); 
	struct string s;
    init_string(&s);
	const char GET_INFO_URL[]="http://172.16.154.130/cgi-bin/rad_user_info";//定义获取登陆状态函数
	curl_easy_setopt(curl,CURLOPT_URL,GET_INFO_URL);//设置得到用户信息的GET地址
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,1L);//超时设置成1s
    while(1)
    {
		res = curl_easy_perform(curl);//启动libcurl 
		if (res != CURLE_OK)
		{
			printf("\n----------------------------------------\n");
			printf("\n              *** 错误: ***             \n");
			printf("\n  无法获取您的信息,请检查您的网络连接!  \n");
			printf("\n----------------------------------------\n");
			char input;
			while(1)
			{
				printf("\n     您是否要重试？<1.重试/2.退出>:     \n");
				char input=getch();
				if(input=='1')
				{ 
				   	break;
				}
			   	if(input=='2')
			   		exit (0);
			   	else
				{
					printf("\n----------------------------------------\n");
					printf("\n             输入错误请重试!            \n"); 
					printf("\n----------------------------------------\n");
				}
			}
		}
		if(res == CURLE_OK)
			break;
	}
	(char *)s.ptr;
	if(*s.ptr=='n')
	{//如果未登陆 
		printf("\n----------------------------------------\n");
		printf("\n                您不在线！              \n");
		printf("\n----------------------------------------\n");
		*state=0;
	}
	else
	{//如果已登录 
		printf("\n----------------------------------------\n");
		printf("\n       您已在线，下面是您的信息:        \n");
		printf("\n----------------------------------------\n");
		char *j=(char *)s.ptr;
		for(int flag=0;;s.ptr++)//开始循环直到数据末尾 
			{
				char temp[20]; //定义一个临时变量
				if(*s.ptr==',')//如果当前位为,分隔符 
				{	
					flag++;//,分隔符计数 
					char *t=temp;//定义一个临时指针指向t
					for(;*j!=*s.ptr;j++,t++)
						*t=*j; 
					*t='\0';
					if(flag==1)
						printf("\n您的登入用户名是: %s\n",temp);
					if(flag==7)
						printf("\n您这个月所用流量: %f GB\n",1.0*atof(temp)/1073741824);//atof(string)转浮点 
					if(flag==8)
						printf("\n您这个月所用时间: %03d小时%02d分钟%02d秒\n",atoi(temp)/3600,atoi(temp)/60%60,atoi(temp)%60); //atoi(string)转整数 
					if(flag==9)
					{
						printf("\n您 的 登 入  IP : %s\n",temp);
						break;
					}
					j++; 
					continue;
				}
			}
		printf("\n----------------------------------------\n");
		*state=1;
	} 
	curl_easy_cleanup(curl);//清理
}

void PRINT_WELCOME_INFO(void)
{//输出欢迎信息
	printf("\n----------------------------------------\n");
	printf("      欢迎使用校园网登陆器 C 语言版     \n");	
	printf("Made By CHN-STUDENT & MouYouLing V0.0.9\n");
	printf("\n----------------------------------------\n");
}


char * LOGIN_INFO_HANDLE(struct login_info *data)
{//用户名密码加密及URLENCODE函数 
	char *ptr=(char *)malloc(200);//分配200位的空间形成POST参数	
	char *back=ptr;//备份指针。因为将会用于后面循环复制信息 
	if(!ptr)
	{
		printf("\n----------------------------------------\n");
		printf("\n          为程序分配内存失败！          \n"); 
		printf("\n----------------------------------------\n");
		system("pause"); 
		exit(-1);
	}
	strcpy(ptr,"&action=login&drop=0&pop=1&type=2&n=117&mbytes=0&minutes=0&mac=&ac_id=");//POST必要参数 
	while(*ptr!='\0')ptr++;//到达POST参数末尾 
	*ptr=data->acid;//复制ACID信息 
	ptr++;//进入下一位 
	for(char *username_Post="&username=%7BSRUN3%7D%0D%0A";*username_Post!='\0';username_Post++,ptr++)
	{//复制名字POST参数头，这里头中的符号已经urlencode过 
		*ptr=*username_Post;
	}
	char temp[50]="\0";//这是用来临时存储加密后用户名和密码 
	for (int i = 0; i<strlen(data->username); ++i)
	{//用户名加密 
		temp[i] = data->username[i] + 4;
	}
	CURL *curl1 = curl_easy_init();//使用libcurl完成urlencode 
	CURL *curl2 = curl_easy_init();//当时测试一个完成用户名和密码urlencode会造成程序崩溃，所以此处用了两个分别完成 
	if(!(curl1&&curl2))
  	{//如果非正常初始化 
	 	printf("\nLibcurl初始化失败，请重新打开本程序重试!\n");
	 	system("pause"); 
	 	exit(-1); 
	}
	char *name_urlencode = curl_easy_escape(curl1,temp,0);//对名字urlencode 
	while(*name_urlencode!='\0')
	{//复制urlencode后的名字到POST参数字符数组 
		*ptr=*name_urlencode;
		ptr++;
		name_urlencode++;
	}
	memset(temp,0,sizeof(temp));//将temp字符数组重置 
	char key[] = "1234567890";//加密key
	int i;
	for (i = 0; i<strlen(data->password); ++i)
	{//这是密码加密函数 
		int ki = data->password[i] ^ key[strlen(key) - i%strlen(key) - 1];
		char _l[4] = { (char)((ki & 0x0f) + 0x36) };
		char _h[4] = { (char)((ki >> 4 & 0x0f) + 0x63) };
		if (i % 2 == 0)
		{
			strcat(_l, _h);
			strcat(temp, _l);
		}
		else
		{
			strcat(_h, _l);
			strcat(temp, _h);
		}
	}
	for(char *password_Post="&password=";*password_Post!='\0';password_Post++,ptr++)
	{
		*ptr=*password_Post;
	}
	char *password_urlencode = curl_easy_escape(curl2,temp,0);
	while(*password_urlencode !='\0')
	{
		*ptr=*password_urlencode ;
		ptr++;
		password_urlencode++;
	}
	*ptr='\0';//设置字符数组结束位 
	curl_easy_cleanup(curl1);//清理
	curl_easy_cleanup(curl2);//清理
	return back;//返回它的首地址 
}

int LOGIN(struct login_info *data,int * state,int * file)
{//登入函数 
	data=(info *)malloc(sizeof(info));//存取登陆信息用 
	if(!data)
	{
		printf("\n----------------------------------------\n");
		printf("\n          为程序分配内存失败！          \n"); 
		printf("\n----------------------------------------\n");
		system("pause"); 
		exit(-1);
	}
	FILE *read=fopen("config.json","rb");
	if(read&&(*state!=1))
	{
		*file=1;
		char temp[256];
		while(feof(read)==0)
		{
			fscanf(read,"%s",temp);
			if(strcmp(temp,"{")==0)
			{//以{为读取标识 
				break;
			}
		}
		fscanf(read,"%*10s%s",data->username);//扔掉前面的10位，正好扔掉#username=读 
		fscanf(read,"%*10s%s",data->password);
		fscanf(read,"%*6s%c",&data->acid);
		fscanf(read,"%*12s%c",&data->auto_login);
		fclose(read);
		if(data->auto_login=='1')
		{//第一次存在配置文件登陆显示 
			printf("\n----------------------------------------\n");
			printf("\n      检测到配置文件，自动登陆中!       \n"); 
			printf("\n----------------------------------------\n");
		}
		if(data->auto_login=='0')
		{
			printf("\n----------------------------------------\n");
			printf("\n              检测到配置文件，          \n");
			printf("\n        你是否现在登陆:<1.是/2.退出>    \n"); 
			printf("\n----------------------------------------\n");
			while(1)
			{
				char input=getch();
	   			if(input=='1')
	   			{ 
	   				break;
	   			}
				if(input=='2')
					exit (0);
				else
				{
					printf("\n----------------------------------------\n");
					printf("\n             输入错误请重试!            \n"); 
					printf("\n----------------------------------------\n");
				}
			}
		} 
	}
	else
	{
		printf("\n            读取不到配置文件            \n"); 
		printf("\n           请输入您的信息登陆           \n"); 
		printf("\n----------------------------------------\n");
		*file=0;
	}
	while(1)
	{
		if(*file==0||*file==-1)
		{
			if(*state==-2||*state==0)
			{//第一次输入或者用户名错误 
				printf("\n----------------------------------------\n");
				printf("\n            请输入你的用户名:           \n");
				gets(data->username); 
			} 
			if(*state==-1||*state==-2||*state==0) 
			{//第一次输入或者密码输入错误 
				printf("\n----------------------------------------\n");
				printf("\n         请输入你的密码（不显示）:      \n");
				char ch;
				unsigned int i;
				for (i = 0; (ch = getch()) != 13; )
				{//密码不回显输入 
					if (ch == 8)
					{//如果按下了退格键 
						if (i == 0)//先判断是否到了密码第一位 
							data->password[0] = '\0';
						else
						{//否则将它前一位变成空 
							data->password[i - 1] = '\0';
							i--;
						}
					}
					else
					{//否则完成本位输入并且进行下一位输入 
						data->password[i] = ch;
						i++;
					}
				}
				data->password[i] = '\0';
				printf("\n");
				printf("\n----------------------------------------\n");
			} 
			data->acid='1';//默认ACID=1 
			data->auto_login='0';//默认不自动登陆 
			if(*state==-3)
			{//ACID错误情况 
				data->acid='2';//如果错误自动修改 
			}  
		}
		HTTP_POST(LOGIN_INFO_HANDLE(data),state);
		if(*state==1)
		{
			break;
		}
		if(*state!=1)
		{
			*file=-1;
			while(*state!=1)
			{
				printf("\n----------------------------------------\n");
				printf("\n 登陆失败，你是否现在重试:<1.是/2.退出> \n"); 
				printf("\n----------------------------------------\n");
				while(1)
				{
					char input=getch();
		   			if(input=='1')
		   			{ 
		   				break;
		   			}
					if(input=='2')
						exit (0);
					else
					{
						printf("\n----------------------------------------\n");
						printf("\n             输入错误请重试!            \n"); 
						printf("\n----------------------------------------\n");
					}
				}
				break;
			}
		}
	}
	if(*state==1&&(*file==0||*file==-1)) 
	{
		printf("\n----------------------------------------\n");
		printf("\n      是否保存你的信息<1.是/2.否>       \n"); 
		printf("\n----------------------------------------\n");
		while(1)
		{
			char input=getch();
   			if(input=='1')
   			{ 
   				printf("\n----------------------------------------\n");
				printf("\n      是否自动登陆网络<1.是/2.否>       \n"); 
				printf("\n----------------------------------------\n");
				while(1)
				{
					char input=getch();
		   			if(input=='1')
		   			{ 
		   				data->auto_login='1';
		   				break;
		   			}
	   				if(input=='2')
	   				{
	   					data->auto_login='0';
	   					break; 
					} 
	   				else
					{
						printf("\n----------------------------------------\n");
						printf("\n             输入错误请重试!            \n"); 
						printf("\n----------------------------------------\n");
					}
				}
				FILE *out=fopen("config.json","wb");
				if(!out)
				{
					printf("\n----------------------------------------\n");
					printf("\n		  	    文件另存失败 	  		  \n");
					printf("\n----------------------------------------\n");
					return -1; 
				}
				fprintf(out,"/**************************************\n");
				fprintf(out,"这是校园网登陆器配置文件\n");
				fprintf(out,"为防止错误，请勿更改！\n");
				fprintf(out,"**************************************/\n");
				fprintf(out,"\n");
				fprintf(out,"{\n");
				fprintf(out,"#username=%s\n",data->username);
				fprintf(out,"#password=%s\n",data->password);
				fprintf(out,"#acid=%c\n",data->acid);
				fprintf(out,"#auto_login=%c\n",data->auto_login);
				fprintf(out,"}");
				fclose(out); 
   				break;
   			}
			if(input=='2')
				exit (0);
			else
			{
				printf("\n----------------------------------------\n");
				printf("\n             输入错误请重试!            \n"); 
				printf("\n----------------------------------------\n");
			}
		}
	} 
}

int main(void)
{
	system("title 校园网登陆器C语言版 V0.0.9");//设置程序名称 
	system("mode con cols=40");//设置宽度40
	system("color A");//字体设置为绿色 
	int state=0;//用户状态变量，1为登陆，0为未登录，-1密码错误，-2用户名错误，-3ACID错误
	int file=0;//文件状态变量，1为存在，2为不存在，-1代表文件内容错误会重新覆盖保存 
	PRINT_WELCOME_INFO();//输出欢迎信息 
	HTTP_GET_INFO(&state);//获取用户状态 
	struct login_info *data=NULL;//定义用户数据结构体指针 
	if(state==0)
	{//如果不在线 
		LOGIN(data,&state,&file);//登入 
	}
	else
   	{//已登录 
		printf("\n----------------------------------------\n");
		printf("\n      你是否现在注销:<1.是/2.退出>      \n"); 
		printf("\n----------------------------------------\n");
		while(1)
		{
			char input=getch();
   			if(input=='1')
   			{ 
   				HTTP_POST(LOGOUT,&state);//注销 
   				break;
   			}
			if(input=='2')
				exit (0);
			else
			{
				printf("\n----------------------------------------\n");
				printf("\n             输入错误请重试!            \n"); 
				printf("\n----------------------------------------\n");
			}
		}
   		if(state==0)
   		{//注销后重登陆 
   			printf("\n----------------------------------------\n");
			printf("\n      你是否现在重登陆:<1.是/2.退出>    \n"); 
			printf("\n----------------------------------------\n");
			while(1)
			{
				char input=getch();
	   			if(input=='1')
	   			{ 
					LOGIN(data,&state,&file);//登入 
	   				break;
	   			}
   				if(input=='2')
   					exit (0);
   				else
				{
					printf("\n----------------------------------------\n");
					printf("\n             输入错误请重试!            \n"); 
					printf("\n----------------------------------------\n");
				}
			}
		}
	} 
	free(data); 
	printf("\n----------------------------------------\n");
	printf("\n  感谢您的使用，欢迎给我提供改进建议！  \n"); 
	printf("\n----------------------------------------\n");
	system("pause"); 
	return 0;
}
