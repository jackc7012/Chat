#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "public.h"

#include <Windows.h>

#include "json.h"

namespace cwy
{
    enum class CommunicationType
    {
        NULLCOMMUNICATION,
        ERRORCOMMUNICATION,
        /*
            { "communication_type":"register"
            , "customer" : "aaa"
            , "password" : "******"} // º”√‹
        */
        REGISTER,
        /*
            { "communication_type":"registerbacksucceed"
            , "customer" : "aaa"
            , "id" : "10000"}
        */
        REGISTERBACKSUCCEED,
        /*
            { "communication_type":"registerbackfailed"
            , "customer" : "aaa"
            , "description" : "unkown error"}
        */
        REGISTERBACKFAILED,
        /*
            { "communication_type":"login"
            , "id" : "10000"
            , "password" : "******"} // º”√‹
        */
        LOGIN,
        /*
            { "communication_type":"loginbacksucceed"
            , "id" : "10000"
            , "customer" : "aaa"}
        */
        LOGINBACKSUCCEED,
        /*
            { "communication_type":"loginbackfailed"
            , "id" : "10000"
            , "customer" : "aaa" *optional
            , "ip" : "192.168.0.1"  *optional
            , "description" : "password error"}
        */
        LOGINBACKFAILED,
        /*
            { "communication_type":"getandshow"
            , "id" : "10000"}
        */
        GETANDSHOW,
        /*
            { "communication_type":"showlogin"
            , "customer" : "10000:aaa:1;10001:bbb:0;10002:ccc:0"
            . "show_login_type" : 0}
        */
        SHOWLOGIN,
        /*
            { "communication_type":"delcustomer"
            , "id" : "10000"}
        */
        DELCUSTOMER,
        /*
            { "communication_type":"chat"
            , "sourceid" : "10000"
            , "targetid" : "10001"
            , "content" : "hello"
            , "time" : "2023-5-7 23:08:00"}
        */
        CHAT,
        /*
            { "communication_type":"transferfileinfo"
            , "sourceid" : "10000"
            , "targetid" : "10001"
            , "file_info" : "a.cpp_10_1"
            , "time" : "2023-5-7 23:08:00"}
        */
        TRANSFERFILEINFO,
        /*
            { "communication_type":"transferfilecontent"
            , "sourceid" : "10000"
            , "targetid" : "10001"
            , "file_content" : "abcdefghijklmn"
            , "now_block" : 0}
        */
        TRANSFERFILECONTENT,
        /*
            { "communication_type":"forcedelete"
            , "id" : "10000"}
        */
        FORCEDELETE,
        /*
            { "communication_type":"changepassword"
            , "id" : "10000"
            , "old_password" : "******"
            , "password" : "******"} // º”√‹
        */
        CHANGEPASSWORD,
        /*
            { "communication_type":"changepasswordback"
            , "id" : "10000"
            , "update_result" : "failed"
            , "description" : "old_password error"}
        */
        CHANGEPASSWORDBACK,
        /*
            { "communication_type":"initcustomerchat"
            , "requestid" : "10001"
            , "id" : "10000"}
        */
        INITCUSTOMERCHAT,
        /*
            { "communication_type":"initcustomerchatback"
            , "sourcecontent" : ""
            , "targetcontent" : "HelloWorld"
            , "id" : "10000"
            , "contentid" : "10001"}
        */
        INITCUSTOMERCHATBACK
    };

    static std::unordered_map<std::string, CommunicationType> typeStrToInt = {
        {"register", CommunicationType::REGISTER}, {"registerbacksucceed", CommunicationType::REGISTERBACKSUCCEED},
        {"registerbackfailed", CommunicationType::REGISTERBACKFAILED}, {"login", CommunicationType::LOGIN},
        {"loginbacksucceed", CommunicationType::LOGINBACKSUCCEED}, {"loginbackfailed", CommunicationType::LOGINBACKFAILED},
        {"getandshow", CommunicationType::GETANDSHOW}, {"showlogin", CommunicationType::SHOWLOGIN},
        {"delcustomer", CommunicationType::DELCUSTOMER}, {"chat", CommunicationType::CHAT},
        {"transferfileinfo", CommunicationType::TRANSFERFILEINFO}, {"transferfilecontent", CommunicationType::TRANSFERFILECONTENT},
        {"forcedelete", CommunicationType::FORCEDELETE}, {"changepassword", CommunicationType::CHANGEPASSWORD},
        {"changepasswordback", CommunicationType::CHANGEPASSWORDBACK}, {"initcustomerchat", CommunicationType::INITCUSTOMERCHAT},
        {"initcustomerchatback", CommunicationType::INITCUSTOMERCHATBACK}
    };

    static std::unordered_map<CommunicationType, std::string> typeIntToStr = {
        {CommunicationType::REGISTER, "register"}, {CommunicationType::REGISTERBACKSUCCEED, "registerbacksucceed"},
        {CommunicationType::REGISTERBACKFAILED, "registerbackfailed"}, {CommunicationType::LOGIN, "login"},
        {CommunicationType::LOGINBACKSUCCEED, "loginbacksucceed"}, {CommunicationType::LOGINBACKFAILED, "loginbackfailed"},
        {CommunicationType::GETANDSHOW, "getandshow"}, {CommunicationType::SHOWLOGIN, "showlogin"},
        {CommunicationType::DELCUSTOMER, "delcustomer"}, {CommunicationType::CHAT, "chat"},
        {CommunicationType::TRANSFERFILEINFO, "transferfileinfo"}, {CommunicationType::TRANSFERFILECONTENT, "transferfilecontent"},
        {CommunicationType::FORCEDELETE, "forcedelete"}, {CommunicationType::CHANGEPASSWORD, "changepassword"},
        {CommunicationType::CHANGEPASSWORDBACK, "changepasswordback"}, {CommunicationType::INITCUSTOMERCHAT, "initcustomerchat"},
        {CommunicationType::INITCUSTOMERCHATBACK, "initcustomerchatback"}
    };

    class HandleRecv
    {
    public:
        HandleRecv(SOCKET socketAccept, const std::string& str)
            : socketHandle_(socketAccept)
        {
            DecodeJson(str);
        }

        HandleRecv()
            : type_(CommunicationType::NULLCOMMUNICATION)
            , socketHandle_(0)
        {
        }

        void Clear()
        {
            type_ = CommunicationType::NULLCOMMUNICATION;
            jsonHandle_.Clear();
            socketHandle_ = 0;
        }

        void SetSocket(SOCKET socketHandle)
        {
            socketHandle_ = socketHandle;
        }

        inline const CommunicationType GetType() const
        {
            return type_;
        }

        inline const std::string GetContent(const std::string& key)
        {
            return jsonHandle_[key];
        }

        void SetContent(const std::string& key, const std::string& value)
        {
            jsonHandle_[key] = value;
        }

        std::string Write(CommunicationType type = CommunicationType::NULLCOMMUNICATION)
        {
            if (type != CommunicationType::NULLCOMMUNICATION)
            {
                jsonHandle_["communication_type"] = typeIntToStr[type];
            }
            return EncodeJson();
        }

    private:
        bool DecodeJson(const std::string& value);
        std::string EncodeJson();

    private:
        CommunicationType type_;
        SOCKET socketHandle_;
        Json jsonHandle_;
    };
}

#endif // !PROTOCOL