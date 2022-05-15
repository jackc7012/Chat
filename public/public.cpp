#include "public.h"
#include <sstream>

void UnregisterSpace(CommunicationType type, s_HandleRecv& field)
{
    try {
        switch (type) {
            case CommunicationType::REGISTER:
            {
                delete []field.Param.register_.customer;
                delete []field.Param.register_.password;
            }
            break;

            case CommunicationType::REGISTERBACKSUCCEED:
            {
                delete []field.Param.registerBack_.customer;
                delete []field.Param.registerBack_.register_result;
            }
            break;

            case CommunicationType::REGISTERBACKFAILED:
            {
                delete []field.Param.registerBack_.customer;
                delete []field.Param.registerBack_.register_result;
                delete []field.Param.registerBack_.description;
            }
            break;

            case CommunicationType::LOGIN:
            {
                delete []field.Param.login_.customer;
                delete []field.Param.login_.password;
            }
            break;

            case CommunicationType::LOGINBACKSUCCEED:
            {
                delete []field.Param.loginBack_.customer;
                delete []field.Param.loginBack_.login_result;
            }
            break;

            case CommunicationType::LOGINBACKFAILED:
            {
                delete []field.Param.loginBack_.customer;
                delete []field.Param.loginBack_.login_result;
                delete []field.Param.loginBack_.description;
            }
            break;

            case CommunicationType::SHOWLOGIN:
            {
            }
            break;

            case CommunicationType::DELETECUSTOMER:
            {
                delete []field.Param.delCustomer_.customer;
            }
            break;

            case CommunicationType::CHAT:
            {
                delete []field.Param.chat_.content;
                delete []field.Param.chat_.source;
                delete []field.Param.chat_.target;
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
    }
    catch (...) {
        return ;
    }
}

std::string EncodeJson(const CommunicationType type, const s_HandleRecv &s_param) {
    Json::Value js_value;
    Json::FastWriter js_write;
    std::string str_json("");
    try {
        switch (type) {
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
                js_value["customer"] = s_param.Param.login_.customer;
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
                std::string temp = CombineString(s_param.Param.showLogin_.customer, s_param.Param.showLogin_.customer_num);
                js_value["customer"] = temp.c_str();
                js_value["customer_num"] = s_param.Param.showLogin_.customer_num;
            }
            break;

            case CommunicationType::DELETECUSTOMER:
            {
                js_value["communication_type"] = "deletecustomer";
                js_value["customer"] = s_param.Param.delCustomer_.customer;
            }
            break;

            case CommunicationType::CHAT:
            {
                js_value["communication_type"] = "chat";
                js_value["content"] = s_param.Param.chat_.content;
                js_value["target"] = s_param.Param.chat_.target;
                js_value["source"] = s_param.Param.chat_.source;
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
    }
    catch (...) {
        return "";
    }

    str_json = js_write.write(js_value);

    return str_json;
}

bool DecodeJson(const std::string &value, s_HandleRecv &s_return)
{
    try {
        Json::Value js_value;
        Json::Reader js_reader;
        if (!CheckJsonValid(value)) {
            return false;
        }
        bool flag = js_reader.parse(value, js_value);
        if (flag == false || js_value.isMember("communication_type") == false) {
            return false;
        }

        if (js_value["communication_type"].asString() == "register") { // register
            std::string customer = js_value["customer"].asString(), password = js_value["password"].asString();
            RegisterSpace(&s_return.Param.register_.customer, customer.c_str());
            RegisterSpace(&s_return.Param.register_.password, password.c_str());
            s_return.type_ = CommunicationType::REGISTER;

        } else if (js_value["communication_type"].asString() == "registerbacksucceed") { // register_succeed
            std::string customer = js_value["customer"].asString(), register_result = js_value["register_result"].asString();
            s_return.type_ = CommunicationType::REGISTERBACKSUCCEED;

        } else if (js_value["communication_type"].asString() == "registerbackfailed") { // register_failed
            std::string customer = js_value["customer"].asString(), register_result = js_value["register_result"].asString(),
                description = js_value["description"].asString();
            s_return.type_ = CommunicationType::REGISTERBACKFAILED;

        } else if (js_value["communication_type"].asString() == "login") { // login
            std::string customer = js_value["customer"].asString(), password = js_value["password"].asString();
            RegisterSpace(&s_return.Param.login_.customer, customer.c_str());
            RegisterSpace(&s_return.Param.login_.password, password.c_str());
            s_return.type_ = CommunicationType::LOGIN;

        } else if (js_value["communication_type"].asString() == "loginbacksucceed") { // login_succeed
            std::string customer = js_value["customer"].asString(), login_result = js_value["login_result"].asString();
            s_return.type_ = CommunicationType::LOGINBACKSUCCEED;

        } else if (js_value["communication_type"].asString() == "loginbackfailed") { // login_faileds
            std::string customer = js_value["customer"].asString(), login_result = js_value["login_result"].asString(),
                description = js_value["description"].asString();
            s_return.type_ = CommunicationType::LOGINBACKFAILED;

        } else if (js_value["communication_type"].asString() == "showlogin") { // show_login
            std::string customer = js_value["customer"].asString();
            int size = 0;
            if (size != js_value["customer_num"].asInt()) {
                s_return.type_ = CommunicationType::ERRORCOMMUNICATION;
                for (int i = 0; i < size; ++i) {
                    delete[]s_return.Param.showLogin_.customer[i];
                }
                delete[]s_return.Param.showLogin_.customer;
            } else {
                s_return.type_ = CommunicationType::SHOWLOGIN;
            }

        } else if (js_value["communication_type"].asString() == "delcustomer") { // delete_customer
            std::string customer = js_value["customer"].asString();
            RegisterSpace(&s_return.Param.delCustomer_.customer, customer.c_str());
            s_return.type_ = CommunicationType::DELETECUSTOMER;

        } else if (js_value["communication_type"].asString() == "chat") { // chat
            std::string content = js_value["content"].asString(), source = js_value["source"].asString(),
                target = js_value["target"].asString();
            RegisterSpace(&s_return.Param.chat_.content, content.c_str());
            RegisterSpace(&s_return.Param.chat_.source, source.c_str());
            RegisterSpace(&s_return.Param.chat_.target, target.c_str());
            s_return.type_ = CommunicationType::CHAT;

        } else if (js_value["communication_type"].asString() == "transferfilerequest") { // transfer_filequest
            std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
                target = js_value["target"].asString(), file_length = js_value["file_length"].asString();
            s_return.type_ = CommunicationType::TRANSFERFILEREQUEST;

        } else if (js_value["communication_type"].asString() == "transferfilerespond") { // transfer_filerespond
            std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
                target = js_value["target"].asString(), transfer_result = js_value["transfer_result"].asString();
            s_return.type_ = CommunicationType::TRANSFERFILERESPOND;

        } else if (js_value["communication_type"].asString() == "transferfile") { // transfer_file
            std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
                target = js_value["target"].asString(), file_content = js_value["file_content"].asString(),
                file_length = js_value["file_length"].asString();
            s_return.type_ = CommunicationType::TRANSFERFILE;

        } else { // other
            s_return.type_ = CommunicationType::NULLCOMMUNICATION;
        }
    }
    catch (...) {
        return false;
    }
    return true;
}

std::string Encryption(const std::string & param) {
    std::string str_return;

    unsigned int len = param.length();
    for (unsigned int i = 0; i < len; ++i) {
        str_return += (~(~(param[i] ^ 0xff) + i));
    }
    return str_return;
}

std::string Decryption(const std::string & param) {
    std::string str_return;

    unsigned int len = param.length();
    for (unsigned int i = 0; i < len; ++i) {
        str_return += (~(~param[i] - i) ^ 0xff);
    }
    return str_return;
}

bool VerifyCode(const std::string & code, const std::string & code_verify) {
    std::string temp1(code), temp2(code_verify);
    ToLow(temp1), ToLow(temp2);
    if (temp1.compare(temp2) == 0) {
        return true;
    } else {
        return false;
    }
}

void ToLow(std::string & code) {
    int len = code.length();
    std::string temp("");
    char low = 0;
    for (int i = 0; i < len; ++i) {
        low = tolower(code[i]);
        temp += (low);
    }
    code = temp;

    return;
}

std::string CombineString(char ** be_combined, const int size) {
    std::string s_return("");
    for (int i = 0; i < size; ++i) {
        s_return += be_combined[i];
        s_return += "|";
    }
    return s_return.substr(0, s_return.length() - 1);
}

void SplitString(const std::string & be_converted, const char separator, std::vector<std::string>& dest) {
    int pos = -1, old_pos = 0;
    pos = be_converted.find(separator, pos + 1);
    while (pos != be_converted.npos) {
        dest.push_back(be_converted.substr(old_pos, pos - old_pos - 1));
        old_pos = pos;
        pos = be_converted.find(pos + 1, separator);
    }
    dest.push_back(be_converted.substr(old_pos + 1));

    return;
}

void SplitString(const char * be_converted, const char separator, char **dest, int & size) {
    int pos = -1, old_pos = 0, i = 0;
    std::string temp(be_converted);
    pos = temp.find(separator, pos + 1);
    if (pos == temp.npos) {
        dest[0] = new char[temp.length() + 1];
        memset(dest[0], 0, temp.length() + 1);
        memcpy_s(dest[0], temp.length() + 1, temp.c_str(), temp.length() + 1);
        size = 1;
        return;
    }
    while (pos != temp.npos) {
        std::string temp1 = temp.substr(old_pos, pos - old_pos);
        dest[i] = new char[temp1.length() + 1];
        memset(dest[i], 0, temp1.length() + 1);
        memcpy_s(dest[i++], temp1.length() + 1, temp1.c_str(), temp1.length() + 1);
        old_pos = pos + 1;
        pos = temp.find(separator, pos + 1);
    }
    dest[i] = new char[temp.substr(old_pos).length() + 1];
    memset(dest[i], 0, temp.substr(old_pos).length() + 1);
    memcpy_s(dest[i++], temp.substr(old_pos).length() + 1, temp.substr(old_pos).c_str(),
             temp.substr(old_pos).length() + 1);
    size = i;

    return;
}

std::string ToDbString(const std::string& src)
{
    std::ostringstream result;
    if (CheckValid(src)) {
        result << "'" << src << "'";
    }
    return result.str();
}

std::string DbJoin(const std::vector<long long>& srcList)
{
    std::string result("(");
    for (const auto& srcStr : srcList) {
        std::string src = std::to_string(srcStr);
        if (CheckValid(src)) {
            result += "'" + src + "', ";
        }
    }
    result += ")";
    return result;
}

std::string DbJoin(const std::vector<std::string>& srcList)
{
    std::string result("(");
    for (const auto& src : srcList) {
        if (CheckValid(src)) {
            result += "'" + src + "', ";
        }
    }
    result = result.substr(0, result.length() - 2);
    result += ")";
    return result;
}

bool CheckSqlValid(const std::string& src)
{
    std::string key[9] = {"%", "/", "union", "|", "&", "^", "#", "/*", "*/"};
    for (int i = 0; i < 9; i++) {
        if (src.find(key[i]) != std::string::npos) {
            return false;
        }
    }
    return true;
}

bool CheckValid(const std::string& src)
{
    std::string key[14] = {"and","*","="," ","%0a","%","/","union","|","&","^" ,"#","/*","*/"};
    for (int i = 0; i < 14; i++) {
        if (src.find(key[i]) != std::string::npos) {
            return false;
        }
    }
    return true;
}

bool CheckJsonValid(const std::string& src)
{
    size_t index1 = src.find_first_of('{');
    size_t index2 = src.find_last_of('}');
    unsigned int quotation{0}, colon{0}, comma{0}, character{0};
    if (index1 == std::string::npos || index2 == std::string::npos) {
        return false;
    }
    for (size_t i = index1; i < index2; ++i) {
        if (src[i] == '\"') {
            ++quotation;
        } else if (src[i] == ':') {
            ++colon;
        } else if (src[i] == ',') {
            ++comma;
        } else if ((src[i] >= 'a' && src[i] <= 'z') || (src[i] >= 'A' || src[i] <= 'Z') || (src[i] >= '0' || src[i] <= '9')
            || src[i] == ' ') {
            ++character;
        } else {
            return false;
        }
    }
    if (((colon * 4) != quotation) || ((comma + 1) != colon)) {
        return false;
    }
    return true;
}
