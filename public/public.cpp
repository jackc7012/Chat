#include "public.h"

std::string cwy::EncodeJson(const CommunicationType type, const s_HandleRecv& param) {
    CHandleJson* handleJson = new CHandleJson("");
    switch (type) {
    case CommunicationType::REGISTER: {
        handleJson->SetJsonValue("communication_type", "register");
        handleJson->SetJsonValue("customer", param.param.Register.customer);
        handleJson->SetJsonValue("password", param.param.Register.password);
    }
    break;

    case CommunicationType::REGISTERBACK: {
        handleJson->SetJsonValue("communication_type", "registerback");
        handleJson->SetJsonValue("customer", param.param.RegisterBack.customer);
        handleJson->SetJsonValue("register_result", param.param.RegisterBack.register_result);
        handleJson->SetJsonValue("id", param.param.RegisterBack.id);
    }
    break;

    case CommunicationType::LOGIN: {
        handleJson->SetJsonValue("communication_type", "login");
        handleJson->SetJsonValue("customer", param.param.Login.customer);
        handleJson->SetJsonValue("password", param.param.Login.password);
    }
    break;

    case CommunicationType::LOGINBACK: {
        handleJson->SetJsonValue("communication_type", "loginback");
        handleJson->SetJsonValue("customer", param.param.LoginBack.customer);
        handleJson->SetJsonValue("login_result", param.param.LoginBack.login_result);
        handleJson->SetJsonValue("description", param.param.LoginBack.description);
    }
    break;

    /*case CommunicationType::SHOWLOGIN: {
        js_value["communication_type"] = "showlogin";
        std::string temp = mychat::CombineString(param.param.ShowLogin.customer, param.param.ShowLogin.customer_num);
        js_value["customer"]           = temp.c_str();
        js_value["customer_num"]       = param.param.ShowLogin.customer_num;
    }
    break;

    case CommunicationType::DELETECUSTOMER: {
        js_value["communication_type"] = "deletecustomer";
        js_value["del_customer"]       = param.param.DelCustomer.customer;
        js_value["source"]             = param.param.DelCustomer.source;
    }
    break;

    case CommunicationType::CHAT: {
        js_value["communication_type"] = "chat";
        js_value["content"]            = param.param.Chat.content;
        js_value["target"]             = param.param.Chat.target;
        js_value["source"]             = param.param.Chat.source;
    }
    break;

    case CommunicationType::TRANSFERFILEREQUEST: {
        js_value["communication_type"] = "transferfilerequest";
        js_value["file_name"]          = param.param.TransferFileRequest.file_name;
        js_value["file_length"]        = param.param.TransferFile.file_length;
        js_value["target"]             = param.param.TransferFileRequest.target;
        js_value["source"]             = param.param.TransferFileRequest.source;
    }
    break;

    case CommunicationType::TRANSFERFILERESPOND: {
        js_value["communication_type"] = "transferfilerespond";
        js_value["file_name"]          = param.param.TransferFileRespond.file_name;
        js_value["target"]             = param.param.TransferFileRespond.target;
        js_value["source"]             = param.param.TransferFileRespond.source;
        js_value["transfer_result"]     = param.param.TransferFileRespond.transfer_result;
    }
    break;

    case CommunicationType::TRANSFERFILE: {
        js_value["communication_type"] = "transferfile";
        js_value["file_name"]          = param.param.TransferFile.file_name;
        js_value["file_length"]        = param.param.TransferFile.file_length;
        js_value["file_content"]       = param.param.TransferFile.file_content;
        js_value["file_block"]         = param.param.TransferFile.file_block;
        js_value["current_block"]      = param.param.TransferFile.current_block;
        js_value["target"]             = param.param.TransferFile.target;
        js_value["source"]             = param.param.TransferFile.source;
    }
    break;*/
    }
    std::string result = handleJson->GetJsonBack();
    delete handleJson;
    return result;
}

