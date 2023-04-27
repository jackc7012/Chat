#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "public.h"

#include <Windows.h>

#include "../json/json.h"

enum class CommunicationType {
    NULLCOMMUNICATION = 0,
    ERRORCOMMUNICATION,
    REGISTER,
    /*
        { "communication_type":"register"
        , "customer" : "aaa"
        , "password" : "******"} // 加密
    */
    REGISTERBACKSUCCEED,
    /*
        { "communication_type":"registerbacksucceed"
        , "customer" : "aaa"
        , "id" : 60001
        , "register_result" : "succeed"}
    */
    REGISTERBACKFAILED,
    /*
        { "communication_type":"registerbackfailed"
        , "customer" : "aaa"
        , "register_result" : "failed"
        , "description" : "user name is already have"}
    */
    LOGIN,
    /*
        { "communication_type":"login"
        , "customer" : "aaa"
        , "password" : "******"} // 加密
    */
    LOGINBACKSUCCEED,
    /*
        { "communication_type":"loginbacksucceed"
        , "customer" : "aaa"
        , "login_result" : "succeed"}
    */
    LOGINBACKFAILED,
    /*
        { "communication_type":"loginbackfailed"
        , "customer" : "aaa"
        , "login_result" : "failed"
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
        , "customer" : "aaa"}
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
    s_HandleRecv()
        : type_(CommunicationType::NULLCOMMUNICATION)
        , socket_accept_(0)
    {
    }
    CommunicationType type_;
    SOCKET socket_accept_;
    union param {
        // 注册用户
        struct RegisterType {
            char* customer;
            char* password;
        };
        // 注册用户应答
        struct RegisterBackType {
            char* customer;
            unsigned long long id;
            char* register_result;
            char* description;
        };
        // 用户登录
        struct LoginType {
            unsigned long long id;
            char* password;
        };
        // 用户登录应答
        struct LoginBackType {
            unsigned long long id;
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
        RegisterType register_;
        RegisterBackType registerBack_;
        LoginType login_;
        LoginBackType loginBack_;
        ShowLoginType showLogin_;
        DelCustomerType delCustomer_;
        ChatType chat_;
        TransferFileRequestType transferFileRequest_;
        TransferFileRespondType transferFileRespond_;
        TransferFileType transferFile_;
        ForceDeleteType forceDelete_;
    } Param;
};

inline void RegisterSpace(char** field, const std::string& message)
{
    size_t len = message.length();
    *field = new char[len + 1];
    memset(*field, 0, len + 1);
    memcpy_s(*field, len, message.c_str(), len);
}

void UnregisterSpace(CommunicationType type, s_HandleRecv& field);

std::string EncodeJson(const CommunicationType type, const s_HandleRecv& s_param);

bool DecodeJson(const std::string& value, s_HandleRecv& s_return);

#endif // !PROTOCOL