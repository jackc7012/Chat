#ifndef __NET_WORK_HANDLE_H__
#define __NET_WORK_HANDLE_H__

#include <windows.h>
#include <string>

#include "CDataBase.h"
#include "CLog.h"

namespace mychat {
	const int TCP_PORT = 6000;
	const int UDP_PORT = 6002;

	class CNetWorkHandle
	{
	public:
		static CNetWorkHandle* CreateInstance();

		CommunicationType HandleRecv(const std::string& message, s_HandleRecv& handleRecv, 
			                         std::string& strToSend);

		~CNetWorkHandle();

	private:
		CNetWorkHandle();

		CNetWorkHandle(const CNetWorkHandle&) = delete;
		CNetWorkHandle operator=(const CNetWorkHandle&) = delete;

	private:
		CLog logNetWork;
		CDataBase* dataBase;
	};
}
#endif // !__NET_WORK_HANDLE_H__