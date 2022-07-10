#include "RequestCodec.h"

RequestCodec::RequestCodec()
{

}
RequestCodec::RequestCodec(string encStr)
{
	initMessage(encStr);
}
RequestCodec::RequestCodec(RequestInfo* Info)
{
	initMessage(Info);
}
void RequestCodec::initMessage(string encstr)
{
	m_encStr = encstr;
}
void RequestCodec::initMessage(RequestInfo* Info)
{
	m_msg.set_cmdtype(Info->cmd);
	m_msg.set_clientid(Info->clientId);
	m_msg.set_serverid(Info->severId);
	m_msg.set_sign(Info->sign);
	m_msg.set_data(Info->data);

}
string RequestCodec::encodeMsg()
{
	string output;
	m_msg.SerializeToString(&output);
	return output;
}
void* RequestCodec::decodeMsg()
{
	m_msg.ParseFromString(m_encStr);
	return &m_msg;
}