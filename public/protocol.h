#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "public.h"

#include <Windows.h>

#include "json.h"

enum class LoginResult {
    SUCCEED,
    NOUSER,
    ALREADYLOGININ,
    PASSWORDERROR,
    OTHER,
    UNKNOWNERROR,
    OUTLIMIT,
    NULLLOGIN
};

enum class CommunicationType {
    NULLCOMMUNICATION = 0,
    ERRORCOMMUNICATION,
    REGISTER,
    /*
        { "communication_type":"register"
        , "customer" : "aaa"
        , "password" : "******"} // º”√‹
    */
    REGISTERBACKSUCCEED,
    /*
        { "communication_type":"registerbacksucceed"
        , "customer" : "aaa"
        , "id" : 10000
        , "register_result" : "succeed"}
    */
    REGISTERBACKFAILED,
    /*
        { "communication_type":"registerbackfailed"
        , "customer" : "aaa"
        , "register_result" : "failed"
        , "description" : "unkown error"}
    */
    LOGIN,
    /*
        { "communication_type":"login"
        , "id" : 10000
        , "password" : "******"} // º”√‹
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
        , "customer" : "10000:aaa:1|10001:bbb:0|10002:ccc:0"
        . "show_login_type" : 0}
    */
    DELETECUSTOMER,
    /*
        { "communication_type":"deletecustomer"
        , "id" : 10000}
    */
    CHAT,
    /*
        { "communication_type":"chat"
        , "source" : 10000
        , "targetid" : 10001
        , "content" : "hello"
        , "time" : "2023-5-7 23:08:00"}
    */
    TRANSFERFILEINFO,
    /*
        { "communication_type":"transferfileupinfo"
        , "source" : "bbb"
        , "target" : "aaa"
        , "file_name" : "a.cpp"
        , "file_length" : 10
        , "file_block" : 1
        , "time" : "2023-5-7 23:08:00"}
    */
    TRANSFERFILECONTENT,
    /*
        { "communication_type":"transferfilecontent"
        , "file_content" : "abcdefghijklmn"
        , "file_length" : 1}
    */
    FORCEDELETE,
    /*
        { "communication_type":"forcedelete"
        , "id" : 10000}
    */
    CHANGEPASSWORD,
    /*
        { "communication_type":"changepassword"
        , "id" : 10000
        , "old_password" : "******"
        , "password" : "******"} // º”√‹
    */
    CHANGEPASSWORDBACK,
    /*
        { "communication_type":"changepasswordback"
        , "id" : 10000
        , "update_result" : "succeed"}
    */
};

class s_HandleRecv {
public:
    s_HandleRecv()
        : type_(CommunicationType::NULLCOMMUNICATION)
        , socketAccept_(0)
    {
    }
    void Clear()
    {
        type_ = CommunicationType::NULLCOMMUNICATION;
        socketAccept_ = 0;
    }
    CommunicationType type_;
    SOCKET socketAccept_;
};

class RegisterType : public s_HandleRecv {
public:
    std::string customer_;
    std::string password_;
};

class RegisterBackType : public s_HandleRecv {
public:
    std::string customer_;
    UINT64 id_;
    std::string registerResult_;
    std::string description_;
};

class LoginType : public s_HandleRecv {
public:
    UINT64 id_;
    std::string password_;
};

class LoginBackType : public s_HandleRecv {
public:
    std::string customer_;
    std::string registerResult_;
    std::string description_;
};

class ShowLoginType : public s_HandleRecv {
public:
    std::string customers_;
    UINT8 showLoginType_;
};

class DelCustomerType : public s_HandleRecv {
public:
    UINT64 id_;
};

class ChatType : public s_HandleRecv {
public:
    UINT64 sourceId_;
    UINT64 targetId_;
    std::string content_;
    std::string time_;
};

inline void RegisterSpace(char** field, const std::string& message)
{
    size_t len = message.length();
    *field = new char[len + 1];
    memset(*field, 0, len + 1);
    memcpy_s(*field, len, message.c_str(), len);
}

inline void DeleteSpace(char** field)
{
    delete[] * field;
    *field = nullptr;
}

void UnregisterSpace(CommunicationType type, s_HandleRecv& field);

std::string EncodeJson(const CommunicationType type, const s_HandleRecv& s_param);

bool DecodeJson(const std::string& value, s_HandleRecv& s_return);

#endif // !PROTOCOL