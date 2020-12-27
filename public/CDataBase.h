#ifndef __DATA_BASE_H__
#define __DATA_BASE_H__

#include <windows.h>
#include <string>

#include "CLog.h"

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")

namespace mychat {
	class CDataBase
	{
	public:
		static CDataBase* CreateInstance();

		~CDataBase();

		int InitDataBase(const std::string& dataBaseName);

		int SearchDataBaseLogin(const std::string& loginName, std::string& ip, char *password, int loginStatus);

		int UpdateLoginStatus(const std::string& loginName, const int type);

	private:
		CDataBase();

		CDataBase(const CDataBase&) = delete;
		CDataBase operator=(const CDataBase&) = delete;

	private:
		_ConnectionPtr pMyConnect{ nullptr };
		_RecordsetPtr pRecordset{ nullptr };
		CLog logDataBase;
	};
}

#endif // !__DATA_BASE_H__