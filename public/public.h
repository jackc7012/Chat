#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include "../json/json.h"
#include <windows.h>
#include <vector>
#include <unordered_map>
#include <sstream>

namespace cwy {
    const int DATA_LENGTH = 1024 * 10;

    const std::string SERVER_IP = "192.168.1.3";

    static const char* VERIFY_CODE = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    static std::string nick_name;

    inline void HANDLE_MESSAGE(char** type, const unsigned int length, const std::string& message) {
        *type = new char[length + 1];
        if (*type != nullptr) {
            memset(*type, 0, length + 1);
            memcpy_s(*type, length, message.c_str(), length);
        }    
    }

    enum class ConnectionType {
        UNKNOWN = 0,
        TCP,
        UDP
    };

    enum class CommunicationType {
        NULLCOMMUNICATION = 0,
        ERRORCOMMUNICATION,
        REGISTER,
        /*
          { "communication_type":"register"
          , "customer" : "aaa"
          , "password" : "******"} // 加密
        */
        REGISTERBACK,
        /*
          { "communication_type":"registerback"
          , "customer" : "aaa"
          , "register_result" : "succeed"
          , "id" : "516401043"}
        */
        LOGIN,
        /*
          { "communication_type":"login"
          , "customer" : "aaa"
          , "password" : "******"} // 加密
        */
        LOGINBACK,
        /*
          { "communication_type":"loginback"
          , "customer" : "aaa"
          , "login_result" : "succeed/failed"
          , "description" : "password error"}
        */
        SHOWLOGIN,
        /*
          { "communication_type":"showlogin"
          , "customer" : "aaa|bbb|ccc"
          . "customer_num" : 3}
        */
        DELETECUSTOMER,
        /*
          { "communication_type":"deletecustomer"
          , "customer" : "aaa"
          , "source" : "aaa"}
        */
        CHAT,
        /*
          { "communication_type":"chat"
          , "source" : "aaa"
          , "target" : "bbb"
          , "content" : "hello"}
        */
        TRANSFERFILEREQUEST,
        /*
          { "communication_type":"transferfilerequest"
          , "source" : "bbb"
          , "target" : "aaa"
          , "file_name" : "a.cpp"
          , "file_length" : 10}
        */
        TRANSFERFILERESPOND,
        /*
          { "communication_type":"transferfilerespond"
          , "source" : "aaa"
          , "target" : "bbb"
          , "file_name" : "a.cpp"
          , "transfer_result" : "agree/refuse"}
        */
        TRANSFERFILE,
        /*
          { "communication_type":"transferfile"
          , "source" : "bbb"
          , "target" : "aaa"
          , "file_name" : "a.cpp"
          , "file_length" : 10
          , "file_content" : "1325464524"
          , "file_block" : 1
          , "current_block" : 1}
        */
        FORCEDELETE,
        /*
          { "communication_type":"forcedelete"
          , "customer" : "bbb"}
        */
    };

    struct s_HandleRecv {
        s_HandleRecv() {
            type = CommunicationType::NULLCOMMUNICATION;
            connectionType = ConnectionType::UNKNOWN;
        }
        s_HandleRecv(const s_HandleRecv& other) {
            connectionType = other.connectionType;
            ip = other.ip;
            socket = other.socket;
        }
        s_HandleRecv operator=(const s_HandleRecv& other) {
            connectionType = other.connectionType;
            ip = other.ip;
            socket = other.socket;
            return *this;
        }
        CommunicationType type;
        ConnectionType connectionType;
        std::string ip;
        union Socket {
            SOCKET socket_accept_tcp;
            SOCKADDR_IN addr_client_udp;
        }socket;
        union Param {
            // 注册用户
            struct RegisterType {
                char* customer;
                char* password;
            };
            // 注册用户应答
            struct RegisterBackType {
                char* customer;
                char* register_result;
                long long id;
            };
            // 用户登录
            struct LoginType {
                char* customer;
                char* password;
            };
            // 用户登录应答
            struct LoginBackType {
                char* customer;
                char* login_result;
                char* description;
            };
            // 显示用户
            struct ShowLoginType {
                char** customer;
                unsigned int customer_num;
            };
            // 用户退出
            struct DelCustomerType {
                char* customer;
                char* source;
            };
            // 聊天
            struct ChatType {
                char* source;
                char* target;
                char* content;
            };
            // 传送文件请求
            struct TransferFileRequestType {
                char* source;
                char* target;
                char* file_name;
                char* file_length;
            };
            // 传送文件应答
            struct TransferFileRespondType {
                char* source;
                char* target;
                char* transfer_result;
                char* file_name;
            };
            // 传送文件
            struct TransferFileType {
                char* source;
                char* target;
                char* file_name;
                char* file_length;
                char* file_content;
                unsigned int file_block;
                unsigned int current_block;
            };
            struct ForceDeleteType {
                char* customer;
            };
            RegisterType Register;
            RegisterBackType RegisterBack;
            LoginType Login;
            LoginBackType LoginBack;
            ShowLoginType ShowLogin;
            DelCustomerType DelCustomer;
            ChatType Chat;
            TransferFileRequestType TransferFileRequest;
            TransferFileRespondType TransferFileRespond;
            TransferFileType TransferFile;
            ForceDeleteType ForceDelete;
        } param;
    };

    struct s_TaskQueue {
        s_TaskQueue(const s_HandleRecv& handleRecv, const std::string message) {
            this->handleRecv = handleRecv;
            this->message = message;
        }
        s_HandleRecv handleRecv;
        std::string message;
    };

    std::string EncodeJson(const CommunicationType type, const s_HandleRecv& s_param);

    bool DecodeJson(const std::string& message, s_HandleRecv& s_param);

    void DeleteMemory(const CommunicationType type, s_HandleRecv& s_param);

    std::string Encryption(const std::string& param);

    std::string Decryption(const std::string& param);

    bool VerifyCode(const std::string& code, const std::string& code_verify);

    void ToLow(std::string& code);

    std::string CombineString(char** be_combined, const int size);

    void SplitString(const std::string& be_converted, const char separator, std::vector<std::string>& dest);

    void SplitString(const char* be_converted, const char separator, char** dest, int& size);

    class CHandleJson {
    public:
        CHandleJson(const std::string& message);

        ~CHandleJson() {};

    public:
        void InitJson(const std::string& message);

        void ClearJson();

        std::string GetJsonValueString(const std::string& key) const;

        long long GetJsonValueNum(const std::string& key) const;

        void SetJsonValue(const std::string& key, const std::string& value);

        void SetJsonValue(const std::string& key, const long long value);

        std::string GetJsonBack();

    private:
        CHandleJson(const CHandleJson&) = delete;
        CHandleJson operator=(const CHandleJson&) = delete;

    private:
        std::string strJsonRead;
        std::string strJsonWrite;
    };

    class CHandleIni {
    public:
        CHandleIni(const std::string &file_path);

        ~CHandleIni() {};

    public:
        std::string GetValue(const std::string& key) const;

    private:
        CHandleIni(const CHandleIni&) = delete;
        CHandleIni operator=(const CHandleIni&) = delete;

    private:
        std::unordered_map<std::string, std::string> keyToValue;
    };
}
#endif  //__PUBLIC_H__