#include "stdafx.h"
#include "MyTcpTran.h"


CMyTcpTran::CMyTcpTran():
	m_socket_(INVALID_SOCKET)
{
	
}

CMyTcpTran::~CMyTcpTran()
{
}

BOOL CMyTcpTran::InitSocketLibrary(const int low, const int high)
{
	WORD wVersion = 0;
	int errret = -1;
	WSADATA wsaData;

	wVersion = MAKEWORD(low, high);

	errret = WSAStartup(wVersion, &wsaData);

	if (2 != LOBYTE(wsaData.wVersion) || 2 != HIBYTE(wsaData.wVersion))
	{
		MessageBox(NULL, "winsocket库版本低", "提示", MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

SOCKET CMyTcpTran::InitSocket(const int socket_type, const string &bind_ip/* = ""*/, const short port/* = 8000*/, const bool is_ansy/* = true*/)
{
	SOCKET socket_id = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == socket_id)
	{
		MessageBox(NULL, "创建服务失败", "提示", MB_ICONERROR);
	}
	SOCKADDR_IN socket_addr;
	socket_addr.sin_family = AF_INET;
	if (bind_ip.empty())
	{
		socket_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	else
	{
		socket_addr.sin_addr.S_un.S_addr = inet_addr(bind_ip.c_str);
	}
	socket_addr.sin_port = htons(port);

	bind(socket_id, (sockaddr *)&socket_addr, sizeof(socket_addr));
	listen(socket_id, SOMAXCONN);

	if (is_ansy)
	{
		WSAAsyncSelect(socket_id, this->m_hWnd, WM_SOCKET, FD_ACCEPT | FD_READ | FD_CLOSE);
	}

	return SOCKET();
}

