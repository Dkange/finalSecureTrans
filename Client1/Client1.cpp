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
#include "json/json.h"
using namespace msg;
static int id;

//监听来自服务器的消息，也可能时服务器转发的客户端的消息，clientid = 0：服务器，clientid = number：本身,clientid = 其他：别的客户端
void recvFromServer(SOCKET clientsocket, char* buf, string& key)
{
	sendMessage smsg;
	while (1) {
		memset(buf, '\0', sizeof(buf));
		if (recv(clientsocket, buf, 1024, 0) <= 0)
		{
			printf("关闭连接!\n");
			break;
			closesocket(clientsocket);
		}

		smsg.ParseFromArray(buf, 1024);//反序列化

		if (smsg.iskey())
		{
			key = smsg.data();
			cout << "对称加密密钥已接收:" << key << endl;
		}
		else
		{
			if (smsg.messageid() == 0) std::cout << "服务器" << ":" << endl;
			else if (smsg.messageid() == id) std::cout << "我" << ":" << endl;
			else std::cout << "客户端" << smsg.messageid() << ":" << endl;
			std::cout << smsg.data() << "\n";
		}
	}
}
string key;
using namespace Json;
int main()
{
	SOCKET clientsocket;
	SOCKADDR_IN serveraddr;
	SOCKADDR_IN clientaddr;
	char buf[1024];

	//---------------------------------------json读取客户端配置
	string jsonFile = "./client.json";
	//解析json文件 ->Value
	ifstream ifs1(jsonFile);
	Reader r;
	Value root;
	r.parse(ifs1, root);

	// 将root中的键值对value值取出
	string serverIP = root["ServerIP"].asString();
	unsigned short port = root["Port"].asInt();

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);	//初始化WS2_32.DLL

										//创建套接字
	if ((clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
	{
		printf("套接字socket创建失败!\n");
		system("pause");
		return -1;
	}

	cout << serverIP << ", 端口:" << port << endl;

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.S_un.S_addr = inet_addr(serverIP.c_str());

	//请求连接
	printf("尝试连接中...\n");
	if (connect(clientsocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) != 0)
	{
		printf("连接失败!\n");
		system("pause");
		return -1;
	}

	printf("连接成功!\n");

	//刚连接上时，会收到客户端发来的公钥
	if (recv(clientsocket, buf, 1024, 0) <= 0)
	{
		printf("关闭连接!\n");
		return -1;
		closesocket(clientsocket);
	}
	sendMessage smsg;
	smsg.ParseFromArray(buf, 1024);
	std::cout << "服务器" << ":" << smsg.data() << "\n";
	id = smsg.messageid();//id赋值给number
	smsg.Clear();

	//发送数据，因为只需要发送数据给服务器，所以发送消息可以直接放在主线程

	// 0. 生成密钥对, 将公钥字符串读出
	Cryptographic rsa;
	// 生成密钥对
	rsa.generateRsakey(1024);
	// 读公钥文件
	ifstream ifs2("public.pem");
	stringstream str;
	str << ifs2.rdbuf();

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
		printf("发送错误!\n");
		return -1;
	}
	else
	{
		printf("公钥发送成功!\n");
	}

	std::thread task03(recvFromServer, clientsocket, buf, ref(key));//开启接收消息线程
	task03.detach();

	cout << "key = " << key << endl;

	memset(ss, '\0', sizeof(ss));
	printf("请任意输入:\n");

	while (1) {
		scanf("%s", buf);
		smsgs.set_messageid(id);
		smsgs.set_data(buf);
		smsgs.SerializeToArray(ss, 1024);

		if (send(clientsocket, ss, strlen(ss) + 1, 0) <= 0)
		{
			printf("发送错误!\n");
			break;
		}
	}

	closesocket(clientsocket);
	WSACleanup();    //释放WS2_32.DLL
	system("pause");
	return 0;
}