#include "public.h"

std::string EncodeJson(const CommunicationType type, const s_HandleRecv &s_param) {
    Json::Value js_value;
    Json::FastWriter js_write;
    std::string str_json("");
    switch (type) {
    case CommunicationType::REGISTER: {
        js_value["communication_type"] = "register";
        js_value["customer"]           = s_param.Param.register_.customer;
        js_value["password"]           = s_param.Param.register_.password;
    }
    break;

    case CommunicationType::REGISTERBACKSUCCEED: {
        js_value["communication_type"] = "registerbacksucceed";
        js_value["customer"]           = s_param.Param.registerBack_.customer;
        js_value["register_result"]    = s_param.Param.registerBack_.register_result;
    }
    break;

    case CommunicationType::REGISTERBACKFAILED: {
        js_value["communication_type"] = "registerbackfailed";
        js_value["customer"]           = s_param.Param.registerBack_.customer;
        js_value["register_result"]    = s_param.Param.registerBack_.register_result;
        js_value["description"]        = s_param.Param.registerBack_.description;
    }
    break;

    case CommunicationType::LOGIN: {
        js_value["communication_type"] = "login";
        js_value["customer"]           = s_param.Param.login_.customer;
        js_value["password"]           = s_param.Param.login_.password;
    }
    break;

    case CommunicationType::LOGINBACKSUCCEED: {
        js_value["communication_type"] = "loginbacksucceed";
        js_value["customer"]           = s_param.Param.loginBack_.customer;
        js_value["login_result"]       = s_param.Param.loginBack_.login_result;
    }
    break;

    case CommunicationType::LOGINBACKFAILED: {
        js_value["communication_type"] = "loginbackfailed";
        js_value["customer"]           = s_param.Param.loginBack_.customer;
        js_value["login_result"]       = s_param.Param.loginBack_.login_result;
        js_value["description"]        = s_param.Param.loginBack_.description;
    }
    break;

    case CommunicationType::SHOWLOGIN: {
        js_value["communication_type"] = "showlogin";
        std::string temp = CombineString(s_param.Param.showLogin_.customer, s_param.Param.showLogin_.customer_num);
        js_value["customer"]           = temp.c_str();
        js_value["customer_num"]       = s_param.Param.showLogin_.customer_num;
    }
    break;

    case CommunicationType::DELETECUSTOMER: {
        js_value["communication_type"] = "deletecustomer";
        js_value["del_customer"]       = s_param.Param.delCustomer_.customer;
        js_value["source"]             = s_param.Param.delCustomer_.source;
    }
    break;

    case CommunicationType::CHAT: {
        js_value["communication_type"] = "chat";
        js_value["content"]            = s_param.Param.chat_.content;
        js_value["target"]             = s_param.Param.chat_.target;
        js_value["source"]             = s_param.Param.chat_.source;
    }
    break;

    case CommunicationType::TRANSFERFILEREQUEST: {
        js_value["communication_type"] = "transferfilerequest";
        js_value["file_name"]          = s_param.Param.transferFileRequest_.file_name;
        js_value["file_length"]        = s_param.Param.transferFile_.file_length;
        js_value["target"]             = s_param.Param.transferFileRequest_.target;
        js_value["source"]             = s_param.Param.transferFileRequest_.source;
    }
    break;

    case CommunicationType::TRANSFERFILERESPOND: {
        js_value["communication_type"] = "transferfilerespond";
        js_value["file_name"]          = s_param.Param.transferFileRespond_.file_name;
        js_value["target"]             = s_param.Param.transferFileRespond_.target;
        js_value["source"]             = s_param.Param.transferFileRespond_.source;
        js_value["transfer_result"]    = s_param.Param.transferFileRespond_.transfer_result;
    }
    break;

    case CommunicationType::TRANSFERFILE: {
        js_value["communication_type"] = "transferfile";
        js_value["file_name"]          = s_param.Param.transferFile_.file_name;
        js_value["file_length"]        = s_param.Param.transferFile_.file_length;
        js_value["file_content"]       = s_param.Param.transferFile_.file_content;
        js_value["file_block"]         = s_param.Param.transferFile_.file_block;
        js_value["current_block"]      = s_param.Param.transferFile_.current_block;
        js_value["target"]             = s_param.Param.transferFile_.target;
        js_value["source"]             = s_param.Param.transferFile_.source;
    }
    break;

    default:
        break;
    }

    str_json = js_write.write(js_value);

    return str_json;
}

