#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include "../json/json.h"
#include <windows.h>
#include <vector>

namespace mywork {
    enum class ReturnType {
        C_OK,
        C_SOCKET_FAILED,
        C_DATABASE_FAILED,

    };

    enum class ConnectionType {
        TCP,
        UDP
    };

    const int DATA_LENGTH = 1024 * 10;

    const int SOCKET_PORT = 6000;

    inline void RequestMemory(char** type, const unsigned int length, const std::string& message) {
        *type = new char[length + 1];
        memset(*type, 0, length + 1);
        memcpy_s(*type, length, message.c_str(), length);
    }

    enum class CommunicationType {
        NULLCOMMUNICATION = 0,
        ERRORCOMMUNICATION,
        REGISTER,
        /*
          { "communicationType":"register"
          , "customer" : "aaa"
          , "password" : "******"} // ����
        */
        REGISTERBACKSUCCEED,
        /*
          { "communicationType":"registerbacksucceed"
          , "customer" : "aaa"
          , "registerResult" : "succeed"}
        */
        REGISTERBACKFAILED,
        /*
          { "communicationType":"registerbackfailed"
          , "customer" : "aaa"
          , "registerResult" : "failed"
          , "description" : "user name is already have"}
        */
        LOGIN,
        /*
          { "communicationType":"login"
          , "customer" : "aaa"
          , "password" : "******" // ����
          , "ip" : "127.0.0.1"} 
        */
        LOGINBACKSUCCEED,
        /*
          { "communicationType":"loginbacksucceed"
          , "customer" : "aaa"
          , "loginResult" : "succeed"}
        */
        LOGINBACKFAILED,
        /*
          { "communicationType":"loginbackfailed"
          , "customer" : "aaa"
          , "loginResult" : "failed"
          , "description" : "password error"}
        */
        SHOWLOGIN,
        /*
          { "communicationType":"showlogin"
          , "customer" : "aaa|bbb|ccc"
          . "customerNum" : 3}
        */
        DELETECUSTOMER,
        /*
          { "communicationType":"deletecustomer"
          , "customer" : "aaa"}
        */
        CHAT,
        /*
          { "communicationType":"chat"
          , "source" : "aaa"
          , "target" : "bbb"
          , "content" : "hello"}
        */
        TRANSFERFILEREQUEST,
        /*
          { "communicationType":"transferfilerequest"
          , "source" : "bbb"
          , "target" : "aaa"
          , "fileName" : "a.cpp"
          , "fileLength" : 10}
        */
        TRANSFERFILERESPOND,
        /*
          { "communicationType":"transferfilerespond"
          , "source" : "aaa"
          , "target" : "bbb"
          , "fileName" : "a.cpp"
          , "transferResult" : "agree/refuse"}
        */
        TRANSFERFILE,
        /*
          { "communicationType":"transferfile"
          , "source" : "bbb"
          , "target" : "aaa"
          , "fileName" : "a.cpp"
          , "fileLength" : 10
          , "fileContent" : "1325464524"
          , "fileBlock" : 1
          , "currentBlock" : 1}
        */
        FORCEDELETE,
        /*
          { "communicationType":"forcedelete"
          , "customer" : "bbb"}
        */
    };

    struct HandleRecv {
        CommunicationType communicationType;
        SOCKET socketAccept;
        union Param {
            // ע���û�
            struct RegisterType {
                char* customer;
                char* password;
            };
            // ע���û�Ӧ��
            struct RegisterBackType {
                char* customer;
                char* registerResult;
                char* description;
            };
            // �û���¼
            struct LoginType {
                char* customer;
                char* password;
                char* ip;
            };
            // �û���¼Ӧ��
            struct LoginBackType {
                char* customer;
                char* loginResult;
                char* description;
            };
            // ��ʾ�û�
            struct ShowLoginType {
                char** customer;
                unsigned int customerNum;
            };
            // �û��˳�
            struct DelCustomerType {
                char* customer;
            };
            // ����
            struct ChatType {
                char* source;
                char* target;
                char* content;
            };
            // �����ļ�����
            struct TransferFileRequestType {
                char* source;
                char* target;
                char* fileName;
                char* fileLength;
            };
            // �����ļ�Ӧ��
            struct TransferFileRespondType {
                char* source;
                char* target;
                char* transferResult;
                char* fileName;
            };
            // �����ļ�
            struct TransferFileType {
                char* source;
                char* target;
                char* fileName;
                char* fileLength;
                char* fileContent;
                unsigned int fileBlock;
                unsigned int currentBlock;
            };
            // ����
            struct ForceDeleteType {
                char* customer;
            };
            RegisterType registerIn;
            RegisterBackType registerBack;
            LoginType login;
            LoginBackType loginBack;
            ShowLoginType showLogin;
            DelCustomerType delCustomer;
            ChatType chat;
            TransferFileRequestType transferFileRequest;
            TransferFileRespondType transferFileRespond;
            TransferFileType transferFile;
            ForceDeleteType forceDelete;
        } param;
    };

    std::string EncodeJson(const CommunicationType type, const HandleRecv& handleRecv);

    bool DecodeJson(const std::string& value, HandleRecv& handleRecv);

    std::string Encryption(const std::string& param);

    std::string Decryption(const std::string& param);

    bool VerifyCode(const std::string& code, const std::string& codeVerify);

    void ToLow(std::string& code);

    std::string CombineString(char** beCombined, const int size);

    void SplitString(const std::string& beConverted, const char separator, std::vector<std::string>& dest);

    void SplitString(const char* beConverted, const char separator, char** dest, int& size);
}

#endif  //__PUBLIC_H__
