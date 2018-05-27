#include <stdio.h>
#include <process.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

//Ĭ��������ɫ
#define DEFAULT_COLOR FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED

unsigned int __stdcall GetNewMessage(void* p);	//��ȡ�ͻ�����Ϣ����
unsigned int __stdcall send_message(void* p);		//������Ϣ����
struct Client_List* InsertNewClientSocket(int socket);	//��ӿͻ�����Ϣ����
BOOL SetConsoleColor(WORD wAttributes);		//���ÿ���̨����

//�ͻ�����Ϣ
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
	SOCKET sockSrv;			//�����׽��֣��󶨣����������Ӷ�����
	SOCKET sockConn;			//�����׽���
	SOCKADDR_IN server;		//��������ַ�ṹ��Ϣ�����Ǹ������׽������͵ı���
	SOCKADDR_IN client;		//�ͻ��˵�ַ�ṹ��Ϣ
	WSADATA wsaData;		//��汾��Ϣ�ṹ
	int Length = sizeof(SOCKADDR);	//�ṹ��ַ��С

	//�ͻ�����Ϣ��ʼ��
	ClientList = (struct Client_List*)malloc(sizeof(struct Client_List));
	t_socket = (struct Client_List*)malloc(sizeof(struct Client_List));
	ClientList->client_socket = NULL;
	ClientList->next = NULL;
	t_socket->client_socket = NULL;
	t_socket->next = NULL;
	g_hThreadEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	//��ʼ���׽���
	WSAStartup(MAKEWORD(1,1),&wsaData);
	//��ʼ���׽���
	sockSrv = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//��ʼ����������ַ�ṹ
	server.sin_port = htons(6001);
	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	bind(sockSrv,(SOCKADDR*)&server,sizeof(SOCKADDR));
	listen(sockSrv,5);

	SetConsoleColor(FOREGROUND_GREEN);
	printf ("/*===================���ڼ���6001�˿�===================*/\n\n\n");
	SetConsoleColor(DEFAULT_COLOR);

	_beginthreadex(NULL,0,send_message,NULL,0,NULL);

	while (TRUE)
	{
		sockConn = accept(sockSrv,(SOCKADDR*)&client,&Length);

		SetConsoleColor(FOREGROUND_GREEN);
		printf ("\n/*-------------------------------------------*/\n");
		printf ("/*	  New Client to connect              */\n");
		printf ("/*IP is %s ��Port is %d��ID is %d */\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),sockConn);
		printf ("/*--------------------------------------------*/\n\n");
		SetConsoleColor(DEFAULT_COLOR);

		//���¼���Ŀͻ���socket���浽������ȥ
		InsertNewClientSocket(sockConn);

		_beginthreadex(NULL,0,GetNewMessage,(void*)sockConn,0,NULL);
		
	}

	closesocket(sockSrv);
	WSACleanup();				//�رն�̬���ӿ�
	return 0;
}


//������Ϣ����
unsigned int __stdcall GetNewMessage(void* p)
{
	char Receivebuf[50] = {0};
	SOCKET sockconn = (SOCKET)p;

	while(1)
	{
		if (recv(sockconn,Receivebuf,50,0) <= 0)
		{
			SetConsoleColor(FOREGROUND_GREEN);	
			printf ("/*--------��IDΪ%d�Ŀͻ��˶Ͽ����ӣ�--------*/\n\n",sockconn);
			SetConsoleColor(DEFAULT_COLOR);
			return 0;
		}
		else 
		{
			SetConsoleColor(FOREGROUND_GREEN);	
			printf ("IDΪ%d�Ŀͻ��˷�����Ϣ��%s\n\n",sockconn,Receivebuf);
			SetConsoleColor(DEFAULT_COLOR);
		}
	}
	return 0;
}

//������Ϣ����
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
		printf ("\n�����뷢�Ϳͻ��˵�ID��\n");
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
				printf ("û���ҵ�IDΪ%d�Ŀͻ���\n",ClientID);
				printf ("���������룺");
				SetConsoleColor(DEFAULT_COLOR);
				scanf("%d",&ClientID);
				tmp = ClientList;		//���������б��û���ҵ������ID�����������벢���±���
			}
		}

		SetConsoleColor(FOREGROUND_GREEN);
		printf ("\n�����뷢����Ϣ��\n");
		SetConsoleColor(DEFAULT_COLOR);

		fflush(stdin);
		gets(SendBuf);
		fflush(stdin);

		if (send(ClientID,SendBuf,50,0) == SOCKET_ERROR)
			printf ("��Ϣ����ʧ�ܣ�����\n");
	}

	return 0;
}

//ȫ��βָ�룬ָ����������һλ
struct Client_List* InsertNewClientSocket(int socket)
{
	struct Client_List* NewNode;
	
	NewNode = (struct Client_List*)malloc(sizeof(struct Client_List));
	NewNode->client_socket = socket;
	NewNode->next = NULL;

	//���ǵ�һ���ͻ���
	if (ClientList->next == NULL)
	{
		ClientList = NewNode;	//����һ������ͷ�ڵ�
		t_socket = NewNode;	//β�ڵ���ָ��ͷ�ڵ�
		ClientList->next = t_socket;
	}
	else
	{
		t_socket->next = NewNode;	//β�ڵ����һ��ָ���µĽڵ�
		t_socket = NewNode;			//�½ڵ㸳��β�ڵ�
	}
	
	return NewNode;
}

//���ÿ���̨�����ɫ
BOOL SetConsoleColor(WORD wAttributes)
{
	//��ȡ��׼����ľ�� 
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	 
	if (hConsole == INVALID_HANDLE_VALUE)	//��Ϊ��Ч�ľ��ֵ 
		return TRUE;
		
	return SetConsoleTextAttribute(hConsole,wAttributes);
}