#ifndef __MY_DATA_BASE_H__
#define __MY_DATA_BASE_H__
#include <windows.h>
#include <string>
#include <vector>

#include "public.h"

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")

namespace mywork {
	class CDataBase
	{
	public:
		CDataBase();
		~CDataBase();

	public:
		ReturnType InitDataBase(const std::string& ip);

		BOOL AddData(const std::string& sql);

		BOOL DeleteData(const std::string& sql);

		BOOL UpdateData(const std::string& sql);

		std::vector<std::string> SearchData(const std::string& sql);

		char* QueryPasswordFromUserName(const std::string& userName);

		ReturnType InsertUserInfo(const std::string& userName, const char *userPassword);

	private:
		_ConnectionPtr connectionPtr{ nullptr };
		_RecordsetPtr recordSetPtr{ nullptr };
	};
}

#endif //__MY_DATA_BASE_H__

