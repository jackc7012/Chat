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
            DeleteSpace(&field.Param.loginBack_.customer);
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
            DeleteSpace(&field.Param.chat_.chat_time);
        }
        break;

        case CommunicationType::TRANSFERFILEINFO:
        {
            DeleteSpace(&field.Param.transferFileInfo_.file_name);
            DeleteSpace(&field.Param.transferFileInfo_.transfer_time);
        }
        break;

        case CommunicationType::TRANSFERFILECONTENT:
        {
            DeleteSpace(&field.Param.transferFileContent_.file_content);
        }
        break;

        case CommunicationType::FORCEDELETE:
        {
        }
        break;

        case CommunicationType::CHANGEPASSWORD:
        {
            DeleteSpace(&field.Param.changePassword_.old_password);
            DeleteSpace(&field.Param.changePassword_.password);
        }
        break;

        case CommunicationType::CHANGEPASSWORDBACK:
        {
            DeleteSpace(&field.Param.changePasswordBack_.update_result);
        }
        break;

        default:
            break;
        }
    } catch (...)
    {
        return;
    }

    return;
}

std::string EncodeJson(const CommunicationType type, const s_HandleRecv& s_param)
{
    cwy::Json js_value;

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
            js_value["customer"] = s_param.Param.loginBack_.customer;
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
            js_value["communication_type"] = "delcustomer";
            js_value["id"] = std::to_string(s_param.Param.delCustomer_.id);
        }
        break;

        case CommunicationType::CHAT:
        {
            js_value["communication_type"] = "chat";
            js_value["sourceid"] = std::to_string(s_param.Param.chat_.sourceid);
            js_value["targetid"] = std::to_string(s_param.Param.chat_.targetid);
            js_value["content"] = s_param.Param.chat_.content;
            js_value["time"] = s_param.Param.chat_.chat_time;
        }
        break;

        case CommunicationType::TRANSFERFILEINFO:
        {
            js_value["communication_type"] = "transferfileinfo";
            js_value["file_name"] = s_param.Param.transferFileInfo_.file_name;
            js_value["file_length"] = std::to_string(s_param.Param.transferFileInfo_.file_length);
            js_value["file_block"] = std::to_string(s_param.Param.transferFileInfo_.file_block);
            js_value["sourceid"] = std::to_string(s_param.Param.chat_.sourceid);
            js_value["targetid"] = std::to_string(s_param.Param.chat_.targetid);
            js_value["transfer_time"] = s_param.Param.transferFileInfo_.transfer_time;
        }
        break;

        case CommunicationType::TRANSFERFILECONTENT:
        {
            js_value["communication_type"] = "transferfilecontent";
            js_value["file_content"] = s_param.Param.transferFileContent_.file_content;
            js_value["now_block"] = std::to_string(s_param.Param.transferFileContent_.now_block);
        }
        break;

        case CommunicationType::FORCEDELETE:
        {
            js_value["communication_type"] = "forcedelete";
            js_value["id"] = std::to_string(s_param.Param.forceDelete_.id);
        }
        break;

        case CommunicationType::CHANGEPASSWORD:
        {
            js_value["communication_type"] = "changepassword";
            js_value["id"] = std::to_string(s_param.Param.changePassword_.id);
            js_value["old_password"] = s_param.Param.changePassword_.old_password;
            js_value["password"] = s_param.Param.changePassword_.password;
        }
        break;

        default:
            break;
        }
    } catch (...)
    {
        return "";
    }

    js_value.Write(str_json);

    return str_json.str();
}