bool DecodeJson(const std::string &value, s_HandleRecv &s_return)
{
    Json::Value js_value;
    Json::Reader js_reader;
    js_reader.parse(value, js_value);
    if (!js_value.isMember("communication_type")) {
        return false;
    }

    if (js_value["communication_type"].asString() == "register") { // register
        std::string customer = js_value["customer"].asString(), password = js_value["password"].asString();
        HANDLE_MESSAGE(&s_return.Param.register_.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_return.Param.register_.password, password.length(), password.c_str());
        s_return.type_ = CommunicationType::REGISTER;
    } else if (js_value["communication_type"].asString() == "registerbacksucceed") { // register_succeed
        std::string customer = js_value["customer"].asString(), register_result = js_value["register_result"].asString();
        HANDLE_MESSAGE(&s_return.Param.registerBack_.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_return.Param.registerBack_.register_result, register_result.length(), register_result.c_str());
        s_return.type_ = CommunicationType::REGISTERBACKSUCCEED;
    } else if (js_value["communication_type"].asString() == "registerbackfailed") { // register_failed
        std::string customer = js_value["customer"].asString(), register_result = js_value["register_result"].asString(),
                    description = js_value["description"].asString();
        HANDLE_MESSAGE(&s_return.Param.registerBack_.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_return.Param.registerBack_.register_result, register_result.length(), register_result.c_str());
        HANDLE_MESSAGE(&s_return.Param.registerBack_.description, description.length(), description.c_str());
        s_return.type_ = CommunicationType::REGISTERBACKFAILED;
    } else if (js_value["communication_type"].asString() == "login") { // login
        std::string customer = js_value["customer"].asString(), password = js_value["password"].asString();
        HANDLE_MESSAGE(&s_return.Param.login_.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_return.Param.login_.password, password.length(), password.c_str());
        s_return.type_ = CommunicationType::LOGIN;
    } else if (js_value["communication_type"].asString() == "loginbacksucceed") { // login_succeed
        std::string customer = js_value["customer"].asString(), login_result = js_value["login_result"].asString();
        HANDLE_MESSAGE(&s_return.Param.loginBack_.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_return.Param.loginBack_.login_result, login_result.length(), login_result.c_str());
        s_return.type_ = CommunicationType::LOGINBACKSUCCEED;
    } else if (js_value["communication_type"].asString() == "loginbackfailed") { // login_faileds
        std::string customer = js_value["customer"].asString(), login_result = js_value["login_result"].asString(),
                    description = js_value["description"].asString();
        HANDLE_MESSAGE(&s_return.Param.loginBack_.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_return.Param.loginBack_.login_result, login_result.length(), login_result.c_str());
        HANDLE_MESSAGE(&s_return.Param.loginBack_.description, description.length(), description.c_str());
        s_return.type_ = CommunicationType::LOGINBACKFAILED;
    } else if (js_value["communication_type"].asString() == "showlogin") { // show_login
        std::string customer = js_value["customer"].asString();
        int size = 0;
        s_return.Param.showLogin_.customer = new char *[js_value["customer_num"].asInt()];
        s_return.Param.showLogin_.customer_num = js_value["customer_num"].asInt();
        SplitString(customer.c_str(), '|', s_return.Param.showLogin_.customer, size);
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
        std::string customer = js_value["customer"].asString(), source = js_value["source"].asString();
        HANDLE_MESSAGE(&s_return.Param.delCustomer_.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_return.Param.delCustomer_.source, source.length(), source.c_str());
        s_return.type_ = CommunicationType::DELETECUSTOMER;
    } else if (js_value["communication_type"].asString() == "chat") { // chat
        std::string content = js_value["content"].asString(), source = js_value["source"].asString(),
                    target = js_value["target"].asString();
        HANDLE_MESSAGE(&s_return.Param.chat_.content, content.length(), content.c_str());
        HANDLE_MESSAGE(&s_return.Param.chat_.source, source.length(), source.c_str());
        HANDLE_MESSAGE(&s_return.Param.chat_.target, target.length(), target.c_str());
        s_return.type_ = CommunicationType::CHAT;
    } else if (js_value["communication_type"].asString() == "transferfilerequest") { // transfer_filequest
        std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
                    target = js_value["target"].asString(), file_length = js_value["file_length"].asString();
        HANDLE_MESSAGE(&s_return.Param.transferFileRequest_.file_name, file_name.length(), file_name.c_str());
        HANDLE_MESSAGE(&s_return.Param.transferFileRequest_.source, source.length(), source.c_str());
        HANDLE_MESSAGE(&s_return.Param.transferFileRequest_.target, target.length(), target.c_str());
        HANDLE_MESSAGE(&s_return.Param.transferFileRequest_.file_length, file_length.length(), file_length.c_str());
        s_return.type_ = CommunicationType::TRANSFERFILEREQUEST;
    } else if (js_value["communication_type"].asString() == "transferfilerespond") { // transfer_filerespond
        std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
                    target = js_value["target"].asString(), transfer_result = js_value["transfer_result"].asString();
        HANDLE_MESSAGE(&s_return.Param.transferFileRespond_.file_name, file_name.length(), file_name.c_str());
        HANDLE_MESSAGE(&s_return.Param.transferFileRespond_.source, source.length(), source.c_str());
        HANDLE_MESSAGE(&s_return.Param.transferFileRespond_.target, target.length(), target.c_str());
        HANDLE_MESSAGE(&s_return.Param.transferFileRespond_.transfer_result, transfer_result.length(), transfer_result.c_str());
        s_return.type_ = CommunicationType::TRANSFERFILERESPOND;
    } else if (js_value["communication_type"].asString() == "transferfile") { // transfer_file
        std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
                    target = js_value["target"].asString(), file_content = js_value["file_content"].asString(),
                    file_length = js_value["file_length"].asString();
        HANDLE_MESSAGE(&s_return.Param.transferFile_.file_name, file_name.length(), file_name.c_str());
        HANDLE_MESSAGE(&s_return.Param.transferFile_.source, source.length(), source.c_str());
        HANDLE_MESSAGE(&s_return.Param.transferFile_.target, target.length(), target.c_str());
        HANDLE_MESSAGE(&s_return.Param.transferFileRequest_.file_length, file_length.length(), file_length.c_str());
        HANDLE_MESSAGE(&s_return.Param.transferFile_.file_content, file_content.length(), file_content.c_str());
        s_return.Param.transferFile_.file_block = js_value["file_block"].asUInt();
        s_return.Param.transferFile_.current_block = js_value["current_block"].asUInt();
        s_return.type_ = CommunicationType::TRANSFERFILE;
    } else { // other
        s_return.type_ = CommunicationType::NULLCOMMUNICATION;
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
