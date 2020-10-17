#include "stdafx.h"
#include "CNetWork.h"

namespace mywork {
	CNetWork::CNetWork()
	{
        gLog.InitLog("./network", INFO_LEVEL, true);
	}

	CNetWork::~CNetWork()
	{
		CloseSocket();
	}

	ReturnType CNetWork::InitSocketAndDataBase(const ConnectionType connectionType, const HWND hWnd, const std::string& databaseIp)
	{
		if (socketService != 0) {
			return ReturnType::C_SOCKET_FAILED;
		}
		if (connectionType == ConnectionType::TCP) {
			socketService = ::socket(AF_INET, SOCK_STREAM, 0);
			if (socketService == INVALID_SOCKET) {
				return ReturnType::C_SOCKET_FAILED;
			}
			else {
				addrService.sin_family = AF_INET;
				addrService.sin_port = htons(SOCKET_PORT);
				addrService.sin_addr.S_un.S_addr = INADDR_ANY;
				::bind(socketService, (sockaddr*)&addrService, sizeof(addrService));
				::listen(socketService, SOMAXCONN);
				::WSAAsyncSelect(socketService, hWnd, WM_SOCKET, FD_ACCEPT | FD_READ | FD_CLOSE);
                this->hWnd = hWnd;
                if (gDataBase.InitDataBase(databaseIp) != ReturnType::C_OK) {
                    return ReturnType::C_DATABASE_FAILED;
                }
				return ReturnType::C_OK;
			}
		}
		else if (connectionType == ConnectionType::UDP) {
			return ReturnType::C_SOCKET_FAILED;
		}
		
		return ReturnType::C_OK;
	}

	BOOL CNetWork::CloseSocket()
	{
		if (socketService == 0) {
			return FALSE;
		}
		::closesocket(socketService);
		socketService = 0;
		return TRUE;
	}

	BOOL CNetWork::SendMessage(const SOCKET& clientSocket, const std::string& message)
	{
		return 0;
	}

    LRESULT CNetWork::OnSocket(WPARAM wParam, LPARAM lParam) {
        CString strText;
        switch (lParam) {
        case FD_ACCEPT: {
            int addrLen = sizeof(addrAccept);
            socketAccept.push_back(::accept(socketService, (SOCKADDR*)&addrAccept, &addrLen));
            std::string ip = ::inet_ntoa(addrAccept.sin_addr);
            gLog << "ip = " << ip << " connect";
            gLog.PrintlogInfo(FILE_FORMAT);
            ++accpetCount;
        }
            break;

        case FD_READ: {
            char* strRecv = new char[DATA_LENGTH];
            memset(strRecv, 0, DATA_LENGTH);
            unsigned int i = 0;
            for (; i < socketAccept.size(); ++i) {
                int i_return = ::recv(socketAccept[i], strRecv, DATA_LENGTH, 0);
                if (i_return > 0)
                    break;
            }
            HandleRecv handleRecv;
            DecodeJson(strRecv, handleRecv);
            handleRecv.socketAccept = socketAccept[i];
            gLog << "recv message from " << handleRecv.param.login.ip << ", message content = " << strRecv;
            gLog.PrintlogInfo(FILE_FORMAT);
            HandleRecvMessage(handleRecv);
            delete[]strRecv;
        }
            break;

        case FD_CLOSE: {
        }
            break;

        default: {

        }
            break;
        }

        return 0;
    }

