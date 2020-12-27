#include "CDataBase.h"
using namespace mychat;

CDataBase::CDataBase()
{
	logDataBase.InitLog("../{time}/database");
}

CDataBase* CDataBase::CreateInstance()
{
	static CDataBase* ptr = nullptr;
	if (ptr == nullptr) {
		ptr = new CDataBase();
	}
	return ptr;
}

CDataBase::~CDataBase()
{
}

int CDataBase::InitDataBase(const std::string& dataBaseName)
{
	if (pMyConnect.CreateInstance(__uuidof(Connection)) != 0) {
		logDataBase << "ConnectionPtr create failed";
		logDataBase.PrintlogError(FILE_FORMAT);
		return -1;
	}
	if (pRecordset.CreateInstance(__uuidof(Recordset)) != 0) {
		logDataBase << "RecordsetPtr create failed";
		logDataBase.PrintlogError(FILE_FORMAT);
		return -1;
	}
	char connectionString[100] = { 0 };
	sprintf_s(connectionString, 100, "Driver={sql server};server=127.0.0.1;uid=sa;pwd=chen931125;database=%s;",							dataBaseName.c_str());
	if (pMyConnect->Open(connectionString, "", "", adModeUnknown) != 0) {
		logDataBase << "open database" << dataBaseName.c_str() << "failed";
		logDataBase.PrintlogError(FILE_FORMAT);
		return -1;
	}
	return 0;
}

int CDataBase::SearchDataBaseLogin(const std::string& loginName, std::string& ip, char* password, int loginStatus)
{
	int result = 0;
	char sql[100] = { 0 };
	sprintf_s(sql, 100, "select Password, Ip, IsLogin from tb_info where Name = '%s'", loginName.c_str());
	try
	{
		pRecordset = pMyConnect->Execute(sql, NULL, adCmdText);
		_variant_t sqlPassword = pRecordset->GetFields()->GetItem((long)0)->GetValue();
		_variant_t sqlIp = pRecordset->GetFields()->GetItem((long)1)->GetValue();
		_variant_t sqlIsLogin = pRecordset->GetFields()->GetItem((long)2)->GetValue();
		memcpy_s(password, 50, (char*)(_bstr_t)&sqlPassword, strlen((_bstr_t)&sqlPassword) + 1);
		ip = (std::string)(_bstr_t)&sqlIp;
		loginStatus = (int)&sqlIsLogin;
	}
	catch (_com_error e)
	{
		logDataBase << "searchdatabaselogin failed descripton :" << (std::string)e.Description();
		logDataBase.PrintlogError(FILE_FORMAT);
		result = -1;
	}
	return result;
}

int CDataBase::UpdateLoginStatus(const std::string& loginName, const int type)
{
	int result = 0;
	char sql[100] = { 0 };
	sprintf_s(sql, 100, "update tb_info set IsLogin = %d where Name = %s", type, loginName.c_str());
	try
	{
		pRecordset = pMyConnect->Execute(sql, NULL, adCmdText);
	}
	catch (_com_error e)
	{
		logDataBase << "updateloginstatus " << type << " failed descripton :" << (std::string)e.Description();
		logDataBase.PrintlogError(FILE_FORMAT);
		result = -1;
	}
	return result;
}
