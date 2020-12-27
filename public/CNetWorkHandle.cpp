#include "CNetWorkHandle.h"
using namespace mychat;

CNetWorkHandle::CNetWorkHandle()
{
    logNetWork.InitLog("../{time}/network");
	dataBase = CDataBase::CreateInstance();
	dataBase->InitDataBase("mychat");
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

CommunicationType CNetWorkHandle::HandleRecv(const std::string& message, s_HandleRecv& handleRecv, std::string& strToSend)
{
	std::string description;
	CommunicationType result = CommunicationType::NULLCOMMUNICATION;
	logNetWork << "recv " << (handleRecv.connectionType == ConnectionType::TCP ? "TCP" : "UDP") <<
		" message from " << handleRecv.ip << " , message content = " << message;
	logNetWork.PrintlogInfo(FILE_FORMAT);
	DecodeJson(message, handleRecv);
	s_HandleRecv toSend;
	toSend.type = handleRecv.type;
	switch (handleRecv.type) {
	case CommunicationType::LOGIN: {
		result = CommunicationType::LOGINBACK;
		toSend.param.LoginBack.customer = handleRecv.param.Login.customer;
		toSend.param.LoginBack.login_result = "failed";
		char* password = new char[50];
		if (password == nullptr) {
			result = CommunicationType::ERRORCOMMUNICATION;
		}
		else {
			memset(password, 0, 50);
			std::string ip;
			int IsLogin = -1;
			int nRet = dataBase->SearchDataBaseLogin(handleRecv.param.Login.customer, ip, password, IsLogin);
			if (nRet != 0) {
				description = "no such people";
			}
			else if (IsLogin == 1) {
				description = "people has already login";
			}
			else if (strcmp(password, handleRecv.param.Login.password) != 0) {
				description = "password error";
			}
			else if (strcmp(password, handleRecv.param.Login.password) == 0) {
				description = ((ip == handleRecv.ip) ? "login succeed" : "ip does not match last time");
				toSend.param.LoginBack.login_result = "succeed";
				dataBase->UpdateLoginStatus(handleRecv.param.Login.customer, 1);
			}
			toSend.param.LoginBack.description = (char *)description.c_str();
			DeleteMemory(CommunicationType::LOGIN, handleRecv);
			delete[]password;
		}	
	}
    break;

	}
	toSend.type = result;
	strToSend = EncodeJson(result, toSend);
	return result;
}