    void CNetWork::HandleRecvMessage(const HandleRecv& handleRecv) {
        std::lock_guard<std::mutex> mt(serverHandle);
        switch (handleRecv.communicationType) {
        case CommunicationType::NULLCOMMUNICATION:
            break;

        case CommunicationType::ERRORCOMMUNICATION:
            break;

        // 用户注册
        case CommunicationType::REGISTER: {
            HandleRecvRegister(handleRecv);
        }
            break;

        // 用户登录
        case CommunicationType::LOGIN: {
            HandleRecvLogin(handleRecv);
        }
            break;

        // 用户退出
        case CommunicationType::DELETECUSTOMER: {
            HandleRecvDeleteCustomer(handleRecv);
        }
            break;

        // 聊天
        case CommunicationType::CHAT: {
            HandleRecvChat(handleRecv);
        }
           break;

        // 传送文件请求
        case CommunicationType::TRANSFERFILEREQUEST: {
            HandleRecvTransferFileRequest(handleRecv);
        }
           break;

        // 传送文件应答
        case CommunicationType::TRANSFERFILERESPOND: {
            HandleRecvTransferFileRespond(handleRecv);
        }
           break;

        // 传送文件
        case CommunicationType::TRANSFERFILE: {
            HandleRecvTransferFile(handleRecv);
        }
           break;

        // 踢人
        case CommunicationType::FORCEDELETE: {

        }
           break;
        }

        return;
    }

    void CNetWork::HandleRecvRegister(const HandleRecv& handleRecv) {
        std::string jsSend;
        HandleRecv toSend;
        char* password = gDataBase.QueryPasswordFromUserName(handleRecv.param.registerIn.customer);
        gLog << "customer " << handleRecv.param.registerIn.customer << " register ";
        if (password != nullptr) {
            toSend.param.registerBack.customer = handleRecv.param.registerIn.customer;
            toSend.param.registerBack.registerResult = "failed";
            toSend.param.registerBack.description = "user name is already have";
            jsSend = EncodeJson(CommunicationType::REGISTERBACKFAILED, toSend);
            gLog << "result failed description is " << toSend.param.registerBack.description;
            gLog.PrintlogError(FILE_FORMAT);
            DeleteValue(password);
        }
        else {
            toSend.param.registerBack.customer = handleRecv.param.registerIn.customer;
            toSend.param.registerBack.registerResult = "succeed";
            gDataBase.InsertUserInfo(handleRecv.param.registerIn.customer, handleRecv.param.registerIn.password);
            jsSend = EncodeJson(CommunicationType::REGISTERBACKSUCCEED, toSend);
            gLog << "result succeed";
            gLog.PrintlogInfo(FILE_FORMAT);
        }
        ::send(handleRecv.socketAccept, jsSend.c_str(), jsSend.length(), 0);
        gLog << "send message :" << jsSend;
        gLog.PrintlogInfo(FILE_FORMAT);
        DeleteValue(handleRecv.param.registerIn.customer);
        DeleteValue(handleRecv.param.registerIn.password);
    }

