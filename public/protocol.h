#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "public.h"

#include <Windows.h>

#include "json/json.h"

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
        , "password" : "******"
        , "mac" : "AAAAA-BBBBB-CCCCC-DDDDD"
        , "ip" : "192.168.0.1"} // ����
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
        , "password" : "******"} // ����
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
        , "password" : "******"} // ����
    */
    CHANGEPASSWORDBACK,
    /*
        { "communication_type":"changepasswordback"
        , "id" : 10000
        , "update_result" : "succeed"}
    */
};

struct s_HandleRecv {
    s_HandleRecv()
        : type_(CommunicationType::NULLCOMMUNICATION)
        , socket_accept_(0)
    {
        memset((void*)&Param, 0, sizeof(Param));
    }
    void Clear()
    {
        type_ = CommunicationType::NULLCOMMUNICATION;
        socket_accept_ = 0;
        memset((void*)&Param, 0, sizeof(Param));
    }
    CommunicationType type_;
    SOCKET socket_accept_;
    union param {
        // ע���û�
        struct RegisterType {
            char* customer;
            char* password;
        };
        // ע���û�Ӧ��
        struct RegisterBackType {
            char* customer;
            UINT64 id;
            char* register_result;
            char* description;
        };
        // �û���¼
        struct LoginType {
            UINT64 id;
            char* password;
        };
        // �û���¼Ӧ��
        struct LoginBackType {
            char* customer;
            char* login_result;
            char* description;
        };
        // ��ʾ�û�
        struct ShowLoginType {
            char* customer;
            int show_login_type;
        };
        // �û��˳�
        struct DelCustomerType {
            UINT64 id;
        };
        // ����
        struct ChatType {
            UINT64 sourceid;
            UINT64 targetid;
            char* content;
            char* chat_time;
        };
        // �����ļ���Ϣ
        struct TransferFileInfoType {
            UINT64 sourceid;
            UINT64 targetid;
            char* file_name;
            UINT64 file_length;
            UINT64 file_block;
            char* transfer_time;
        };
        // �����ļ�����
        struct TransferFileContentType {
            UINT64 now_block;
            char* file_content;
        };
        // ǿ������
        struct ForceDeleteType {
            UINT64 id;
        };
        // �û�����
        struct ChangePassordType {
            UINT64 id;
            char* old_password;
            char* password;
        };
        // �û�����Ӧ��
        struct ChangePassordBackType {
            UINT64 id;
            char* update_result;
        };
        RegisterType register_;
        RegisterBackType registerBack_;
        LoginType login_;
        LoginBackType loginBack_;
        ShowLoginType showLogin_;
        DelCustomerType delCustomer_;
        ChatType chat_;
        TransferFileInfoType transferFileInfo_;
        TransferFileContentType transferFileContent_;
        ForceDeleteType forceDelete_;
        ChangePassordType changePassword_;
        ChangePassordBackType changePasswordBack_;
    } Param;
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