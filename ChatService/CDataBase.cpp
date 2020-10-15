#include "stdafx.h"
#include "CDataBase.h"

CDataBase::CDataBase()
	: connectPtr(nullptr)
	, recordSetPtr(nullptr)
{
}

CDataBase::~CDataBase()
{
	if (connectPtr->State) {
		connectPtr->Close();
	}
	connectPtr.Release();
	connectPtr = nullptr;
}

BOOL CDataBase::InitDataBase(const std::string& ip)
{
	HRESULT ret = CoInitialize(nullptr);
	if (ret != S_OK) {
		return FALSE;
	}

	ret = connectPtr.CreateInstance(__uuidof(Connection));
	if (ret != S_OK) {
		return FALSE;
	}

	char connectionParam[100] = { 0 };
	sprintf_s(connectionParam, 100, "Driver={sql server};server=%s;uid=sa;pwd=chen931125;database=MyChat;", ip.c_str());
	ret = connectPtr->Open(connectionParam, "", "", adModeUnknown);
	if (ret != S_OK) {
		return FALSE;
	}
	return TRUE;
}

BOOL CDataBase::AddData(const std::string& sql)
{
	HRESULT ret = connectPtr->Execute(sql.c_str(), NULL, adCmdText);
	if (ret != S_OK) {
		return FALSE;
	}
	return TRUE;
}

BOOL CDataBase::DeleteData(const std::string& sql)
{
	return 0;
}

BOOL CDataBase::UpdateData(const std::string& sql)
{
	return 0;
}

std::vector<std::string> CDataBase::SearchData(const std::string& sql)
{
	return std::vector<std::string>();
}
