#include <iostream>

#include <WinSock2.h>

#include <tchar.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <WS2tcpip.h>
HANDLE m_hCam; //���ָ��

//server
int main()
{
	//��ʼ�����ӿ�
	WSADATA wsaData;
	//�������ӿ�汾
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// ���ü�����ַ
	char * hostName = "127.0.0.1";
	// ��������������
	int maxClientNum = 3;
	// �˿ں�
	unsigned short port = 9999;

	// ����һ��socket
	// ��������ַЭ���壬 socket���ͣ� ����Э��,���Ϊ0��ϵͳ�Զ�����ǰ��������������ʲô���͡�
	//
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// ��SOCKADDһ������16�ֽڣ����ǰ�ip��port�ֿ��ˡ����㸳ֵ������ʹ��ǿ������ת��������������
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = 9999;
	// ע��sin_addr�Ľṹ�� htonl=
	// addrServer.sin_addr.S_addr = htonl(hostName);
	//htons() ���˿ں��������ֽ�˳��ת��Ϊ�����ֽ�˳��
	// inet_addr() ��ip�ַ���ת���������ֽ��������ֵ��
	// inet_ntoa() ��sin_addr�ṹ�������ip�ַ����� network to assic

//	addrServer.sin_addr.S_un.S_addr = inet_addr(hostName);
	inet_pton(AF_INET,hostName, &addrServer.sin_addr.S_un.S_addr);
	// ��socket�Ĳ���
	if (bind(sock, (SOCKADDR*)&addrServer, sizeof(SOCKADDR_IN))<0) {
		std::cout << "��ʼ��ʧ��" << std::endl;
	}
	std::cout << "��ʼ���ɹ�" << std::endl;
	if (listen(sock, maxClientNum) == SOCKET_ERROR) {
		std::cout << "����ʧ��" << std::endl;
	}
	std::cout << "���ڼ�����" << hostName << ":" << port << std::endl;

	// �ͻ�������
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	// ����
	SOCKET sockConn = accept(sock, (SOCKADDR *)&addrClient, &len);

	while(1)
	{
		//׼������
		//׼������
		imencode(".jpg", cv_img, data_encode);
		char str[INET_ADDRSTRLEN];
		printf("�����ַ %s,�˿�\n", inet_ntop(AF_INET, &addrClient.sin_addr, str, sizeof(str)));
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
	}
	// ��Դ�ͷ�
	closesocket(sock);
	WSACleanup();
	return 0;
}
