#ifndef _MY_TCP_TRAN_H_
#define _MY_TCP_TRAN_H_

#define SOCKETBIND              1            // �������˼������ض˿ڵȴ��ͻ�������ͨ�ŷ�ʽ
#define SOCKETNOBIND            2            // ���������������ӿͻ���ͨ�ŷ�ʽ
#define SOCKET_TIMEOUT          -100         // �׽��ֳ�ʱ

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
	// ��ʼ��socket
	SOCKET InitSocket(const int socket_type, const string &bind_ip = "", const short port = 8000, const bool is_ansy = true);

	// ��������������
	bool Accept();

	// �ͻ�������
	bool Connect();

	// ��������
	bool Send();

	// ��������
	bool Recv();

private:
	SOCKET m_socket_;
};
#endif // !_MY_TCP_TRAN_H_



