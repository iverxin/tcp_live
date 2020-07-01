#include <iostream>

#include <WinSock2.h>

#include <tchar.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <WS2tcpip.h>
HANDLE m_hCam; //相机指针

//server
int main()
{
	//初始化链接库
	WSADATA wsaData;
	//设置链接库版本
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// 设置监听地址
	char * hostName = "127.0.0.1";
	// 服务队列最大数量
	int maxClientNum = 3;
	// 端口号
	unsigned short port = 9999;

	// 创建一个socket
	// 参数：地址协议族， socket类型， 传输协议,如果为0，系统自动根据前两个参数推演是什么类型。
	//
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 与SOCKADD一样都是16字节，但是把ip和port分开了。方便赋值。可以使用强制类型转换用来做参数。
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = 9999;
	// 注意sin_addr的结构。 htonl=
	// addrServer.sin_addr.S_addr = htonl(hostName);
	//htons() 将端口号由主机字节顺序转换为网络字节顺序
	// inet_addr() 将ip字符串转换成网络字节序的整数值。
	// inet_ntoa() 将sin_addr结构体输出成ip字符串。 network to assic

//	addrServer.sin_addr.S_un.S_addr = inet_addr(hostName);
	inet_pton(AF_INET,hostName, &addrServer.sin_addr.S_un.S_addr);
	// 绑定socket的参数
	if (bind(sock, (SOCKADDR*)&addrServer, sizeof(SOCKADDR_IN))<0) {
		std::cout << "初始化失败" << std::endl;
	}
	std::cout << "初始化成功" << std::endl;
	if (listen(sock, maxClientNum) == SOCKET_ERROR) {
		std::cout << "监听失败" << std::endl;
	}
	std::cout << "正在监听：" << hostName << ":" << port << std::endl;

	// 客户端声明
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	// 阻塞
	SOCKET sockConn = accept(sock, (SOCKADDR *)&addrClient, &len);

	while(1)
	{
		//准备数据
		//准备数据
		imencode(".jpg", cv_img, data_encode);
		char str[INET_ADDRSTRLEN];
		printf("连入地址 %s,端口\n", inet_ntop(AF_INET, &addrClient.sin_addr, str, sizeof(str)));
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
	}
	// 资源释放
	closesocket(sock);
	WSACleanup();
	return 0;
}
