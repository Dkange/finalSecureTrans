//#pragma once
#include<iostream>
#include "Message.pb.h"
//#include"RespondMsg.pb.h"
#include"Codec.h"
using namespace std;

struct RequestInfo
{
	int cmd;
	string clientId;
	string severId;
	string sign;
	string data;
};

class RequestCodec :public Codec
{
public:
	// 空对象
	RequestCodec();

	// 构造出的对象用于 解码 的场景
	RequestCodec(string encStr);

	// 构造出的对象用于 编码 的场景
	RequestCodec(RequestInfo* Info);

	// 给空构造准备的
	void initMessage(string encstr);
	void initMessage(RequestInfo* Info);

	// 重写的基类函数 用于返回序列化的字符串
	string encodeMsg();

	// 重写的基类函数 用于返回反序列化的数据（结构体）
	/*
		返回void*是因为有两个类RequestMsg以及RespondMsg需要覆写该函数，
		为了能够兼容这两个类(两个类的返回值不一样)所以返回void*，后续对其强制类型转换来获取数据
		*/
	void* decodeMsg();

private:
	string m_encStr;

	//要序列化的数据在RequestMsg类中， 通过该类进行序列化输出
	msg m_msg;
};

