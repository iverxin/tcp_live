// CamServer.cpp : �������̨Ӧ�ó������ڵ㡣
//
//

#include <stdio.h>
#include <tchar.h>
#include <opencv2\opencv.hpp>
#include <WinSock2.h>
#include <vector>
#include <string>
#include <iostream>
#include <windows.h>
#include <WS2tcpip.h>
#include <MVImage.h>
#include <ctime>
using namespace cv;

Mat cv_img; //cv_ͼ��
VideoCapture capture(0); //����������
boolean thread_stop = false; //����߳�
HANDLE hMutex = NULL; //ͬ����
std::vector<uchar> data_encode; //���������
char * send_buffer[]; //���ͻ�����

// �����������
DWORD WINAPI local_cam_thread(LPVOID lpParamter)
{
	while(!thread_stop)
	{
		//WaitForSingleObject(hMutex, INFINITE);
		capture.read(cv_img);
		
		imshow("ʵʱͼ��", cv_img);
		waitKey(2);
		//ReleaseMutex(hMutex);
		Sleep(10);
	}
	return 0L;
}

int main()
{	
	//�����ʼ��
	std::cout << "�����������ʧ�ܣ�ʹ�ñ������" <<std::endl;
	// �½�����߳�
	HANDLE hThread = CreateThread(NULL, 0, local_cam_thread, NULL, 0, NULL);
	// ͬ����
	//hMutex = CreateMutex(NULL, FALSE, NULL);
	CloseHandle(hThread);
	char * hostName;
	unsigned short port;
	int maxClientNum = 3;
	
	// ��ʼ�����ӿ�
	WSADATA wsaData;
	//�������ӿ��
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// ���ü�����
	hostName = "127.0.0.1";
	// ������������
	int maxService = 3;
	// �˿�
	port = 9999;

	// ����һ��socket
	// ��������ַЭ���壬 socket���ͣ� ����Э��,���Ϊ0��ϵͳ�Զ�����ǰ��������������ʲô���͡�
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	// ��SOCKADDһ������16�ֽڣ����ǰ�ip��port�ֿ��ˡ����㸳ֵ������ʹ��ǿ������ת������������
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);
	//addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //0.0.0.0
	inet_pton(AF_INET, hostName, &addrServer.sin_addr.S_un.S_addr);
	// addrServer.sin_addr.S_addr = htonl(hostName);
	// htons() ���˿ں��������ֽ�˳��ת��Ϊ�����ֽ�˳��
	// inet_addr() ��ip�ַ���ת���������ֽ��������ֵ��
	// inet_ntoa() ��sin_addr�ṹ�������ip�ַ����� network to assic

	
	int retVal = bind(sock, (SOCKADDR *)&addrServer, sizeof(SOCKADDR_IN));
	if (retVal == SOCKET_ERROR) {
		printf("��ʼ��ʧ��");
		return 0;
	}
	printf("�����������\n");
	
	// ��ʼ����
	if (listen(sock, maxClientNum) == SOCKET_ERROR) {
		printf("����ʧ��");
		return 0;
	}
	std::cout << "���ڼ�����" << hostName << ":" << port << std::endl;
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	
	while(1)
	{
		SOCKET sockConn = accept(sock, (SOCKADDR *)&addrClient, &len);
		while (1)
		{
			//׼������
			clock_t start_time = clock();
			//׼������
			imencode(".jpg", cv_img, data_encode);
			clock_t end_time = clock();
			std::cout << "����ʱ��" << end_time-start_time;
			
			char str[INET_ADDRSTRLEN];
			printf("�����ַ %s", inet_ntop(AF_INET, &addrClient.sin_addr, str, sizeof(str)));
			printf("ta����");
			//���Ի������Ƿ�������
			if (data_encode.empty()) {
				char sendbuf[] = "����ͷδ����";
				send(sockConn, sendbuf, sizeof(sendbuf), 0);
			}
			
			//��ȡ���ݳ���
			int len_encode = data_encode.size();
			//����ת�����ַ������ͳ�ȥ��
			std::string len_msg = std::to_string(len_encode);
			//���쳤����ϢΪ�̶���16�ֽ�
			int ori_len = len_msg.length();
			for (int i = 0; i< 16 - ori_len; i++)
			{
				//ƴ��
				len_msg = len_msg + " ";
			}
			std::cout << "len_msg:" << len_msg.length() << std::endl;
			send(sockConn, len_msg.c_str(), 16, 0);

			//���ͻ���������

			char *send_b = new char[data_encode.size()];
			start_time = clock();

			for (int i = 0; i<data_encode.size(); i++)
			{
				//data_encode.size()����װ�����ַ�����
				send_b[i] = data_encode[i];
			}
			int iSend = send(sockConn, send_b, data_encode.size(), 0);
			delete[]send_b;
			if (iSend == SOCKET_ERROR) {
				printf("����ʧ��");
				std::cout << "�ͻ��˶Ͽ����ȴ���������..." << std::endl;
				break;
			}
			end_time = clock();
			std::cout << "����ʱ��" << end_time - start_time;
		}
	}
	closesocket(sock);
	WSACleanup();
	thread_stop = true;
	return 0;
}