    void CNetWork::HandleRecvLogin(const HandleRecv& handleRecv) {
        std::string jsSend;
        HandleRecv toSend;
        char password[MAX_PATH] = { 0 };
        bool isSucceed = false;
        gLog << "customer " << handleRecv.param.login.customer << " login ";
        if (nameToSocketIpAccept.find(handleRecv.param.login.customer) != nameToSocketIpAccept.end()) {
            toSend.param.loginBack.customer = handleRecv.param.login.customer;
            toSend.param.loginBack.loginResult = "failed";
            toSend.param.loginBack.description = "this people has already login";
            jsSend = EncodeJson(CommunicationType::LOGINBACKFAILED, toSend);
            gLog << "result failed description is " << toSend.param.loginBack.description;
            gLog.PrintlogError(FILE_FORMAT);
        }
        else {
            char *password = gDataBase.QueryPasswordFromUserName(handleRecv.param.login.customer);
            if (password == nullptr) {
                toSend.param.loginBack.customer = handleRecv.param.login.customer;
                toSend.param.loginBack.loginResult = "failed";
                toSend.param.loginBack.description = "no such people";
                jsSend = EncodeJson(CommunicationType::LOGINBACKFAILED, toSend);
                gLog << "result failed description is " << toSend.param.loginBack.description;
                gLog.PrintlogError(FILE_FORMAT);
            }
            else {
                if (strcmp(password, handleRecv.param.login.password) == 0) {
                    toSend.param.loginBack.customer = handleRecv.param.login.customer;
                    toSend.param.loginBack.loginResult = "succeed";
                    jsSend = EncodeJson(CommunicationType::LOGINBACKSUCCEED, toSend);
                    isSucceed = true;
                    gLog << "result succeed";
                    gLog.PrintlogInfo(FILE_FORMAT);
                }
                else {
                    toSend.param.loginBack.customer = handleRecv.param.login.customer;
                    toSend.param.loginBack.loginResult = "failed";
                    toSend.param.loginBack.description = "password error";
                    jsSend = EncodeJson(CommunicationType::LOGINBACKFAILED, toSend);
                    gLog << "result failed description is " << toSend.param.loginBack.description;
                    gLog.PrintlogError(FILE_FORMAT);
                }
                DeleteValue(password);
            }
        }
        ::send(handleRecv.socketAccept, jsSend.c_str(), jsSend.length(), 0);
        gLog << "send message :" << jsSend;
        gLog.PrintlogInfo(FILE_FORMAT);
        if (isSucceed) {
            std::pair<SOCKET, std::string> socketAndIp(std::pair<SOCKET, std::string>(handleRecv.socketAccept, handleRecv.param.login.ip));
            nameToSocketIpAccept.insert(std::pair<std::string, std::pair<SOCKET, std::string>>(handleRecv.param.login.customer, socketAndIp));
            toSend.param.showLogin.customerNum = nameToSocketIpAccept.size();
            toSend.param.showLogin.customer = new char*[toSend.param.showLogin.customerNum];
            unsigned int i = 0;
            for (auto itor = nameToSocketIpAccept.cbegin(); itor != nameToSocketIpAccept.cend(); ++itor, ++i) {
                toSend.param.showLogin.customer[i] = const_cast<char*>(itor->first.c_str());
            }   
            jsSend = EncodeJson(CommunicationType::SHOWLOGIN, toSend);
            gLog << "send message for all :" << jsSend;
            gLog.PrintlogInfo(FILE_FORMAT);
            delete[]toSend.param.showLogin.customer;
            toSend.param.showLogin.customer = nullptr;
            for (auto itor = nameToSocketIpAccept.cbegin(); itor != nameToSocketIpAccept.cend(); ++itor) {
                ::send(itor->second.first, jsSend.c_str(), jsSend.length(), 0);
            }
        }
        ::PostMessage(hWnd, WM_LOGIN_INFO, (WPARAM)&nameToSocketIpAccept, 0);
        DeleteValue(handleRecv.param.login.customer);
        DeleteValue(handleRecv.param.login.password);
    }

    void CNetWork::HandleRecvDeleteCustomer(const HandleRecv& handleRecv) {
        std::string jsSend;
        HandleRecv toSend;
        nameToSocketIpAccept.erase(handleRecv.param.delCustomer.customer);
        gLog << "delete customer " << handleRecv.param.delCustomer.customer << " succeed";
        gLog.PrintlogInfo(FILE_FORMAT);
        toSend.param.delCustomer.customer = handleRecv.param.delCustomer.customer;
        jsSend = EncodeJson(CommunicationType::DELETECUSTOMER, toSend);
        for (auto itor = nameToSocketIpAccept.cbegin(); itor != nameToSocketIpAccept.cend(); ++itor) {
            ::send(itor->second.first, jsSend.c_str(), jsSend.length(), 0);
        }
        DeleteValue(handleRecv.param.delCustomer.customer);
        ::PostMessage(hWnd, WM_LOGIN_INFO, (WPARAM)&nameToSocketIpAccept, 1);
    }

    void CNetWork::HandleRecvChat(const HandleRecv& handle_recv) {
        std::string js_str_send;
        HandleRecv to_send;
        auto itor = name_to_socket_accept.find(handle_recv.Param.Chat.target);
        to_send.Param.Chat.source = handle_recv.Param.Chat.source;
        to_send.Param.Chat.target = handle_recv.Param.Chat.target;
        to_send.Param.Chat.content = handle_recv.Param.Chat.content;
        js_str_send = EncodeJson(CHAT, to_send);
        g_log << "send message :" << js_str_send;
        g_log.PrintlogInfo(FILE_FORMAT);
        ::send(itor->second, js_str_send.c_str(), js_str_send.length(), 0);
        delete[]handle_recv.Param.Chat.source;
        delete[]handle_recv.Param.Chat.target;
        delete[]handle_recv.Param.Chat.content;
    }

