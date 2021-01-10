#include "public.h"

#include <sstream>

std::string cwy::EncodeJson(const CommunicationType type, const s_HandleRecv& lParam) {
    CHandleJson* handleJson = new CHandleJson("");
    switch (type) {
    case CommunicationType::TOKEN: {
        handleJson->SetJsonValue("communication_type", "token");
        handleJson->SetJsonValue("token_length", lParam.param_.token_.tokenLength_);
        handleJson->SetJsonValue("content", lParam.param_.token_.content_);
    }
    break;

    case CommunicationType::TOKENBACK: {
        handleJson->SetJsonValue("communication_type", "tokenback");
        handleJson->SetJsonValue("token_length", lParam.param_.tokenBack_.tokenLength_);
        handleJson->SetJsonValue("content", lParam.param_.tokenBack_.content_);
    }
    break;

    case CommunicationType::REGISTER: {
        handleJson->SetJsonValue("communication_type", "register");
        handleJson->SetJsonValue("customer", lParam.param_.register_.customer_);
        handleJson->SetJsonValue("password", lParam.param_.register_.password_);
    }
    break;

    case CommunicationType::REGISTERBACK: {
        handleJson->SetJsonValue("communication_type", "registerback");
        handleJson->SetJsonValue("customer", lParam.param_.registerBack_.customer_);
        handleJson->SetJsonValue("register_result", lParam.param_.registerBack_.registerResult_);
        handleJson->SetJsonValue("id", lParam.param_.registerBack_.id_);
    }
    break;

    case CommunicationType::LOGIN: {
        handleJson->SetJsonValue("communication_type", "login");
        handleJson->SetJsonValue("id", lParam.param_.login_.id_);
        handleJson->SetJsonValue("password", lParam.param_.login_.password_);
    }
    break;

    case CommunicationType::LOGINBACK: {
        handleJson->SetJsonValue("communication_type", "loginback");
        handleJson->SetJsonValue("customer", lParam.param_.loginBack_.customer_);
        handleJson->SetJsonValue("login_result", lParam.param_.loginBack_.loginResult_);
        handleJson->SetJsonValue("description", lParam.param_.loginBack_.description_);
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

bool cwy::DecodeJson(const std::string& message, s_HandleRecv& lParam) {
    CHandleJson* handleJson = new CHandleJson(message);
    std::string communicationType = handleJson->GetJsonValueString("communication_type");
    if (communicationType == "token") { // token
        std::string token = handleJson->GetJsonValueString("content");
        lParam.param_.token_.tokenLength_ = (int)handleJson->GetJsonValueNum("token_length");
        HANDLE_MESSAGE(&lParam.param_.token_.content_, token.length(), token.c_str());
        lParam.type_ = CommunicationType::TOKEN;
    } else if (communicationType == "tokenback") { // token_back
        std::string token = handleJson->GetJsonValueString("content");
        lParam.param_.token_.tokenLength_ = (int)handleJson->GetJsonValueNum("token_length");
        HANDLE_MESSAGE(&lParam.param_.token_.content_, token.length(), token.c_str());
        lParam.type_ = CommunicationType::TOKENBACK;
    } else if (communicationType == "register") { // register
        std::string customer = handleJson->GetJsonValueString("customer"),
            password = handleJson->GetJsonValueString("password");
        HANDLE_MESSAGE(&lParam.param_.register_.customer_, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&lParam.param_.register_.password_, password.length(), password.c_str());
        lParam.type_ = CommunicationType::REGISTER;
    } else if (communicationType == "registerback") { // register_back
        std::string customer = handleJson->GetJsonValueString("customer"),
            register_result = handleJson->GetJsonValueString("register_result");
        lParam.param_.registerBack_.id_ = handleJson->GetJsonValueNum("id");
        HANDLE_MESSAGE(&lParam.param_.registerBack_.customer_, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&lParam.param_.registerBack_.registerResult_, register_result.length(), register_result.c_str());
        lParam.type_ = CommunicationType::REGISTERBACK;
    } else if (communicationType == "login") { // login
        std::string customer = handleJson->GetJsonValueString("customer"),
                    password = handleJson->GetJsonValueString("password");
        lParam.param_.login_.id_ = handleJson->GetJsonValueNum("id");
        HANDLE_MESSAGE(&lParam.param_.login_.password_, password.length(), password.c_str());
        lParam.type_ = CommunicationType::LOGIN;
    } else if (communicationType == "loginback") { // login_back
        std::string customer = handleJson->GetJsonValueString("customer"),
                    login_result = handleJson->GetJsonValueString("login_result"),
                    description = handleJson->GetJsonValueString("description");
        HANDLE_MESSAGE(&lParam.param_.loginBack_.customer_, customer.length(), customer.c_str());
        HANDLE_MESSAGE(&lParam.param_.loginBack_.loginResult_, login_result.length(), login_result.c_str());
        HANDLE_MESSAGE(&lParam.param_.loginBack_.description_, description.length(), description.c_str());
        lParam.type_ = CommunicationType::LOGINBACK;
    //} else if (handleJson->GetJsonValue("communication_type") == "showlogin") { // show_login
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
    } else { // other
        lParam.type_ = CommunicationType::NULLCOMMUNICATION;
    }
    delete handleJson;
    return true;
}

void cwy::DeleteMemory(const CommunicationType type, s_HandleRecv& lParam)
{
    switch (type)
    {
    case CommunicationType::REGISTER: {
        delete[]lParam.param_.register_.customer_;
        delete[]lParam.param_.register_.password_;
    }
        break;

    case CommunicationType::REGISTERBACK: {
        delete[]lParam.param_.registerBack_.customer_;
        delete[]lParam.param_.registerBack_.registerResult_;
    }
        break;

    case CommunicationType::LOGIN: {
        delete[]lParam.param_.login_.password_;
    }
        break;

    case CommunicationType::LOGINBACK: {
        delete[]lParam.param_.loginBack_.customer_;
        delete[]lParam.param_.loginBack_.description_;
        delete[]lParam.param_.loginBack_.loginResult_;
    }
        break;
    }
}

void cwy::GetToken(std::string& token, int& tokenLength)
{
    srand((unsigned int)time(NULL));
    int code_rand = 0;
    for (int i = 0; i < 16; ++i) {
        code_rand = rand() % strlen(cwy::VERIFY_CODE);
        token += cwy::VERIFY_CODE[code_rand];
    }
    tokenLength = 16;
}

bool cwy::AnalToken(const std::string& token)
{
    return false;
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
    strJsonWrite_ += "{";
}

void cwy::CHandleJson::InitJson(const std::string& message)
{
    strJsonRead_ = message;
}

void cwy::CHandleJson::ClearJson()
{
    strJsonRead_ = "";
    strJsonWrite_ = "";
}

std::string cwy::CHandleJson::GetJsonValueString(const std::string& key) const
{
    std::string result = "";
    int pos = strJsonRead_.find(key + "\":");
    if (pos != std::string::npos) {
        int pos1 = strJsonRead_.find_first_of(",", pos + 1);
        if (pos1 == std::string::npos) {
            pos1 = strJsonRead_.find_first_of("}", pos + 1);
        }
        result = strJsonRead_.substr(pos + key.length() + 3, pos1 - pos - key.length() - 4);
    }
    return result;
}

long long cwy::CHandleJson::GetJsonValueNum(const std::string& key) const
{
    long long result = 0;
    int pos = strJsonRead_.find(key + "\":");
    if (pos != std::string::npos) {
        int pos1 = strJsonRead_.find_first_of(",", pos + 1);
        if (pos1 == std::string::npos) {
            pos1 = strJsonRead_.find_first_of("}", pos + 1);
        }
        std::string tmp = strJsonRead_.substr(pos + key.length() + 2, pos1 - pos - key.length() - 2);
        result = atoll(tmp.c_str());
    }
    return result;
}

void cwy::CHandleJson::SetJsonValue(const std::string& key, const std::string& value)
{
    std::ostringstream tmp;
    tmp << "\"" << key << "\":\"" << value << "\",";
    strJsonWrite_ += tmp.str();
}

void cwy::CHandleJson::SetJsonValue(const std::string& key, const long long value)
{
    std::ostringstream tmp;
    tmp << "\"" << key << "\":" << value << ",";
    strJsonWrite_ += tmp.str();
}

std::string cwy::CHandleJson::GetJsonBack()
{
    strJsonWrite_ = strJsonWrite_.substr(0, strJsonWrite_.length() - 1);
    strJsonWrite_ += "}";
    return strJsonWrite_;
}

cwy::CHandleIni::CHandleIni(const std::string& file_path)
{
}

std::string cwy::CHandleIni::GetValue(const std::string& key) const
{
    return std::string();
}