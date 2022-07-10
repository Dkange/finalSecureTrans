#pragma once

#include<iostream>
//#include"RequestMsg.pb.h"
#include"Message.pb.h"
#include"Codec.h"
using namespace std;

struct RespondInfo
{
	int status;
	int seckeyID;
	string clientID;
	string serverID;
	string data;
};

class RespondCodec :public Codec
{
public:
	RespondCodec();
	RespondCodec(string enc);
	RespondCodec(RespondInfo *info);
	void initMessage(string enc);
	void initMessage(RespondInfo *info);
	string encodeMsg();
	void* decodeMsg();

	~RespondCodec();

private:
	string m_encstr;
	RespondMsg m_msg;
};

