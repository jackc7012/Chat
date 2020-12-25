#ifndef __NET_WORK_HANDLE_H__
#define __NET_WORK_HANDLE_H__

#include <windows.h>
#include <string>

#include "CLog.h"

namespace mychat {
	const int TCP_PORT = 6000;
	const int UDP_PORT = 6002;

	class CNetWorkHandle
	{
	public:
		static CNetWorkHandle* CreateInstance();

		int HandleRecv(const std::string& ip, const std::string& message, const s_HandleRecv& handleRecv);

		~CNetWorkHandle();

	private:
		CNetWorkHandle();

		CNetWorkHandle(const CNetWorkHandle&) = delete;
		CNetWorkHandle operator=(const CNetWorkHandle&) = delete;

		int HandleRecvTcp(const s_HandleRecv& handleRecv);

		int HandleRecvUdp(const s_HandleRecv& handleRecv);

	private:
		CLog logNetWork;
	};
}
#endif // !__NET_WORK_HANDLE_H__