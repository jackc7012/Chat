#include "CNetWorkHandle.h"
using namespace mychat;

CNetWorkHandle::CNetWorkHandle()
{
    logNetWork.InitLog("../{time}/network");
}

CNetWorkHandle* CNetWorkHandle::CreateInstance()
{
	static CNetWorkHandle* ptr = nullptr;
	if (ptr == nullptr) {
		ptr = new CNetWorkHandle();
	}
	return ptr;
}

CNetWorkHandle::~CNetWorkHandle()
{
}

int CNetWorkHandle::HandleRecv(const std::string& ip, const std::string& message, const s_HandleRecv& handleRecv)
{
	logNetWork << "recv " << (handleRecv.connectionType == ConnectionType::TCP ? "TCP" : "UDP") <<
		" message from " << ip << ", message content = " << message;
	logNetWork.PrintlogInfo(FILE_FORMAT);
	s_HandleRecv temp(handleRecv);
	DecodeJson(message, temp);
	return 0;
}

int CNetWorkHandle::HandleRecvTcp(const s_HandleRecv& handleRecv)
{
	return 0;
}

int CNetWorkHandle::HandleRecvUdp(const s_HandleRecv& handleRecv)
{
	return 0;
}
