#include "public.h"

namespace mywork {
    std::string EncodeJson(const CommunicationType type, const HandleRecv& handleRecv) {
        Json::Value js_value;
        Json::FastWriter js_write;
        std::string str_json("");
        switch (type) {
        case CommunicationType::REGISTER: {
            js_value["communication_type"] = "register";
            js_value["customer"] = handleRecv.param.registerIn.customer;
            js_value["password"] = handleRecv.param.registerIn.password;
        }
            break;

        case CommunicationType::REGISTERBACKSUCCEED: {
            js_value["communication_type"] = "registerbacksucceed";
            js_value["customer"] = handleRecv.param.registerBack.customer;
            js_value["register_result"] = handleRecv.param.registerBack.registerResult;
        }
            break;

        case CommunicationType::REGISTERBACKFAILED: {
            js_value["communication_type"] = "registerbackfailed";
            js_value["customer"] = handleRecv.param.registerBack.customer;
            js_value["register_result"] = handleRecv.param.registerBack.registerResult;
            js_value["description"] = handleRecv.param.registerBack.description;
        }
            break;

        case CommunicationType::LOGIN: {
            js_value["communication_type"] = "login";
            js_value["customer"] = handleRecv.param.login.customer;
            js_value["password"] = handleRecv.param.login.password;
        }
            break;

        case CommunicationType::LOGINBACKSUCCEED: {
            js_value["communication_type"] = "loginbacksucceed";
            js_value["customer"] = handleRecv.param.loginBack.customer;
            js_value["login_result"] = handleRecv.param.loginBack.loginResult;
        }
            break;

        case CommunicationType::LOGINBACKFAILED: {
            js_value["communication_type"] = "loginbackfailed";
            js_value["customer"] = handleRecv.param.loginBack.customer;
            js_value["login_result"] = handleRecv.param.loginBack.loginResult;
            js_value["description"] = handleRecv.param.loginBack.description;
        }
            break;

        case CommunicationType::SHOWLOGIN: {
            js_value["communication_type"] = "showlogin";
            std::string temp = CombineString(handleRecv.param.showLogin.customer, handleRecv.param.showLogin.customerNum);
            js_value["customer"] = temp.c_str();
            js_value["customer_num"] = handleRecv.param.showLogin.customerNum;
        }
            break;

        case CommunicationType::DELETECUSTOMER: {
            js_value["communication_type"] = "deletecustomer";
            js_value["del_customer"] = handleRecv.param.delCustomer.customer;
        }
            break;

        case CommunicationType::CHAT: {
            js_value["communication_type"] = "chat";
            js_value["content"] = handleRecv.param.chat.content;
            js_value["target"] = handleRecv.param.chat.target;
            js_value["source"] = handleRecv.param.chat.source;
        }
            break;

        case TRANSFERFILEREQUEST: {
            js_value["communication_type"] = "transferfilerequest";
            js_value["file_name"] = s_param.Param.TransferFileRequest.file_name;
            js_value["file_length"] = s_param.Param.TransferFile.file_length;
            js_value["target"] = s_param.Param.TransferFileRequest.target;
            js_value["source"] = s_param.Param.TransferFileRequest.source;
        }
                                break;

        case TRANSFERFILERESPOND: {
            js_value["communication_type"] = "transferfilerespond";
            js_value["file_name"] = s_param.Param.TransferFileRespond.file_name;
            js_value["target"] = s_param.Param.TransferFileRespond.target;
            js_value["source"] = s_param.Param.TransferFileRespond.source;
            js_value["transfer_result"] = s_param.Param.TransferFileRespond.transfer_result;
        }
                                break;

        case TRANSFERFILE: {
            js_value["communication_type"] = "transferfile";
            js_value["file_name"] = s_param.Param.TransferFile.file_name;
            js_value["file_length"] = s_param.Param.TransferFile.file_length;
            js_value["file_content"] = s_param.Param.TransferFile.file_content;
            js_value["file_block"] = s_param.Param.TransferFile.file_block;
            js_value["current_block"] = s_param.Param.TransferFile.current_block;
            js_value["target"] = s_param.Param.TransferFile.target;
            js_value["source"] = s_param.Param.TransferFile.source;
        }
                         break;

        default:
            break;
        }

        str_json = js_write.write(js_value);

        return str_json;
    }

