#ifndef __NET_WORK_HANDLE_H__
#define __NET_WORK_HANDLE_H__

namespace mychat {
	enum class ConnectionType {
		TCP = 0,
		UDP
	};

	const int TCP_PORT = 6000;
	const int UDP_PORT = 6002;

	class CNetWorkHandle
	{
	public:
		CNetWorkHandle();

		~CNetWorkHandle();
	};
}
#endif // !__NET_WORK_HANDLE_H__