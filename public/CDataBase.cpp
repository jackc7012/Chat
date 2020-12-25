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

int CDataBase::SearchDataBaseLogin(const std::string& loginName, const std::string& loginPassword)
{
	int result = 0;
	char sql[100] = { 0 };
	sprintf_s(sql, 100, "select Password from tb_info where Name = '%s'", loginName.c_str());
	try
	{
		pRecordset = pMyConnect->Execute(sql, NULL, adCmdText);
		pRecordset->MoveFirst();
		_variant_t t = pRecordset->GetFields()->GetItem((long)0)->GetValue();
		std::string ss = (std::string)(_bstr_t)t;
	}
	catch (_com_error e)
	{
		logDataBase << "searchdatabaselogin failed descripton :" << (std::string)e.Description();
		logDataBase.PrintlogError(FILE_FORMAT);
		result = -1;
	}
	return result;
}