    bool DecodeJson(const std::string& value, HandleRecv& handleRecv) {
        Json::Value js_value;
        Json::Reader js_reader;
        js_reader.parse(value, js_value);

        if (js_value["communication_type"].asString() == "register") { // register
            std::string customer = js_value["customer"].asString(), password = js_value["password"].asString();
            RequestMemory(&handleRecv.param.registerIn.customer, customer.length(), customer.c_str());
            RequestMemory(&handleRecv.param.registerIn.password, password.length(), password.c_str());
            handleRecv.communicationType = CommunicationType::REGISTER;
        }
        else if (js_value["communication_type"].asString() == "registerbacksucceed") { // register_succeed
            std::string customer = js_value["customer"].asString(), register_result = js_value["register_result"].asString();
            RequestMemory(&handleRecv.param.registerBack.customer, customer.length(), customer.c_str());
            RequestMemory(&handleRecv.param.registerBack.registerResult, register_result.length(), register_result.c_str());
            handleRecv.communicationType = CommunicationType::REGISTERBACKSUCCEED;
        }
        else if (js_value["communication_type"].asString() == "registerbackfailed") { // register_failed
            std::string customer = js_value["customer"].asString(), register_result = js_value["register_result"].asString(),
                description = js_value["description"].asString();
            RequestMemory(&handleRecv.param.registerBack.customer, customer.length(), customer.c_str());
            RequestMemory(&handleRecv.param.registerBack.registerResult, register_result.length(), register_result.c_str());
            RequestMemory(&handleRecv.param.registerBack.description, description.length(), description.c_str());
            handleRecv.communicationType = CommunicationType::REGISTERBACKFAILED;
        }
        else if (js_value["communication_type"].asString() == "login") { // login
            std::string customer = js_value["customer"].asString(), password = js_value["password"].asString(),
                ip = js_value["ip"].asString();
            RequestMemory(&handleRecv.param.login.customer, customer.length(), customer.c_str());
            RequestMemory(&handleRecv.param.login.password, password.length(), password.c_str());
            RequestMemory(&handleRecv.param.login.ip, ip.length(), ip.c_str());
            handleRecv.communicationType = CommunicationType::LOGIN;
        }
        else if (js_value["communication_type"].asString() == "loginbacksucceed") { // login_succeed
            std::string customer = js_value["customer"].asString(), login_result = js_value["login_result"].asString();
            RequestMemory(&handleRecv.param.loginBack.customer, customer.length(), customer.c_str());
            RequestMemory(&handleRecv.param.loginBack.loginResult, login_result.length(), login_result.c_str());
            handleRecv.communicationType = CommunicationType::LOGINBACKSUCCEED;
        }
        else if (js_value["communication_type"].asString() == "loginbackfailed") { // login_faileds
            std::string customer = js_value["customer"].asString(), login_result = js_value["login_result"].asString(),
                description = js_value["description"].asString();
            RequestMemory(&handleRecv.param.loginBack.customer, customer.length(), customer.c_str());
            RequestMemory(&handleRecv.param.loginBack.loginResult, login_result.length(), login_result.c_str());
            RequestMemory(&handleRecv.param.loginBack.description, description.length(), description.c_str());
            handleRecv.communicationType = CommunicationType::LOGINBACKFAILED;
        }
        else if (js_value["communication_type"].asString() == "showlogin") { // show_login
            std::string customer = js_value["customer"].asString();
            int size = 0;
            handleRecv.param.showLogin.customer = new char* [js_value["customer_num"].asInt()];
            handleRecv.param.showLogin.customerNum = js_value["customer_num"].asInt();
            SplitString(customer.c_str(), '|', handleRecv.param.showLogin.customer, size);
            if (size != js_value["customer_num"].asInt()) {
                handleRecv.communicationType = CommunicationType::ERRORCOMMUNICATION;
                for (int i = 0; i < size; ++i) {
                    delete[]handleRecv.param.showLogin.customer[i];
                }
                delete[]handleRecv.param.showLogin.customer;
            }
            else {
                handleRecv.communicationType = CommunicationType::SHOWLOGIN;
            }
        }
        else if (js_value["communication_type"].asString() == "deletecustomer") { // delete_customer
            std::string customer = js_value["del_customer"].asString();
            RequestMemory(&handleRecv.param.delCustomer.customer, customer.length(), customer.c_str());
            handleRecv.communicationType = CommunicationType::DELETECUSTOMER;
        }
        else if (js_value["communication_type"].asString() == "chat") { // chat
            std::string content = js_value["content"].asString(), source = js_value["source"].asString(),
                target = js_value["target"].asString();
            RequestMemory(&handleRecv.param.chat.content, content.length(), content.c_str());
            RequestMemory(&handleRecv.param.chat.source, source.length(), source.c_str());
            RequestMemory(&handleRecv.param.chat.target, target.length(), target.c_str());
            handleRecv.communicationType = CommunicationType::CHAT;
        }
        else if (js_value["communication_type"].asString() == "transferfilerequest") { // transfer_filequest
            std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
                target = js_value["target"].asString(), file_length = js_value["file_length"].asString();
            RequestMemory(&s_return.Param.TransferFileRequest.file_name, file_name.length(), file_name.c_str());
            RequestMemory(&s_return.Param.TransferFileRequest.source, source.length(), source.c_str());
            RequestMemory(&s_return.Param.TransferFileRequest.target, target.length(), target.c_str());
            RequestMemory(&s_return.Param.TransferFileRequest.file_length, file_length.length(), file_length.c_str());
            s_return.communicationType = TRANSFERFILEREQUEST;
        }
        else if (js_value["communication_type"].asString() == "transferfilerespond") { // transfer_filerespond
            std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
                target = js_value["target"].asString(), transfer_result = js_value["transfer_result"].asString();
            RequestMemory(&s_return.Param.TransferFileRespond.file_name, file_name.length(), file_name.c_str());
            RequestMemory(&s_return.Param.TransferFileRespond.source, source.length(), source.c_str());
            RequestMemory(&s_return.Param.TransferFileRespond.target, target.length(), target.c_str());
            RequestMemory(&s_return.Param.TransferFileRespond.transfer_result, transfer_result.length(), transfer_result.c_str());
            s_return.communicationType = TRANSFERFILERESPOND;
        }
        else if (js_value["communication_type"].asString() == "transferfile") { // transfer_file
            std::string file_name = js_value["file_name"].asString(), source = js_value["source"].asString(),
                target = js_value["target"].asString(), file_content = js_value["file_content"].asString(),
                file_length = js_value["file_length"].asString();
            RequestMemory(&s_return.Param.TransferFile.file_name, file_name.length(), file_name.c_str());
            RequestMemory(&s_return.Param.TransferFile.source, source.length(), source.c_str());
            RequestMemory(&s_return.Param.TransferFile.target, target.length(), target.c_str());
            RequestMemory(&s_return.Param.TransferFileRequest.file_length, file_length.length(), file_length.c_str());
            RequestMemory(&s_return.Param.TransferFile.file_content, file_content.length(), file_content.c_str());
            s_return.Param.TransferFile.file_block = js_value["file_block"].asUInt();
            s_return.Param.TransferFile.current_block = js_value["current_block"].asUInt();
            s_return.communicationType = TRANSFERFILE;
        }
        else { // other
            s_return.communicationType = NULLCOMMUNICATION;
        }
        return true;
    }