bool DecodeJson(const std::string& value, s_HandleRecv& s_return)
{
    cwy::Json js_value;
    if (!js_value.Parse(value))
    {
        return false;
    }

    RegisterType a;
    s_return = (RegisterType*)&s_return;
    if (js_value["communication_type"] == "register")
    { // register
        ((RegisterType*)s_return)->customer_;
        s_retur->customer_
        std::string customer = js_value["customer"], password = js_value["password"], ip = js_value["ip"];
        RegisterSpace(&s_return.Param.register_.customer, customer);
        RegisterSpace(&s_return.Param.register_.password, password);
        s_return.type_ = CommunicationType::REGISTER;
    }

    else if (js_value["communication_type"] == "registerbacksucceed")
    { // register_succeed
        std::string customer = js_value["customer"], register_result = js_value["register_result"];
        RegisterSpace(&s_return.Param.registerBack_.customer, customer);
        s_return.Param.registerBack_.id = strtoull(js_value["id"].c_str(), nullptr, 10);
        RegisterSpace(&s_return.Param.registerBack_.register_result, register_result);
        s_return.type_ = CommunicationType::REGISTERBACKSUCCEED;
    }

    else if (js_value["communication_type"] == "registerbackfailed")
    { // register_failed
        std::string register_result = js_value["register_result"], description = js_value["description"];
        RegisterSpace(&s_return.Param.registerBack_.register_result, register_result);
        RegisterSpace(&s_return.Param.registerBack_.description, description);
        s_return.type_ = CommunicationType::REGISTERBACKFAILED;
    }

    else if (js_value["communication_type"] == "login")
    { // login
        std::string password = js_value["password"];
        RegisterSpace(&s_return.Param.login_.password, password);
        s_return.Param.login_.id = strtoull(js_value["id"].c_str(), nullptr, 10);
        s_return.type_ = CommunicationType::LOGIN;
    }

    else if (js_value["communication_type"] == "loginbacksucceed")
    { // login_succeed
        std::string customer = js_value["customer"], login_result = js_value["login_result"];
        RegisterSpace(&s_return.Param.loginBack_.customer, customer);
        RegisterSpace(&s_return.Param.loginBack_.login_result, login_result);
        s_return.type_ = CommunicationType::LOGINBACKSUCCEED;
    }

    else if (js_value["communication_type"] == "loginbackfailed")
    { // login_failed
        std::string customer = js_value["customer"], login_result = js_value["login_result"],
            description = js_value["description"];
        RegisterSpace(&s_return.Param.register_.customer, customer);
        RegisterSpace(&s_return.Param.loginBack_.login_result, login_result);
        RegisterSpace(&s_return.Param.loginBack_.description, description);
        s_return.type_ = CommunicationType::LOGINBACKFAILED;
    }

    else if (js_value["communication_type"] == "showlogin")
    { // show_login
        std::string customer = js_value["customer"];
        RegisterSpace(&s_return.Param.showLogin_.customer, customer);
        s_return.Param.showLogin_.show_login_type = atoi(js_value["show_login_type"].c_str());
        s_return.type_ = CommunicationType::SHOWLOGIN;
    }

    else if (js_value["communication_type"] == "delcustomer")
    { // delete_customer
        s_return.Param.delCustomer_.id = strtoull(js_value["id"].c_str(), nullptr, 10);
        s_return.type_ = CommunicationType::DELETECUSTOMER;
    }

    else if (js_value["communication_type"] == "forcedelete")
    { // force delete_customer
        s_return.Param.forceDelete_.id = strtoull(js_value["id"].c_str(), nullptr, 10);
        s_return.type_ = CommunicationType::FORCEDELETE;
    }

    else if (js_value["communication_type"] == "chat")
    { // chat
        std::string content = js_value["content"], chat_time = js_value["time"];
        s_return.Param.chat_.sourceid = strtoull(js_value["sourceid"].c_str(), nullptr, 10);
        s_return.Param.chat_.targetid = strtoull(js_value["targetid"].c_str(), nullptr, 10);
        RegisterSpace(&s_return.Param.chat_.content, content);
        RegisterSpace(&s_return.Param.chat_.chat_time, chat_time);
        s_return.type_ = CommunicationType::CHAT;
    }

    else if (js_value["communication_type"] == "changepassword")
    { // change_password
        std::string old_password = js_value["old_password"], password = js_value["password"];
        s_return.Param.changePassword_.id = strtoull(js_value["id"].c_str(), nullptr, 10);
        RegisterSpace(&s_return.Param.changePassword_.old_password, old_password);
        RegisterSpace(&s_return.Param.changePassword_.password, password);
        s_return.type_ = CommunicationType::CHANGEPASSWORD;
    }

    else if (js_value["communication_type"] == "changepasswordback")
    { // change_password_back
        std::string update_result = js_value["update_result"];
        s_return.Param.changePasswordBack_.id = strtoull(js_value["id"].c_str(), nullptr, 10);
        RegisterSpace(&s_return.Param.changePasswordBack_.update_result, update_result);
        s_return.type_ = CommunicationType::CHANGEPASSWORDBACK;
    }

    else if (js_value["communication_type"] == "transferfileinfo")
    { // transfer_file_info
        std::string file_name = js_value["file_name"], transfer_time = js_value["time"];
        s_return.Param.transferFileInfo_.sourceid = strtoull(js_value["sourceid"].c_str(), nullptr, 10);
        s_return.Param.transferFileInfo_.targetid = strtoull(js_value["targetid"].c_str(), nullptr, 10);
        s_return.Param.transferFileInfo_.file_length = strtoull(js_value["file_length"].c_str(), nullptr, 10);
        s_return.Param.transferFileInfo_.file_block = strtoull(js_value["file_block"].c_str(), nullptr, 10);
        RegisterSpace(&s_return.Param.transferFileInfo_.file_name, file_name);
        RegisterSpace(&s_return.Param.transferFileInfo_.transfer_time, transfer_time);
        s_return.type_ = CommunicationType::TRANSFERFILEINFO;
    }

    else if (js_value["communication_type"] == "transferfilecontent")
    { // transfer_file_content
        std::string file_content = js_value["file_content"];
        RegisterSpace(&s_return.Param.transferFileContent_.file_content, file_content);
        s_return.Param.transferFileContent_.now_block = strtoull(js_value["now_block"].c_str(), nullptr, 10);
        s_return.type_ = CommunicationType::TRANSFERFILECONTENT;
    }

    else
    { // other
        s_return.type_ = CommunicationType::NULLCOMMUNICATION;
        return false;
    }

    return true;
}
