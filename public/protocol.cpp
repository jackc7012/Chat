#include "protocol.h"

void UnregisterSpace(CommunicationType type, s_HandleRecv& field)
{
    try
    {
        switch (type)
        {
        case CommunicationType::REGISTER:
        {
            DeleteSpace(&field.Param.register_.customer);
            DeleteSpace(&field.Param.register_.password);
            DeleteSpace(&field.Param.register_.mac);
            DeleteSpace(&field.Param.register_.ip);
        }
        break;

        case CommunicationType::REGISTERBACKSUCCEED:
        {
            DeleteSpace(&field.Param.registerBack_.customer);
            DeleteSpace(&field.Param.registerBack_.register_result);
        }
        break;

        case CommunicationType::REGISTERBACKFAILED:
        {
            DeleteSpace(&field.Param.registerBack_.customer);
            DeleteSpace(&field.Param.registerBack_.register_result);
            DeleteSpace(&field.Param.registerBack_.description);
        }
        break;

        case CommunicationType::LOGIN:
        {
            DeleteSpace(&field.Param.login_.password);
        }
        break;

        case CommunicationType::LOGINBACKSUCCEED:
        {
            DeleteSpace(&field.Param.loginBack_.customer);
            DeleteSpace(&field.Param.loginBack_.login_result);
        }
        break;

        case CommunicationType::LOGINBACKFAILED:
        {
            DeleteSpace(&field.Param.loginBack_.login_result);
            DeleteSpace(&field.Param.loginBack_.description);
        }
        break;

        case CommunicationType::SHOWLOGIN:
        {
            DeleteSpace(&field.Param.showLogin_.customer);
        }
        break;

        case CommunicationType::DELETECUSTOMER:
        {
        }
        break;

        case CommunicationType::CHAT:
        {
            DeleteSpace(&field.Param.chat_.content);
        }
        break;

        case CommunicationType::TRANSFERFILEREQUEST:
        {
        }
        break;

        case CommunicationType::TRANSFERFILERESPOND:
        {
        }
        break;

        case CommunicationType::TRANSFERFILE:
        {
        }
        break;

        default:
            break;
        }
    } catch (...)
    {
        return;
    }
}

std::string EncodeJson(const CommunicationType type, const s_HandleRecv& s_param)
{
    Json::Value js_value;
    Json::StreamWriterBuilder js_write_builder;
    Json::StreamWriter* js_write(js_write_builder.newStreamWriter());
    std::stringstream str_json("");
    try
    {
        switch (type)
        {
        case CommunicationType::REGISTER:
        {
            js_value["communication_type"] = "register";
            js_value["customer"] = s_param.Param.register_.customer;
            js_value["password"] = s_param.Param.register_.password;
            js_value["mac"] = s_param.Param.register_.mac;
            js_value["ip"] = s_param.Param.register_.ip;
        }
        break;

        case CommunicationType::REGISTERBACKSUCCEED:
        {
            js_value["communication_type"] = "registerbacksucceed";
            js_value["customer"] = s_param.Param.registerBack_.customer;
            js_value["id"] = std::to_string(s_param.Param.registerBack_.id);
            js_value["register_result"] = s_param.Param.registerBack_.register_result;
        }
        break;

        case CommunicationType::REGISTERBACKFAILED:
        {
            js_value["communication_type"] = "registerbackfailed";
            js_value["customer"] = s_param.Param.registerBack_.customer;
            js_value["register_result"] = s_param.Param.registerBack_.register_result;
            js_value["description"] = s_param.Param.registerBack_.description;
        }
        break;

        case CommunicationType::LOGIN:
        {
            js_value["communication_type"] = "login";
            js_value["id"] = std::to_string(s_param.Param.login_.id);
            js_value["password"] = s_param.Param.login_.password;
        }
        break;

        case CommunicationType::LOGINBACKSUCCEED:
        {
            js_value["communication_type"] = "loginbacksucceed";
            js_value["customer"] = s_param.Param.loginBack_.customer;
            js_value["login_result"] = s_param.Param.loginBack_.login_result;
        }
        break;

        case CommunicationType::LOGINBACKFAILED:
        {
            js_value["communication_type"] = "loginbackfailed";
            js_value["login_result"] = s_param.Param.loginBack_.login_result;
            js_value["description"] = s_param.Param.loginBack_.description;
        }
        break;

        case CommunicationType::SHOWLOGIN:
        {
            js_value["communication_type"] = "showlogin";
            js_value["customer"] = s_param.Param.showLogin_.customer;
            js_value["show_login_type"] = std::to_string(s_param.Param.showLogin_.show_login_type);
        }
        break;

        case CommunicationType::DELETECUSTOMER:
        {
            js_value["communication_type"] = "deletecustomer";
            js_value["id"] = std::to_string(s_param.Param.delCustomer_.id);
        }
        break;

        case CommunicationType::CHAT:
        {
            js_value["communication_type"] = "chat";
            js_value["content"] = s_param.Param.chat_.content;
            js_value["sourceid"] = std::to_string(s_param.Param.chat_.sourceid);
            js_value["targetid"] = std::to_string(s_param.Param.chat_.targetid);
        }
        break;

        case CommunicationType::TRANSFERFILEREQUEST:
        {
            js_value["communication_type"] = "transferfilerequest";
            js_value["file_name"] = s_param.Param.transferFileRequest_.file_name;
            js_value["file_length"] = s_param.Param.transferFile_.file_length;
            js_value["target"] = s_param.Param.transferFileRequest_.target;
            js_value["source"] = s_param.Param.transferFileRequest_.source;
        }
        break;

        case CommunicationType::TRANSFERFILERESPOND:
        {
            js_value["communication_type"] = "transferfilerespond";
            js_value["file_name"] = s_param.Param.transferFileRespond_.file_name;
            js_value["target"] = s_param.Param.transferFileRespond_.target;
            js_value["source"] = s_param.Param.transferFileRespond_.source;
            js_value["transfer_result"] = s_param.Param.transferFileRespond_.transfer_result;
        }
        break;

        case CommunicationType::TRANSFERFILE:
        {
            js_value["communication_type"] = "transferfile";
            js_value["file_name"] = s_param.Param.transferFile_.file_name;
            js_value["file_length"] = s_param.Param.transferFile_.file_length;
            js_value["file_content"] = s_param.Param.transferFile_.file_content;
            js_value["file_block"] = s_param.Param.transferFile_.file_block;
            js_value["current_block"] = s_param.Param.transferFile_.current_block;
            js_value["target"] = s_param.Param.transferFile_.target;
            js_value["source"] = s_param.Param.transferFile_.source;
        }
        break;

        default:
            break;
        }
    } catch (...)
    {
        return "";
    }

    js_write->write(js_value, &str_json);

    return str_json.str();
}

