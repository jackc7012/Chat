#ifndef __MY_DATA_BASE_H__
#define __MY_DATA_BASE_H__
#include <windows.h>
#include <string>
#include <vector>

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")
class CDataBase
{
public:
	CDataBase();
	~CDataBase();

	BOOL InitDataBase(const std::string &ip);

	BOOL AddData(const std::string& sql);

	BOOL DeleteData(const std::string& sql);

	BOOL UpdateData(const std::string& sql);

	std::vector<std::string>  SearchData(const std::string& sql);

private:
	_ConnectionPtr connectPtr;
	_RecordsetPtr recordSetPtr;
};

#endif

