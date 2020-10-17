#include "stdafx.h"
#include "CDataBase.h"

namespace mywork {
	CDataBase::CDataBase()
	{
	}

	CDataBase::~CDataBase()
	{
		if (connectionPtr->State) {
			connectionPtr->Close();
		}
		connectionPtr.Release();
		connectionPtr = nullptr;
	}

	ReturnType CDataBase::InitDataBase(const std::string& ip)
	{
		HRESULT ret = CoInitialize(nullptr);
		if (ret != S_OK) {
			return ReturnType::C_DATABASE_FAILED;
		}

		ret = connectionPtr.CreateInstance(__uuidof(Connection));
		if (ret != S_OK) {
			return ReturnType::C_DATABASE_FAILED;
		}

		char connectionParam[100] = { 0 };
		sprintf_s(connectionParam, 100, "Driver={sql server};server=%s;uid=sa;pwd=chen931125;database=MyChat;", ip.c_str());
		ret = connectionPtr->Open(connectionParam, "", "", adModeUnknown);
		if (ret != S_OK) {
			return ReturnType::C_DATABASE_FAILED;
		}
		return ReturnType::C_OK;
	}

	BOOL CDataBase::AddData(const std::string& sql)
	{
		HRESULT ret = connectionPtr->Execute(sql.c_str(), NULL, adCmdText);
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

	char* CDataBase::QueryPasswordFromUserName(const std::string& userName)
	{
		char sql[50] = { 0 };
		sprintf_s(sql, 50, "sql Password from tb_info where Name = '%s'", userName.c_str());
		recordSetPtr = connectionPtr->Execute(sql, NULL, adCmdText);
		std::vector<_bstr_t> result;
		while (!recordSetPtr->adoEOF) {
			_bstr_t res = (_bstr_t)recordSetPtr->GetFields()->GetItem("Password")->Value;
			result.push_back(res);
			recordSetPtr->MoveNext();
		}
		if (result.size() != 1) {
			return nullptr;
		}
		char* returnResult = new char[std::string(result[0]).length() + 1];
		memcpy_s(returnResult, std::string(result[0]).length() + 1, std::string(result[0]).c_str(), std::string(result[0]).length() + 1);
		return returnResult;
	}

	ReturnType CDataBase::InsertUserInfo(const std::string& userName, const char* userPassword)
	{
		char sql[50] = { 0 };
		sprintf_s(sql, 50, "insert into tb_info(Name,Password) values('%s', '%s')", userName.c_str(), userPassword);
		recordSetPtr = connectionPtr->Execute(sql, NULL, adCmdText);
		return ReturnType::C_OK;
	}
}

