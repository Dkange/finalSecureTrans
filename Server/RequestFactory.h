#pragma once
#include "CodecFactory.h"
#include "RequestCodec.h"
#include "Codec.h"
class RequestFactory:public CodecFactory
{
public:
	RequestFactory(std::string enc);
	RequestFactory(RequestInfo *info);
	Codec* createCodec();
	~RequestFactory();

private:
	bool m_flag;
	std::string m_encStr;
	RequestInfo* m_info;
};