bool cwy::DecodeJson(const std::string& message, s_HandleRecv& s_param) {
    CHandleJson* handleJson = new CHandleJson(message);
    std::string communicationType = handleJson->GetJsonValueString("communication_type");
    if (communicationType == "register") { // register
        std::string customer = handleJson->GetJsonValueString("customer"),
            password = handleJson->GetJsonValueString("password");
        HANDLE_MESSAGE(&s_param.param.Register.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_param.param.Register.password, password.length(), password.c_str());
        s_param.type = CommunicationType::REGISTER;
    } 
    else if (communicationType == "registerback") { // register_back
        std::string customer = handleJson->GetJsonValueString("customer"),
            register_result = handleJson->GetJsonValueString("register_result");
        s_param.param.RegisterBack.id = handleJson->GetJsonValueNum("id");
        HANDLE_MESSAGE(&s_param.param.RegisterBack.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_param.param.RegisterBack.register_result, register_result.length(), register_result.c_str());
        s_param.type = CommunicationType::REGISTERBACK;
    } 
    else if (communicationType == "login") { // login
        std::string customer = handleJson->GetJsonValueString("customer"),
                    password = handleJson->GetJsonValueString("password");
        HANDLE_MESSAGE(&s_param.param.Login.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_param.param.Login.password, password.length(), password.c_str());
        s_param.type = CommunicationType::LOGIN;
    } 
    else if (communicationType == "loginback") { // login_back
        std::string customer = handleJson->GetJsonValueString("customer"),
                    login_result = handleJson->GetJsonValueString("login_result"),
                    description = handleJson->GetJsonValueString("description");
        HANDLE_MESSAGE(&s_param.param.LoginBack.customer, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&s_param.param.LoginBack.login_result, login_result.length(), login_result.c_str());
        HANDLE_MESSAGE(&s_param.param.LoginBack.description, description.length(), description.c_str());
        s_param.type = CommunicationType::LOGINBACK;
    } 
    //else if (handleJson->GetJsonValue("communication_type") == "showlogin") { // show_login
    //    std::string customer = js_value["customer"].asString();
    //    int size = 0;
    //    lReturn.param.ShowLogin.customer = new char *[js_value["customer_num"].asInt()];
    //    lReturn.param.ShowLogin.customer_num = js_value["customer_num"].asInt();
    //    SplitString(customer.c_str(), '|', lReturn.param.ShowLogin.customer, size);
    //    if (size != js_value["customer_num"].asInt()) {
    //        lReturn.type = CommunicationType::ERRORCOMMUNICATION;
    //        for (int i = 0; i < size; ++i) {
    //            delete[]lReturn.param.ShowLogin.customer[i];
    //        }
    //        delete[]lReturn.param.ShowLogin.customer;
    //    } else {
    //        lReturn.type = CommunicationType::SHOWLOGIN;
    //    }
    //} 
    //else if (handleJson->GetJsonValue("communication_type") == "delcustomer") { // delete_customer
    //    std::string customer = js_value["customer"].asString(), source = js_value["source"].asString();
    //    HANDLE_MESSAGE(&lReturn.param.DelCustomer.customer, customer.length(), customer.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.DelCustomer.source, source.length(), source.c_str());
    //    lReturn.type = CommunicationType::DELETECUSTOMER;
    //} 
    //else if (handleJson->GetJsonValue("communication_type") == "chat") { // chat
    //    std::string content = js_value["content"].asString(), source = js_value["source"].asString(),
    //                target = js_value["target"].asString();
    //    HANDLE_MESSAGE(&lReturn.param.Chat.content, content.length(), content.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.Chat.source, source.length(), source.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.Chat.target, target.length(), target.c_str());
    //    lReturn.type = CommunicationType::CHAT;
    //} 
    //else if (handleJson->GetJsonValue("communication_type") == "transferfilerequest") { // transfer_filequest
    //    std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
    //                target = js_value["target"].asString(), file_length = js_value["file_length"].asString();
    //    HANDLE_MESSAGE(&lReturn.param.TransferFileRequest.file_name, file_name.length(), file_name.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.TransferFileRequest.source, source.length(), source.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.TransferFileRequest.target, target.length(), target.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.TransferFileRequest.file_length, file_length.length(), file_length.c_str());
    //    lReturn.type = CommunicationType::TRANSFERFILEREQUEST;
    //} 
    //else if (handleJson->GetJsonValue("communication_type") == "transferfilerespond") { // transfer_filerespond
    //    std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
    //                target = js_value["target"].asString(), transfer_result = js_value["transfer_result"].asString();
    //    HANDLE_MESSAGE(&lReturn.param.TransferFileRespond.file_name, file_name.length(), file_name.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.TransferFileRespond.source, source.length(), source.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.TransferFileRespond.target, target.length(), target.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.TransferFileRespond.transfer_result, transfer_result.length(), transfer_result.c_str());
    //    lReturn.type = CommunicationType::TRANSFERFILERESPOND;
    //} 
    //else if (handleJson->GetJsonValue("communication_type") == "transferfile") { // transfer_file
    //    std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
    //                target = js_value["target"].asString(), file_content = js_value["file_content"].asString(),
    //                file_length = js_value["file_length"].asString();
    //    HANDLE_MESSAGE(&lReturn.param.TransferFile.file_name, file_name.length(), file_name.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.TransferFile.source, source.length(), source.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.TransferFile.target, target.length(), target.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.TransferFileRequest.file_length, file_length.length(), file_length.c_str());
    //    HANDLE_MESSAGE(&lReturn.param.TransferFile.file_content, file_content.length(), file_content.c_str());
    //    lReturn.param.TransferFile.file_block = js_value["file_block"].asUInt();
    //    lReturn.param.TransferFile.current_block = js_value["current_block"].asUInt();
    //    lReturn.type = CommunicationType::TRANSFERFILE;
    //} 
    else { // other
        s_param.type = CommunicationType::NULLCOMMUNICATION;
    }
    delete handleJson;
    return true;
}