    void CNetWork::HandleRecvTransferFileRequest(const HandleRecv& handle_recv) {
        std::string js_str_send;
        HandleRecv to_send;
        auto itor = name_to_socket_accept.find(handle_recv.Param.TransferFileRequest.target);
        to_send.Param.TransferFileRequest.source = handle_recv.Param.TransferFileRequest.source;
        to_send.Param.TransferFileRequest.target = handle_recv.Param.TransferFileRequest.target;
        to_send.Param.TransferFileRequest.file_name = handle_recv.Param.TransferFileRequest.file_name;
        to_send.Param.TransferFileRequest.file_length = handle_recv.Param.TransferFileRequest.file_length;
        js_str_send = EncodeJson(TRANSFERFILEREQUEST, to_send);
        g_log << "send message :" << js_str_send;
        g_log.PrintlogInfo(FILE_FORMAT);
        ::send(itor->second, js_str_send.c_str(), js_str_send.length(), 0);
        delete[]handle_recv.Param.TransferFileRequest.source;
        delete[]handle_recv.Param.TransferFileRequest.target;
        delete[]handle_recv.Param.TransferFileRequest.file_name;
        delete[]handle_recv.Param.TransferFileRequest.file_length;
    }

    void CNetWork::HandleRecvTransferFileRespond(const HandleRecv& handle_recv) {
        std::string js_str_send;
        HandleRecv to_send;
        auto itor = name_to_socket_accept.find(handle_recv.Param.TransferFileRespond.target);
        to_send.Param.TransferFileRespond.source = handle_recv.Param.TransferFileRespond.source;
        to_send.Param.TransferFileRespond.target = handle_recv.Param.TransferFileRespond.target;
        to_send.Param.TransferFileRespond.file_name = handle_recv.Param.TransferFileRespond.file_name;
        to_send.Param.TransferFileRespond.transfer_result = handle_recv.Param.TransferFileRespond.transfer_result;
        js_str_send = EncodeJson(TRANSFERFILERESPOND, to_send);
        g_log << "send message :" << js_str_send;
        g_log.PrintlogInfo(FILE_FORMAT);
        ::send(itor->second, js_str_send.c_str(), js_str_send.length(), 0);
        delete[]handle_recv.Param.TransferFileRespond.source;
        delete[]handle_recv.Param.TransferFileRespond.target;
        delete[]handle_recv.Param.TransferFileRespond.file_name;
        delete[]handle_recv.Param.TransferFileRespond.transfer_result;
    }

    void CNetWork::HandleRecvTransferFile(const HandleRecv& handle_recv) {
        std::string js_str_send;
        HandleRecv to_send;
        auto itor = name_to_socket_accept.find(handle_recv.Param.TransferFile.target);
        to_send.Param.TransferFile.source = handle_recv.Param.TransferFile.source;
        to_send.Param.TransferFile.target = handle_recv.Param.TransferFile.target;
        to_send.Param.TransferFile.file_name = handle_recv.Param.TransferFile.file_name;
        to_send.Param.TransferFile.file_length = handle_recv.Param.TransferFile.file_length;
        to_send.Param.TransferFile.file_block = handle_recv.Param.TransferFile.file_block;
        to_send.Param.TransferFile.file_content = handle_recv.Param.TransferFile.file_content;
        to_send.Param.TransferFile.current_block = handle_recv.Param.TransferFile.current_block;
        js_str_send = EncodeJson(TRANSFERFILE, to_send);
        g_log << "send message :" << js_str_send;
        g_log.PrintlogInfo(FILE_FORMAT);
        ::send(itor->second, js_str_send.c_str(), js_str_send.length(), 0);
        delete[]handle_recv.Param.TransferFile.source;
        delete[]handle_recv.Param.TransferFile.target;
        delete[]handle_recv.Param.TransferFile.file_name;
        delete[]handle_recv.Param.TransferFile.file_content;
        delete[]handle_recv.Param.TransferFile.file_length;
    }
}
