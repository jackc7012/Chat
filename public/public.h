#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <WinSock2.h>
#include "CJson.h"

namespace cwy {
const unsigned int DATA_LENGTH = 1024 * 10;

const std::string VERIFY_CODE = "1234567890ABCDEFGHJKLMNPQRSTUVVWXYZabcdefghijkmnpqrstuvwxyz";

using IpAndSocket = std::pair<std::string, SOCKET>;
static std::unordered_map<std::string, IpAndSocket> nameToIpSock;

using StringMap = std::unordered_map<unsigned int, std::string>;
using IntMap = std::unordered_map<unsigned int, long long>;

struct File {
    File(const std::string& source_, const std::string& target_, const std::string& fileName_, const std::string fileContent_ = "",
        const unsigned int fileBlock_ = 0, const unsigned int currentBlock_ = 0)
        : source(source_), target(target_), fileName(fileName_), fileContent(fileContent_),
        fileBlock(fileBlock_), currentBlock(currentBlock_)
    {
    }
    std::string source;
    std::string target;
    std::string fileName;
    std::string fileContent;
    unsigned int fileBlock;
    unsigned int currentBlock;
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
    LOGINBOARDCAST,
    /*
      { "communication_type":"loginboardcast"
      , "customer" : "aaa"}
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
      , "fileName" : "a.cpp"
      , "file_length" : 10}
    */
    TRANSFERFILERESPOND,
    /*
      { "communication_type":"transferfilerespond"
      , "source" : "aaa"
      , "target" : "bbb"
      , "fileName" : "a.cpp"
      , "transfer_result" : "agree/refuse"}
    */
    TRANSFERFILE,
    /*
      { "communication_type":"transferfile"
      , "source" : "bbb"
      , "target" : "aaa"
      , "fileName" : "a.cpp"
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
        , connect_ip_("")
        , socket_accept_(0)
        , Param()
    {
    }
    CommunicationType type_;
    std::string connect_ip_;
    SOCKET socket_accept_;
    union param {
        // 注册用户
        struct RegisterType {
            char* customer;
            char* password;
        };
        // 注册用户应答
        struct RegisterBackType {
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
            char* customer;
            char* login_result;
            char* description;
        };
        // 用户登录/退出广播
        struct LoginBoardcastType {
            bool flag;
            char* customer;
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
            unsigned long long file_length;
        };
        // 传送文件应答
        struct TransferFileRespondType {
            char* source;
            char* target;
            char* file_name;
            char* transfer_result;
        };
        // 传送文件
        struct TransferFileType {
            char* source;
            char* target;
            char* file_name;
            char* file_content;
            unsigned int file_block;
            unsigned int current_block;
        };
        // 踢人
        struct ForceDeleteType {
            char* customer;
        };
        RegisterType register_;
        RegisterBackType registerBack_;
        LoginType login_;
        LoginBackType loginBack_;
        LoginBoardcastType loginBoardcast_;
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
    if (*field != nullptr) {
        delete[] * field;
        *field = nullptr;
    }
    size_t len = message.length();
    *field = new char[len + 1];
    memset(*field, 0, len + 1);
    memcpy_s(*field, len, message.c_str(), len);
}

void UnregisterSpace(CommunicationType type, s_HandleRecv& field);

inline void UnregisterSingleSpace(char** field)
{
    if (*field != nullptr) {
        delete[] * field;
        *field = nullptr;
    }
}

std::string EncodeJson(const CommunicationType type, const s_HandleRecv& s_param);

bool DecodeJson(const std::string& value, s_HandleRecv& s_return);

std::string Encryption(const std::string& param);

std::string Decryption(const std::string& param);

bool VerifyCode(const std::string& code, const std::string& code_verify);

bool VerifyPassWord(const char* password, const unsigned int len);

void ToLow(std::string& code);

std::string CombineString(char** be_combined, const int size);

void SplitString(const std::string& be_converted, const char separator, std::vector<std::string>& dest);

void SplitString(const char* be_converted, const char separator, char** dest, int& size);

std::string ToDbString(const std::string& src);

std::string DbJoin(const std::vector<long long>& srcList);

std::string DbJoin(const std::vector<std::string>& srcList);

std::string DbJoin(const StringMap& stringMap, const IntMap& intMap, bool hasBrackets = true);

bool CheckSqlValid(const std::string& src);

bool CheckValid(const std::string& src);

bool CheckJsonValid(const std::string& src);

inline std::string GetTime(const int type = 2)
{
    char temp[MAX_PATH] = {0};
    SYSTEMTIME st;
    GetLocalTime(&st);
    if (type == 0) {
        sprintf_s(temp, MAX_PATH, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    } else if (type == 1) {
        sprintf_s(temp, MAX_PATH, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
    } else if (type == 2) {
        sprintf_s(temp, MAX_PATH, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
    }
    return temp;
}
}
#endif  //__PUBLIC_H__