void cwy::DeleteMemory(const CommunicationType type, s_HandleRecv& s_param)
{
    switch (type)
    {
    case CommunicationType::REGISTER: {
        delete[]s_param.param.Register.customer;
        delete[]s_param.param.Register.password;
    }
        break;

    case CommunicationType::REGISTERBACK: {
        delete[]s_param.param.RegisterBack.customer;
        delete[]s_param.param.RegisterBack.register_result;
    }
        break;

    case CommunicationType::LOGIN: {
        delete[]s_param.param.Login.customer;
        delete[]s_param.param.Login.password;
    }
        break;

    case CommunicationType::LOGINBACK: {
        delete[]s_param.param.LoginBack.customer;
        delete[]s_param.param.LoginBack.description;
        delete[]s_param.param.LoginBack.login_result;
    }
        break;
    }
}

std::string cwy::Encryption(const std::string& param) {
    std::string str_return;

    unsigned int len = param.length();
    for (unsigned int i = 0; i < len; ++i) {
        str_return += (~(~(param[i] ^ 0xff) + i));
    }
    return str_return;
}

std::string cwy::Decryption(const std::string& param) {
    std::string str_return;

    unsigned int len = param.length();
    for (unsigned int i = 0; i < len; ++i) {
        str_return += (~(~param[i] - i) ^ 0xff);
    }
    return str_return;
}

bool cwy::VerifyCode(const std::string& code, const std::string& code_verify) {
    std::string temp1(code), temp2(code_verify);
    cwy::ToLow(temp1), cwy::ToLow(temp2);
    if (temp1.compare(temp2) == 0) {
        return true;
    } else {
        return false;
    }
}

void cwy::ToLow(std::string& code) {
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

std::string cwy::CombineString(char** be_combined, const int size) {
    std::string s_return("");
    for (int i = 0; i < size; ++i) {
        s_return += be_combined[i];
        s_return += "|";
    }
    return s_return.substr(0, s_return.length() - 1);
}

void cwy::SplitString(const std::string& be_converted, const char separator, std::vector<std::string>& dest) {
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

void cwy::SplitString(const char * be_converted, const char separator, char **dest, int & size) {
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

cwy::CHandleJson::CHandleJson(const std::string& message)
{
    ClearJson();
    InitJson(message);
    strJsonWrite += "{";
}

void cwy::CHandleJson::InitJson(const std::string& message)
{
    strJsonRead = message;
}

void cwy::CHandleJson::ClearJson()
{
    strJsonRead = "";
    strJsonWrite = "";
}

std::string cwy::CHandleJson::GetJsonValueString(const std::string& key) const
{
    std::string result = "";
    int pos = strJsonRead.find(key + "\":");
    if (pos != std::string::npos) {
        int pos1 = strJsonRead.find_first_of(",", pos + 1);
        if (pos1 == std::string::npos) {
            pos1 = strJsonRead.find_first_of("}", pos + 1);
        }
        result = strJsonRead.substr(pos + key.length() + 3, pos1 - pos - key.length() - 4);
    }
    return result;
}

long long cwy::CHandleJson::GetJsonValueNum(const std::string& key) const
{
    long long result = 0;
    int pos = strJsonRead.find(key + "\":");
    if (pos != std::string::npos) {
        int pos1 = strJsonRead.find_first_of(",", pos + 1);
        if (pos1 == std::string::npos) {
            pos1 = strJsonRead.find_first_of("}", pos + 1);
        }
        std::string tmp = strJsonRead.substr(pos + key.length() + 2, pos1 - pos - key.length() - 2);
        result = atoi(tmp.c_str());
    }
    return result;
}

void cwy::CHandleJson::SetJsonValue(const std::string& key, const std::string& value)
{
    std::ostringstream tmp;
    tmp << "\"" << key << "\":\"" << value << "\",";
    strJsonWrite += tmp.str();
}

void cwy::CHandleJson::SetJsonValue(const std::string& key, const long long value)
{
    std::ostringstream tmp;
    tmp << "\"" << key << "\":" << value << ",";
    strJsonWrite += tmp.str();
}

std::string cwy::CHandleJson::GetJsonBack()
{
    strJsonWrite = strJsonWrite.substr(0, strJsonWrite.length() - 1);
    strJsonWrite += "}";
    return strJsonWrite;
}

cwy::CHandleIni::CHandleIni(const std::string& file_path)
{
}

std::string cwy::CHandleIni::GetValue(const std::string& key) const
{
    return std::string();
}
