#include <stdio.h>
#include <process.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

//默认字体颜色
#define DEFAULT_COLOR FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED

unsigned int __stdcall send_message(void* p);		//发送消息函数
unsigned int __stdcall recv_message(void* P);		//接收消息函数
BOOL SetConsoleColor(WORD wAttributes);		//设置控制台字体

int main(int argc,char* argv[])
{
	SOCKET sockSrv;			//创建套接字
	SOCKET sockconn;			//连接套接字
	SOCKADDR_IN	 server;		//服务器地址结构信息
	WSADATA wsaData;		//库版本信息
	HANDLE hThread[2];
	int Length = sizeof(SOCKADDR);
	int test = 0;

	//加载库版本信息
	WSAStartup(MAKEWORD(1,1),&wsaData);
	//创建套接字
	sockSrv = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(6001);
	server.sin_family =	AF_INET;

	//连接服务器
	sockconn = connect(sockSrv,(SOCKADDR*)&server,Length);

	while (sockconn == SOCKET_ERROR)
	{
		SetConsoleColor(FOREGROUND_GREEN);
		printf("/*------------------与服务器连接失败!------------------*/\n");
		printf ("/*--------------------第%d次重试--------------------*/\n",(++test));
		SetConsoleColor(DEFAULT_COLOR);
		Sleep(1000);
		sockconn = connect(sockSrv,(SOCKADDR*)&server,Length);
	}

	SetConsoleColor(FOREGROUND_GREEN);
	printf ("Server IP is %s\n",inet_ntoa(server.sin_addr));
	SetConsoleColor(DEFAULT_COLOR);

	//创建接收消息线程
	hThread[0] = (HANDLE)_beginthreadex(NULL,0,recv_message,(void*)sockSrv,0,NULL);
	//创建发送消息线程
	hThread[1] = (HANDLE)_beginthreadex(NULL,0,send_message,(void*)sockSrv,0,NULL);

	WaitForMultipleObjects(2,hThread,TRUE,INFINITE);

	closesocket(sockSrv);
	WSACleanup();
	return 0;
}

unsigned int __stdcall send_message(void* p)
{
	char Sendbuf[50];
	SOCKET tmp = (SOCKET)p;

	while(1)
	{
		SetConsoleColor(FOREGROUND_GREEN);
		printf ("Please input message :");
		SetConsoleColor(DEFAULT_COLOR);

		gets(Sendbuf);
		fflush(stdin);
	
		if (send(tmp,Sendbuf,50,0) <= 0)
		{
			SetConsoleColor(FOREGROUND_GREEN);
			printf ("\nsend message error\n");
			SetConsoleColor(DEFAULT_COLOR);
		}
	}
	return 0;
}

unsigned int __stdcall recv_message(void* p)
{
	char Receivebuf[50];
	SOCKET tmp = (SOCKET)p;

	while (1)
	{
		recv(tmp,Receivebuf,50,0);
		if (Receivebuf <= 0)
		{
			SetConsoleColor(FOREGROUND_GREEN);
			printf ("/*--------与服务器断开连接！--------*/\n\n");
			SetConsoleColor(DEFAULT_COLOR);
			return 0;
		}
		else 
		{
			SetConsoleColor(FOREGROUND_GREEN);	
			printf ("\n服务器发来消息：%s\n\n",Receivebuf);
			SetConsoleColor(DEFAULT_COLOR);
		}
	}
	return 0;
}

//设置控制台字体
BOOL SetConsoleColor(WORD wAttributes)
{
	//获取标准输出句柄
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	if (hConsole == INVALID_HANDLE_VALUE)		//若为无效的句柄值
		return TRUE;

	return SetConsoleTextAttribute(hConsole,wAttributes);
}