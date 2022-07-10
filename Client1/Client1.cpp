#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <thread>
#include "Message.pb.h"
#include "Hash.h"
#include <string>
#include "RsaCrypto.h"
#include "AesCrypto.h"
#include <fstream>
#include <sstream>
#pragma comment(lib,"ws2_32.lib")
using namespace msg;
static int id;

//�������Է���������Ϣ��Ҳ����ʱ������ת���Ŀͻ��˵���Ϣ��clientid = 0����������clientid = number������,clientid = ��������Ŀͻ���
void recvFromServer(SOCKET clientsocket, char* buf, string& key)
{
	sendMessage smsg;
	while (1) {
		memset(buf, '\0', sizeof(buf));
		if (recv(clientsocket, buf, 1024, 0) <= 0)
		{
			printf("�ر�����!\n");
			break;
			closesocket(clientsocket);
		}

		smsg.ParseFromArray(buf, 1024);//�����л�

		if (smsg.iskey())
		{
			key = smsg.data();
			cout << "�ԳƼ�����Կ�ѽ���:" << key << endl;
		}
		else
		{
			if (smsg.messageid() == 0) std::cout << "������" << ":" << endl;
			else if (smsg.messageid() == id) std::cout << "��" << ":" << endl;
			else std::cout << "�ͻ���" << smsg.messageid() << ":" << endl;
			std::cout << smsg.data() << "\n";
		}
	}
}
string key;
int main()
{
	SOCKET clientsocket;
	SOCKADDR_IN serveraddr;
	SOCKADDR_IN clientaddr;
	char buf[1024];


	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);	//��ʼ��WS2_32.DLL

										//�����׽���
	if ((clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
	{
		printf("�׽���socket����ʧ��!\n");
		system("pause");
		return -1;
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9102);
	serveraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//��������
	printf("����������...\n");
	if (connect(clientsocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) != 0)
	{
		printf("����ʧ��!\n");
		system("pause");
		return -1;
	}

	printf("���ӳɹ�!\n");

	//��������ʱ�����յ��ͻ��˷����Ĺ�Կ
	if (recv(clientsocket, buf, 1024, 0) <= 0)
	{
		printf("�ر�����!\n");
		return -1;
		closesocket(clientsocket);
	}
	sendMessage smsg;
	smsg.ParseFromArray(buf, 1024);
	std::cout << "������" << ":" << smsg.data() << "\n";
	id = smsg.messageid();//id��ֵ��number
	smsg.Clear();

	//�������ݣ���Ϊֻ��Ҫ�������ݸ������������Է�����Ϣ����ֱ�ӷ������߳�

	// 0. ������Կ��, ����Կ�ַ�������
	Cryptographic rsa;
	// ������Կ��
	rsa.generateRsakey(1024);
	// ����Կ�ļ�
	ifstream ifs("public.pem");
	stringstream str;
	str << ifs.rdbuf();

	sendMessage smsgs;
	char ss[1024];
	memset(ss, '\0', sizeof(ss));

	smsg.set_data(str.str());
	//cout << "str.str()" << str.str() << endl;

	smsg.set_messageid(id);
	smsg.set_iskey(true);
	//smsg.set_data(buf);
	smsg.SerializeToArray(ss, 1024);
	if (send(clientsocket, ss, strlen(ss) + 1, 0) <= 0)
	{
		printf("���ʹ���!\n");
		return -1;
	}
	else
	{
		printf("��Կ���ͳɹ�!\n");
	}

	std::thread task03(recvFromServer, clientsocket, buf, ref(key));//����������Ϣ�߳�
	task03.detach();

	cout << "key = " << key << endl;

	memset(ss, '\0', sizeof(ss));
	printf("����������:\n");

	while (1) {
		scanf("%s", buf);
		smsgs.set_messageid(id);
		smsgs.set_data(buf);
		smsgs.SerializeToArray(ss, 1024);

		if (send(clientsocket, ss, strlen(ss) + 1, 0) <= 0)
		{
			printf("���ʹ���!\n");
			break;
		}
	}

	closesocket(clientsocket);
	WSACleanup();    //�ͷ�WS2_32.DLL
	system("pause");
	return 0;
}