bool DecodeJson(const std::string& value, s_HandleRecv& s_return)
{
    Json::Value js_value;
    JSONCPP_STRING err;
    Json::CharReaderBuilder js_reader_build;
    Json::CharReader* js_reader(js_reader_build.newCharReader());
    js_reader->parse(&value[0], &value[0] + value.length(), &js_value, &err);
    if (!js_value.isMember("communication_type"))
    {
        return false;
    }

    if (js_value["communication_type"].asString() == "register")
    { // register
        std::string customer = js_value["customer"].asString(), password = js_value["password"].asString(),
            mac = js_value["mac"].asString(), ip = js_value["ip"].asString();
        RegisterSpace(&s_return.Param.register_.customer, customer.c_str());
        RegisterSpace(&s_return.Param.register_.password, password.c_str());
        RegisterSpace(&s_return.Param.register_.mac, mac.c_str());
        RegisterSpace(&s_return.Param.register_.ip, ip.c_str());
        s_return.type_ = CommunicationType::REGISTER;

    }
    else if (js_value["communication_type"].asString() == "registerbacksucceed")
    { // register_succeed
        std::string customer = js_value["customer"].asString(), register_result = js_value["register_result"].asString();
        s_return.type_ = CommunicationType::REGISTERBACKSUCCEED;

    }
    else if (js_value["communication_type"].asString() == "registerbackfailed")
    { // register_failed
        std::string customer = js_value["customer"].asString(), register_result = js_value["register_result"].asString(),
            description = js_value["description"].asString();
        s_return.type_ = CommunicationType::REGISTERBACKFAILED;

    }
    else if (js_value["communication_type"].asString() == "login")
    { // login
        std::string password = js_value["password"].asString();
        RegisterSpace(&s_return.Param.login_.password, password.c_str());
        s_return.Param.login_.id = atoll(js_value["id"].asString().c_str());
        s_return.type_ = CommunicationType::LOGIN;

    }
    else if (js_value["communication_type"].asString() == "loginbacksucceed")
    { // login_succeed
        std::string customer = js_value["customer"].asString(), login_result = js_value["login_result"].asString();
        s_return.type_ = CommunicationType::LOGINBACKSUCCEED;

    }
    else if (js_value["communication_type"].asString() == "loginbackfailed")
    { // login_faileds
        std::string customer = js_value["customer"].asString(), login_result = js_value["login_result"].asString(),
            description = js_value["description"].asString();
        s_return.type_ = CommunicationType::LOGINBACKFAILED;

    }
    else if (js_value["communication_type"].asString() == "showlogin")
    { // show_login
        
    }
    else if (js_value["communication_type"].asString() == "delcustomer")
    { // delete_customer
        std::string customer = js_value["customer"].asString();
        s_return.Param.delCustomer_.id = atoll(js_value["id"].asString().c_str());
        s_return.type_ = CommunicationType::DELETECUSTOMER;

    }
    else if (js_value["communication_type"].asString() == "chat")
    { // chat
        std::string content = js_value["content"].asString();
        RegisterSpace(&s_return.Param.chat_.content, content.c_str());
        s_return.Param.chat_.sourceid = atoll(js_value["sourceid"].asString().c_str());
        s_return.Param.chat_.targetid = atoll(js_value["targetid"].asString().c_str());
        s_return.type_ = CommunicationType::CHAT;

    }
    else if (js_value["communication_type"].asString() == "transferfilerequest")
    { // transfer_filequest
        std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
            target = js_value["target"].asString(), file_length = js_value["file_length"].asString();
        s_return.type_ = CommunicationType::TRANSFERFILEREQUEST;

    }
    else if (js_value["communication_type"].asString() == "transferfilerespond")
    { // transfer_filerespond
        std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
            target = js_value["target"].asString(), transfer_result = js_value["transfer_result"].asString();
        s_return.type_ = CommunicationType::TRANSFERFILERESPOND;

    }
    else if (js_value["communication_type"].asString() == "transferfile")
    { // transfer_file
        std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
            target = js_value["target"].asString(), file_content = js_value["file_content"].asString(),
            file_length = js_value["file_length"].asString();
        s_return.type_ = CommunicationType::TRANSFERFILE;

    }
    else
    { // other
        s_return.type_ = CommunicationType::NULLCOMMUNICATION;
    }
    return true;
}