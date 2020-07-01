// CamServer.cpp : 定义控制台应用程序的入口点。
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

Mat cv_img; //cv_图像
VideoCapture capture(0); //本地相机句柄
boolean thread_stop = false; //相机线程
HANDLE hMutex = NULL; //同步锁
std::vector<uchar> data_encode; //编码的数据
char * send_buffer[]; //发送缓冲区

// 本地相机进程
DWORD WINAPI local_cam_thread(LPVOID lpParamter)
{
	while(!thread_stop)
	{
		//WaitForSingleObject(hMutex, INFINITE);
		capture.read(cv_img);
		
		imshow("实时图像", cv_img);
		waitKey(2);
		//ReleaseMutex(hMutex);
		Sleep(10);
	}
	return 0L;
}

int main()
{	
	//相机初始化
	std::cout << "网络相机链接失败，使用本机相机" <<std::endl;
	// 新建相机线程
	HANDLE hThread = CreateThread(NULL, 0, local_cam_thread, NULL, 0, NULL);
	// 同步锁
	//hMutex = CreateMutex(NULL, FALSE, NULL);
	CloseHandle(hThread);
	char * hostName;
	unsigned short port;
	int maxClientNum = 3;
	
	// 初始化链接库
	WSADATA wsaData;
	//设置链接库版
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// 设置监听地
	hostName = "127.0.0.1";
	// 服务队列最大数
	int maxService = 3;
	// 端口
	port = 9999;

	// 创建一个socket
	// 参数：地址协议族， socket类型， 传输协议,如果为0，系统自动根据前两个参数推演是什么类型。
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	// 与SOCKADD一样都是16字节，但是把ip和port分开了。方便赋值。可以使用强制类型转换用来做参数
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);
	//addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //0.0.0.0
	inet_pton(AF_INET, hostName, &addrServer.sin_addr.S_un.S_addr);
	// addrServer.sin_addr.S_addr = htonl(hostName);
	// htons() 将端口号由主机字节顺序转换为网络字节顺序
	// inet_addr() 将ip字符串转换成网络字节序的整数值。
	// inet_ntoa() 将sin_addr结构体输出成ip字符串。 network to assic

	
	int retVal = bind(sock, (SOCKADDR *)&addrServer, sizeof(SOCKADDR_IN));
	if (retVal == SOCKET_ERROR) {
		printf("初始化失败");
		return 0;
	}
	printf("参数设置完毕\n");
	
	// 开始监听
	if (listen(sock, maxClientNum) == SOCKET_ERROR) {
		printf("监听失败");
		return 0;
	}
	std::cout << "正在监听：" << hostName << ":" << port << std::endl;
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	
	while(1)
	{
		SOCKET sockConn = accept(sock, (SOCKADDR *)&addrClient, &len);
		while (1)
		{
			//准备数据
			clock_t start_time = clock();
			//准备数据
			imencode(".jpg", cv_img, data_encode);
			clock_t end_time = clock();
			std::cout << "编码时间" << end_time-start_time;
			
			char str[INET_ADDRSTRLEN];
			printf("连入地址 %s", inet_ntop(AF_INET, &addrClient.sin_addr, str, sizeof(str)));
			printf("ta来了");
			//测试缓冲区是否有数据
			if (data_encode.empty()) {
				char sendbuf[] = "摄像头未开启";
				send(sockConn, sendbuf, sizeof(sendbuf), 0);
			}
			
			//获取数据长度
			int len_encode = data_encode.size();
			//长度转换成字符串发送出去。
			std::string len_msg = std::to_string(len_encode);
			//构造长度信息为固定的16字节
			int ori_len = len_msg.length();
			for (int i = 0; i< 16 - ori_len; i++)
			{
				//拼接
				len_msg = len_msg + " ";
			}
			std::cout << "len_msg:" << len_msg.length() << std::endl;
			send(sockConn, len_msg.c_str(), 16, 0);

			//发送缓冲区数据

			char *send_b = new char[data_encode.size()];
			start_time = clock();

			for (int i = 0; i<data_encode.size(); i++)
			{
				//data_encode.size()数据装换成字符数组
				send_b[i] = data_encode[i];
			}
			int iSend = send(sockConn, send_b, data_encode.size(), 0);
			delete[]send_b;
			if (iSend == SOCKET_ERROR) {
				printf("发送失败");
				std::cout << "客户端断开，等待重新连接..." << std::endl;
				break;
			}
			end_time = clock();
			std::cout << "发送时间" << end_time - start_time;
		}
	}
	closesocket(sock);
	WSACleanup();
	thread_stop = true;
	return 0;
}

