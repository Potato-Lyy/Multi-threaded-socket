#include <stdio.h>
#include <process.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

//Ĭ��������ɫ
#define DEFAULT_COLOR FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED

unsigned int __stdcall send_message(void* p);		//������Ϣ����
unsigned int __stdcall recv_message(void* P);		//������Ϣ����
BOOL SetConsoleColor(WORD wAttributes);		//���ÿ���̨����

int main(int argc,char* argv[])
{
	SOCKET sockSrv;			//�����׽���
	SOCKET sockconn;			//�����׽���
	SOCKADDR_IN	 server;		//��������ַ�ṹ��Ϣ
	WSADATA wsaData;		//��汾��Ϣ
	HANDLE hThread[2];
	int Length = sizeof(SOCKADDR);
	int test = 0;

	//���ؿ�汾��Ϣ
	WSAStartup(MAKEWORD(1,1),&wsaData);
	//�����׽���
	sockSrv = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(6001);
	server.sin_family =	AF_INET;

	//���ӷ�����
	sockconn = connect(sockSrv,(SOCKADDR*)&server,Length);

	while (sockconn == SOCKET_ERROR)
	{
		SetConsoleColor(FOREGROUND_GREEN);
		printf("/*------------------�����������ʧ��!------------------*/\n");
		printf ("/*--------------------��%d������--------------------*/\n",(++test));
		SetConsoleColor(DEFAULT_COLOR);
		Sleep(1000);
		sockconn = connect(sockSrv,(SOCKADDR*)&server,Length);
	}

	SetConsoleColor(FOREGROUND_GREEN);
	printf ("Server IP is %s\n",inet_ntoa(server.sin_addr));
	SetConsoleColor(DEFAULT_COLOR);

	//����������Ϣ�߳�
	hThread[0] = (HANDLE)_beginthreadex(NULL,0,recv_message,(void*)sockSrv,0,NULL);
	//����������Ϣ�߳�
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
			printf ("/*--------��������Ͽ����ӣ�--------*/\n\n");
			SetConsoleColor(DEFAULT_COLOR);
			return 0;
		}
		else 
		{
			SetConsoleColor(FOREGROUND_GREEN);	
			printf ("\n������������Ϣ��%s\n\n",Receivebuf);
			SetConsoleColor(DEFAULT_COLOR);
		}
	}
	return 0;
}

//���ÿ���̨����
BOOL SetConsoleColor(WORD wAttributes)
{
	//��ȡ��׼������
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	if (hConsole == INVALID_HANDLE_VALUE)		//��Ϊ��Ч�ľ��ֵ
		return TRUE;

	return SetConsoleTextAttribute(hConsole,wAttributes);
}