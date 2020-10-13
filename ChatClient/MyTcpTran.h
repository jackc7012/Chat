#ifndef _MY_TCP_TRAN_H_
#define _MY_TCP_TRAN_H_

#define SOCKETBIND              1            // 服务器端监听本地端口等待客户端连接通信方式
#define SOCKETNOBIND            2            // 服务器端主动连接客户端通信方式
#define SOCKET_TIMEOUT          -100         // 套接字超时

#include <winsock2.h>

#include <string>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

class CMyTcpTran
{
public:
	CMyTcpTran();
	~CMyTcpTran();

public:
	static BOOL InitSocketLibrary(const int low, const int high);

public:
	// 初始化socket
	SOCKET InitSocket(const int socket_type, const string &bind_ip = "", const short port = 8000, const bool is_ansy = true);

	// 服务器接受连接
	bool Accept();

	// 客户端连接
	bool Connect();

	// 发送数据
	bool Send();

	// 接受数据
	bool Recv();

private:
	SOCKET m_socket_;
};
#endif // !_MY_TCP_TRAN_H_