    std::string Encryption(const std::string& param) {
        std::string str_return;

        unsigned int len = param.length();
        for (unsigned int i = 0; i < len; ++i) {
            str_return += param[i] + 3;
            //str_return += (~(~(param[i] ^ 0xff) + i));
        }
        return str_return;
    }

    std::string Decryption(const std::string& param) {
        std::string str_return;

        unsigned int len = param.length();
        for (unsigned int i = 0; i < len; ++i) {
            str_return += param[i] - 3;
            //str_return += (~(~param[i] - i) ^ 0xff);
        }
        return str_return;
    }

    bool VerifyCode(const std::string& code, const std::string& code_verify) {
        std::string temp1(code), temp2(code_verify);
        ToLow(temp1), ToLow(temp2);
        if (temp1.compare(temp2) == 0) {
            return true;
        }
        else {
            return false;
        }
    }

    void ToLow(std::string& code) {
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

    std::string CombineString(char** beCombined, const int size) {
        std::string s_return("");
        for (int i = 0; i < size; ++i) {
            s_return += beCombined[i];
            s_return += "|";
        }
        return s_return.substr(0, s_return.length() - 1);
    }

    void SplitString(const std::string& be_converted, const char separator, std::vector<std::string>& dest) {
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

    void SplitString(const char* beConverted, const char separator, char** dest, int& size) {
        int pos = -1, old_pos = 0, i = 0;
        std::string temp(beConverted);
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
}
