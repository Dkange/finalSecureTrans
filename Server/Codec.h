#pragma once
#include<iostream>
using namespace std;
class Codec
{
public:
	Codec();
	virtual ~Codec();

	// 重写的基类函数 用于返回序列化的字符串
	virtual string encodeMsg();

	// 重写的基类函数 用于返回反序列化的数据（结构体）
	/*
		返回void*是因为有两个类RequestMsg以及RespondMsg需要覆写该函数，
		为了能够兼容这两个类所以返回void*，后续对其强制类型转换来获取数据
		*/
	virtual void* decodeMsg();
};

