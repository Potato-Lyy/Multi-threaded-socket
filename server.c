#include <stdio.h>
#include <process.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

//默认字体颜色
#define DEFAULT_COLOR FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED

unsigned int __stdcall GetNewMessage(void* p);	//获取客户端消息函数
unsigned int __stdcall send_message(void* p);		//发送消息函数
struct Client_List* InsertNewClientSocket(int socket);	//添加客户端信息函数
BOOL SetConsoleColor(WORD wAttributes);		//设置控制台字体

//客户端信息
struct Client_List
{
	SOCKET client_socket;
	struct Client_List* next;
};

HANDLE g_hThreadEvent;
struct Client_List* ClientList = {0};
struct Client_List* t_socket = {0};

int main(int argc,char* argv[])
{
	SOCKET sockSrv;			//创建套接字，绑定，监听，连接都靠它
	SOCKET sockConn;			//连接套接字
	SOCKADDR_IN server;		//服务器地址结构信息，它是个定义套接字类型的变量
	SOCKADDR_IN client;		//客户端地址结构信息
	WSADATA wsaData;		//库版本信息结构
	int Length = sizeof(SOCKADDR);	//结构地址大小

	//客户表信息初始化
	ClientList = (struct Client_List*)malloc(sizeof(struct Client_List));
	t_socket = (struct Client_List*)malloc(sizeof(struct Client_List));
	ClientList->client_socket = NULL;
	ClientList->next = NULL;
	t_socket->client_socket = NULL;
	t_socket->next = NULL;
	g_hThreadEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	//初始化套接字
	WSAStartup(MAKEWORD(1,1),&wsaData);
	//初始化套接字
	sockSrv = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//初始化服务器地址结构
	server.sin_port = htons(6001);
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	bind(sockSrv,(SOCKADDR*)&server,sizeof(SOCKADDR));
	listen(sockSrv,5);

	SetConsoleColor(FOREGROUND_GREEN);
	printf ("/*===================正在监听6001端口===================*/\n\n\n");
	SetConsoleColor(DEFAULT_COLOR);

	_beginthreadex(NULL,0,send_message,NULL,0,NULL);

	while (TRUE)
	{
		sockConn = accept(sockSrv,(SOCKADDR*)&client,&Length);

		SetConsoleColor(FOREGROUND_GREEN);
		printf ("\n/*-------------------------------------------*/\n");
		printf ("/*	  New Client to connect              */\n");
		printf ("/*IP is %s ，Port is %d，ID is %d */\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),sockConn);
		printf ("/*--------------------------------------------*/\n\n");
		SetConsoleColor(DEFAULT_COLOR);

		//将新加入的客户端socket保存到链表中去
		InsertNewClientSocket(sockConn);

		_beginthreadex(NULL,0,GetNewMessage,(void*)sockConn,0,NULL);
		
	}

	closesocket(sockSrv);
	WSACleanup();				//关闭动态链接库
	return 0;
}


//接受消息函数
unsigned int __stdcall GetNewMessage(void* p)
{
	char Receivebuf[50] = {0};
	SOCKET sockconn = (SOCKET)p;

	while(1)
	{
		if (recv(sockconn,Receivebuf,50,0) <= 0)
		{
			SetConsoleColor(FOREGROUND_GREEN);	
			printf ("/*--------与ID为%d的客户端断开连接！--------*/\n\n",sockconn);
			SetConsoleColor(DEFAULT_COLOR);
			return 0;
		}
		else 
		{
			SetConsoleColor(FOREGROUND_GREEN);	
			printf ("ID为%d的客户端发来消息：%s\n\n",sockconn,Receivebuf);
			SetConsoleColor(DEFAULT_COLOR);
		}
	}
	return 0;
}

//发送消息函数
unsigned int __stdcall send_message(void* p)
{
	char SendBuf[50] = {0};
	int ClientID = 0;
	struct Client_List* tmp;
	tmp = (struct Client_List*)malloc(sizeof(struct Client_List));
	//tmp = ClientList;

	while(1)
	{
		SetConsoleColor(FOREGROUND_GREEN);	
		printf ("\n请输入发送客户端的ID：\n");
		SetConsoleColor(DEFAULT_COLOR);

		scanf("%d",&ClientID);

		tmp = ClientList;

		while (tmp->client_socket != ClientID)
		{
			//printf ("t_socket = %d		next = %d\n",tmp->client_socket,tmp->next);
			if(tmp->next != NULL)
				tmp = tmp->next;
			else 
			{
				SetConsoleColor(FOREGROUND_GREEN);	
				printf ("没有找到ID为%d的客户端\n",ClientID);
				printf ("请重新输入：");
				SetConsoleColor(DEFAULT_COLOR);
				scanf("%d",&ClientID);
				tmp = ClientList;		//若遍历完列表后都没有找到输入的ID，就重新输入并重新遍历
			}
		}

		SetConsoleColor(FOREGROUND_GREEN);
		printf ("\n请输入发送消息：\n");
		SetConsoleColor(DEFAULT_COLOR);

		fflush(stdin);
		gets(SendBuf);
		fflush(stdin);

		if (send(ClientID,SendBuf,50,0) == SOCKET_ERROR)
			printf ("消息发送失败！！！\n");
	}

	return 0;
}

//全局尾指针，指向链表的最后一位
struct Client_List* InsertNewClientSocket(int socket)
{
	struct Client_List* NewNode;
	
	NewNode = (struct Client_List*)malloc(sizeof(struct Client_List));
	NewNode->client_socket = socket;
	NewNode->next = NULL;

	//若是第一个客户端
	if (ClientList->next == NULL)
	{
		ClientList = NewNode;	//将第一个赋给头节点
		t_socket = NewNode;	//尾节点先指向头节点
		ClientList->next = t_socket;
	}
	else
	{
		t_socket->next = NewNode;	//尾节点的下一个指向新的节点
		t_socket = NewNode;			//新节点赋给尾节点
	}
	
	return NewNode;
}

//设置控制台输出颜色
BOOL SetConsoleColor(WORD wAttributes)
{
	//获取标准输出的句柄 
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	 
	if (hConsole == INVALID_HANDLE_VALUE)	//若为无效的句柄值 
		return TRUE;
		
	return SetConsoleTextAttribute(hConsole,wAttributes);
}