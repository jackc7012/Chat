#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <WinSock2.h>
#include <vector>

#include "../json/json.h"

const unsigned int DATA_LENGTH = 1024 * 10;

enum class CommunicationType
{
    NULLCOMMUNICATION = 0,
    ERRORCOMMUNICATION,
    REGISTER,
    /*
      { "communication_type":"register"
      , "customer" : "aaa"
      , "password" : "******"} // ����
    */
    REGISTERBACKSUCCEED,
    /*
      { "communication_type":"registerbacksucceed"
      , "customer" : "aaa"
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

struct s_HandleRecv
{
    s_HandleRecv()
    : type_(CommunicationType::NULLCOMMUNICATION),
      socket_accept_(0)
    {
    }
    CommunicationType type_;
    SOCKET socket_accept_;
    union param {
        // ע���û�
        struct RegisterType {
            char *customer;
            char *password;
        };
        // ע���û�Ӧ��
        struct RegisterBackType {
            char *customer;
            char *register_result;
            char *description;
        };
        // �û���¼
        struct LoginType {
            char *customer;
            char *password;
        };
        // �û���¼Ӧ��
        struct LoginBackType {
            char *customer;
            char *login_result;
            char *description;
        };
        // ��ʾ�û�
        struct ShowLoginType {
            char **customer;
            unsigned int customer_num;
        };
        // �û��˳�
        struct DelCustomerType {
            char *customer;
        };
        // ����
        struct ChatType {
            char *source;
            char *target;
            char *content;
        };
        // �����ļ�����
        struct TransferFileRequestType {
            char *source;
            char *target;
            char *file_name;
            char *file_length;
        };
        // �����ļ�Ӧ��
        struct TransferFileRespondType {
            char *source;
            char *target;
            char *transfer_result;
            char *file_name;
        };
        // �����ļ�
        struct TransferFileType {
            char *source;
            char *target;
            char *file_name;
            char *file_length;
            char *file_content;
            unsigned int file_block;
            unsigned int current_block;
        };
        struct ForceDeleteType {
            char *customer;
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

std::string EncodeJson(const CommunicationType type, const s_HandleRecv &s_param);

bool DecodeJson(const std::string &value, s_HandleRecv &s_return);

std::string Encryption(const std::string &param);

std::string Decryption(const std::string &param);

bool VerifyCode(const std::string &code, const std::string &code_verify);

void ToLow(std::string &code);

std::string CombineString(char **be_combined, const int size);

void SplitString(const std::string &be_converted, const char separator, std::vector<std::string> &dest);

void SplitString(const char *be_converted, const char separator, char **dest, int &size);
#endif  //__PUBLIC_H__
