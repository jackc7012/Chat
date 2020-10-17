#ifndef __MY_NET_WORK_H__
#define __MY_NET_WORK_H__
#include <windows.h>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

#include "public.h"
#include "CLog.h"
#include "CDataBase.h"

#define WM_SOCKET					(WM_USER + 1000)
#define WM_LOGIN_INFO				(WM_SOCKET + 1)

namespace mywork {
	class CNetWork
	{
	public:
		CNetWork();
		~CNetWork();

	public:
		ReturnType InitSocketAndDataBase(const ConnectionType connectionType, const HWND hWnd, const std::string& databaseIp);

		BOOL CloseSocket();

		BOOL SendMessage(const SOCKET& clientSocket, const std::string& message);

	private:
		afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);

		void HandleRecvMessage(const HandleRecv& handleRecv);

		void HandleRecvRegister(const HandleRecv& handleRecv);

		void HandleRecvLogin(const HandleRecv& handleRecv);

		void HandleRecvDeleteCustomer(const HandleRecv& handleRecv);

		void HandleRecvChat(const HandleRecv& handleRecv);

		void HandleRecvTransferFileRequest(const HandleRecv& handleRecv);

		void HandleRecvTransferFileRespond(const HandleRecv& handleRecv);

		void HandleRecvTransferFile(const HandleRecv& handleRecv);

		inline void DeleteValue(const char* address) {
			delete[]address;
			address = nullptr;
		}
	private:
		SOCKET socketService{ 0 };
		SOCKADDR_IN addrService{ 0 }, addrAccept{ 0 };
		std::vector<SOCKET> socketAccept;
		DWORD accpetCount{ 0 };
		std::mutex serverHandle;
		std::unordered_map<std::string, std::pair<SOCKET, std::string>> nameToSocketIpAccept;	
		CLog gLog;
		CDataBase gDataBase;
		HWND hWnd;
	};
}

#endif //__MY_NET_WORK_H__

