#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <windows.h>
#include <vector>
#include <unordered_map>

#define WM_TO_MAIN_MESSAGE          (WM_USER + 1100)

namespace cwy {
    static const int DATA_LENGTH = 1024 * 10;

    static const std::string SERVER_IP = "192.168.1.3";

    static const std::string DATABASE_NAME = "mychat";

    static const char* VERIFY_CODE = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    static std::string client_nick_name;

    static std::string client_token;

    enum class HandlePwd {
        ENCRYPT = 1,
        DECRYPT
    };

    enum class ConnectionType {
        UNKNOWN = 0,
        TCP,
        UDP
    };

    enum class CommunicationType {
        NULLCOMMUNICATION = 0,
        ERRORCOMMUNICATION,
        TOKEN,
        /*
          { "communication_type":"token"
          , "token_length" : 16
          , "content" : "abcdefghijklmnop"}
        */
        TOKENBACK,
        /*
          { "communication_type":"tokenback"
          , "token_length" : 16
          , "content" : "abcdefghijklmnop"}
        */
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
          , "id" : 60001}
        */
        LOGIN,
        /*
          { "communication_type":"login"
          , "id" : 60001
          , "password" : "******"}
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
          , "source" : 60001
          , "target" : 60002
          , "content" : "hello"}
        */
        CHATRECV,
        /*
          { "communication_type":"chatrecv"
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
        MODIFYPASSWORD,
        /*
          { "communication_type":"modifypassword"
          , "old_password" : "***"
          , "new_password" : "***"}
        */
        MODIFYPASSWORDBACK
        /*
          { "communication_type":"modifypasswordback"
          , "modify_result" : "succeed/failed"
          , "description" : "modify success"}
        */
    };

    struct s_HandleRecv {
        s_HandleRecv() {
            type_ = CommunicationType::NULLCOMMUNICATION;
            connectionType_ = ConnectionType::UNKNOWN;
            socket_ = {0};
            param_ = {0};
        }
        s_HandleRecv(const s_HandleRecv& other) {
            connectionType_ = other.connectionType_;
            ip_ = other.ip_;
            token_ = other.token_;
            socket_ = other.socket_;
            param_ = {0};
        }
        s_HandleRecv operator=(const s_HandleRecv& other) {
            connectionType_ = other.connectionType_;
            ip_ = other.ip_;
            token_ = other.token_;
            socket_ = other.socket_;
            return *this;
        }
        CommunicationType type_;
        ConnectionType connectionType_;
        std::string ip_;
        std::string token_;
        union Socket {
            SOCKET socketAccept_;
        }socket_;
        union Param {
            // token
            struct TokenType {
                unsigned int tokenLength_;
                char* content_;
            };
            // token应答
            struct TokenBackType {
                unsigned int tokenLength_;
                char* content_;
            };
            // 注册用户
            struct RegisterType {
                char* customer_;
                char* password_;
            };
            // 注册用户应答
            struct RegisterBackType {
                char* customer_;
                char* registerResult_;
                unsigned long long id_;
            };
            // 用户登录
            struct LoginType {
                unsigned long long id_;
                char* password_;
            };
            // 用户登录应答
            struct LoginBackType {
                char* customer_;
                char* loginResult_;
                char* description_;
            };
            // 显示用户
            struct ShowLoginType {
                char** customer_;
                unsigned int customerNum_;
            };
            // 用户退出
            struct DelCustomerType {
                char* customer_;
                char* source_;
            };
            // 聊天
            struct ChatType {
                unsigned long long source_;
                unsigned long long target_;
                char* content_;
            };
            // 聊天信息
            struct ChatRecvType {
                char* content_;
            };
            // 传送文件请求
            struct TransferFileRequestType {
                char* source_;
                char* target_;
                char* fileName_;
                char* fileLength_;
            };
            // 传送文件应答
            struct TransferFileRespondType {
                char* source_;
                char* target_;
                char* transferResult_;
                char* fileName_;
            };
            // 传送文件
            struct TransferFileType {
                char* source_;
                char* target_;
                char* fileName_;
                char* fileLength_;
                char* fileContent_;
                unsigned int fileBlock_;
                unsigned int currentBlock_;
            };
            // 强制退出
            struct ForceDeleteType {
                char* customer_;
            };
            // 修改密码
            struct ModifyPasswordType {
                char* oldPassword_;
                char* newPassword_;
            };
            // 修改密码应答
            struct ModifyPasswordRespondType {
                char* modifyResult_;
                char* description_;
            };
            TokenType token_;
            TokenBackType tokenBack_;
            RegisterType register_;
            RegisterBackType registerBack_;
            LoginType login_;
            LoginBackType loginBack_;
            ShowLoginType showLogin_;
            DelCustomerType delCustomer_;
            ChatType chat_;
            ChatRecvType chatRecv_;
            TransferFileRequestType transferFileRequest_;
            TransferFileRespondType transferFileRespond_;
            TransferFileType transferFile_;
            ForceDeleteType forceDelete_;
            ModifyPasswordType modifyPassword_;
            ModifyPasswordRespondType modifyPasswordRespond_;
        } param_;
    };

    struct s_TaskQueue {
        s_TaskQueue(const s_HandleRecv& handleRecv, const std::string& message) {
            this->handleRecv_ = handleRecv;
            this->message_ = message;
        }
        s_HandleRecv handleRecv_;
        std::string message_;
    };

    inline void HANDLE_MESSAGE(char** type, const unsigned int length, const std::string& message) {
        *type = new char[length + 1];
        if (*type != nullptr) {
            memset(*type, 0, length + 1);
            memcpy_s(*type, length, message.c_str(), length);
        }
    }

    std::string EncodeJson(const CommunicationType type, const s_HandleRecv& lParam);

    bool DecodeJson(const std::string& message, s_HandleRecv& lParam);

    void DeleteMemory(const CommunicationType type, s_HandleRecv& lParam);

    void GetToken(std::string& token, int& tokenLength);

    std::string EncryOrDecryPwd(const HandlePwd type, const std::string& pwd, const std::string& token = "");

    bool VerifyCode(const std::string& code, const std::string& codeVerify);

    void ToLow(std::string& code);

    std::string CombineString(char** beCombined, const int size);

    void SplitString(const std::string& beConverted, const char separator, std::vector<std::string>& dest);

    void SplitString(const char* beConverted, const char separator, char** dest, int& size);

    class CHandleJson {
    public:
        CHandleJson(const std::string& message);

        ~CHandleJson() {};

    public:
        void InitJson(const std::string& message);

        void ClearJson();

        std::string GetJsonValueString(const std::string& key) const;

        long long GetJsonValueNum(const std::string& key) const;

        void SetJsonValue(const std::string& key, const char* value);

        void SetJsonValue(const std::string& key, const long long value);

        std::string GetJsonBack();

    private:
        CHandleJson(const CHandleJson&) = delete;
        CHandleJson operator=(const CHandleJson&) = delete;

    private:
        std::string strJsonRead_;
        std::string strJsonWrite_;
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
        std::unordered_map<std::string, std::string> keyToValue_;
    };
}
#endif  //__PUBLIC_H__