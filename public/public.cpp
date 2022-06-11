#include "public.h"

#include <sstream>

namespace cwy {
void UnregisterSpace(CommunicationType type, s_HandleRecv& field)
{
    try {
        switch (type) {
            case CommunicationType::REGISTER: {
                UnregisterSingleSpace(&field.Param.register_.customer);
                UnregisterSingleSpace(&field.Param.register_.password);
            }
            break;

            case CommunicationType::REGISTERBACKSUCCEED: {
                UnregisterSingleSpace(&field.Param.registerBack_.register_result);
            }
            break;

            case CommunicationType::REGISTERBACKFAILED: {
                UnregisterSingleSpace(&field.Param.registerBack_.register_result);
                UnregisterSingleSpace(&field.Param.registerBack_.description);
            }
            break;

            case CommunicationType::LOGIN: {
                UnregisterSingleSpace(&field.Param.login_.password);
            }
            break;

            case CommunicationType::LOGINBACKSUCCEED: {
                UnregisterSingleSpace(&field.Param.loginBack_.customer);
                UnregisterSingleSpace(&field.Param.loginBack_.login_result);
            }
            break;

            case CommunicationType::LOGINBACKFAILED: {
                UnregisterSingleSpace(&field.Param.loginBack_.customer);
                UnregisterSingleSpace(&field.Param.loginBack_.login_result);
                UnregisterSingleSpace(&field.Param.loginBack_.description);
            }
            break;

            case CommunicationType::LOGINBOARDCAST: {
                UnregisterSingleSpace(&field.Param.loginBoardcast_.customer);
            }
            break;

            case CommunicationType::DELETECUSTOMER: {
                UnregisterSingleSpace(&field.Param.delCustomer_.customer);
            }
            break;

            case CommunicationType::CHAT: {
                UnregisterSingleSpace(&field.Param.chat_.source);
                UnregisterSingleSpace(&field.Param.chat_.target);
                UnregisterSingleSpace(&field.Param.chat_.content);
            }
            break;

            case CommunicationType::TRANSFERFILEREQUEST: {
                UnregisterSingleSpace(&field.Param.transferFileRequest_.source);
                UnregisterSingleSpace(&field.Param.transferFileRequest_.target);
                UnregisterSingleSpace(&field.Param.transferFileRequest_.file_name);
            }
            break;

            case CommunicationType::TRANSFERFILERESPOND: {
                UnregisterSingleSpace(&field.Param.transferFileRespond_.source);
                UnregisterSingleSpace(&field.Param.transferFileRespond_.target);
                UnregisterSingleSpace(&field.Param.transferFileRespond_.file_name);
                UnregisterSingleSpace(&field.Param.transferFileRespond_.transfer_result);
            }
            break;

            case CommunicationType::TRANSFERFILE: {
                UnregisterSingleSpace(&field.Param.transferFile_.source);
                UnregisterSingleSpace(&field.Param.transferFile_.target);
                UnregisterSingleSpace(&field.Param.transferFile_.file_name);
                UnregisterSingleSpace(&field.Param.transferFile_.file_content);
            }
            break;
        }
    }
    catch (...) {
        return;
    }
}

std::string EncodeJson(const CommunicationType type, const s_HandleRecv& s_param)
{
    CJson json;
    try {
        switch (type) {
            case CommunicationType::REGISTER: {
                json.set("communication_type", "register");
                json.set("customer", s_param.Param.register_.customer);
                json.set("password", s_param.Param.register_.password);
            }
            break;

            case CommunicationType::REGISTERBACKSUCCEED: {
                json.set("communication_type", "registerbacksucceed");
                json.set("id", std::to_string(s_param.Param.registerBack_.id).c_str());
                json.set("register_result", s_param.Param.registerBack_.register_result);
            }
            break;

            case CommunicationType::REGISTERBACKFAILED: {
                json.set("communication_type", "registerbackfailed");
                json.set("id", std::to_string(s_param.Param.registerBack_.id).c_str());
                json.set("register_result", s_param.Param.registerBack_.register_result);
                json.set("description", s_param.Param.registerBack_.description);
            }
            break;

            case CommunicationType::LOGIN: {
                json.set("communication_type", "login");
                json.set("id", std::to_string(s_param.Param.login_.id).c_str());
                json.set("password", s_param.Param.login_.password);
            }
            break;

            case CommunicationType::LOGINBACKSUCCEED: {
                json.set("communication_type", "loginbacksucceed");
                json.set("customer", s_param.Param.loginBack_.customer);
                json.set("login_result", s_param.Param.loginBack_.login_result);
            }
            break;

            case CommunicationType::LOGINBACKFAILED: {
                json.set("communication_type", "loginbackfailed");
                json.set("customer", "");
                json.set("login_result", s_param.Param.loginBack_.login_result);
                json.set("description", s_param.Param.loginBack_.description);
            }
            break;

            case CommunicationType::LOGINBOARDCAST: {
                json.set("communication_type", "loginboardcast");
                json.set("customer", s_param.Param.loginBoardcast_.customer);
                json.set("first_login", std::to_string(s_param.Param.loginBoardcast_.flag).c_str());
            }
            break;

            case CommunicationType::DELETECUSTOMER: {
                json.set("communication_type", "deletecustomer");
                json.set("customer", s_param.Param.delCustomer_.customer);
            }
            break;

            case CommunicationType::CHAT: {
                json.set("communication_type", "chat");
                json.set("content", s_param.Param.chat_.content);
                json.set("target", s_param.Param.chat_.target);
                json.set("source", s_param.Param.chat_.source);
            }
            break;

            case CommunicationType::TRANSFERFILEREQUEST: {
                json.set("communication_type", "transferfilerequest");
                json.set("file_name", s_param.Param.transferFileRequest_.file_name);
                json.set("target", s_param.Param.transferFileRequest_.target);
                json.set("source", s_param.Param.transferFileRequest_.source);
                json.set("source", s_param.Param.transferFileRequest_.file_length);
            }
            break;

            case CommunicationType::TRANSFERFILERESPOND: {
                json.set("communication_type", "transferfilerespond");
                json.set("file_name", s_param.Param.transferFileRespond_.file_name);
                json.set("target", s_param.Param.transferFileRespond_.target);
                json.set("source", s_param.Param.transferFileRespond_.source);
                json.set("transfer_result", s_param.Param.transferFileRespond_.transfer_result);
            }
            break;

            case CommunicationType::TRANSFERFILE: {
                json.set("communication_type", "transferfile");
                json.set("file_name", s_param.Param.transferFile_.file_name);
                json.set("file_content", s_param.Param.transferFile_.file_content);
                json.set("file_block", s_param.Param.transferFile_.file_block);
                json.set("current_block", s_param.Param.transferFile_.current_block);
                json.set("target", s_param.Param.transferFile_.target);
                json.set("source", s_param.Param.transferFile_.source);
            }
            break;
        }
    }
    catch (...) {
        return "";
    }

    std::string result = json.write();

    return result;
}

bool DecodeJson(const std::string& value, s_HandleRecv& s_return)
{
    try {
        CJson json(value);

        if (json.get("communication_type") == "register") { // register
            std::string customer = json.get("customer"), password = json.get("password");
            RegisterSpace(&s_return.Param.register_.customer, customer);
            RegisterSpace(&s_return.Param.register_.password, password);
            s_return.type_ = CommunicationType::REGISTER;

        } else if (json.get("communication_type") == "registerbacksucceed") { // register_succeed
            std::string id = json.get("id"), register_result = json.get("register_result");
            s_return.Param.registerBack_.id = static_cast<unsigned long long>(atoll(id.c_str()));
            RegisterSpace(&s_return.Param.registerBack_.register_result, register_result);
            s_return.type_ = CommunicationType::REGISTERBACKSUCCEED;

        } else if (json.get("communication_type") == "registerbackfailed") { // register_failed
            std::string id = json.get("id"), register_result = json.get("register_result"),
                description = json.get("description");
            s_return.Param.registerBack_.id = 0;
            RegisterSpace(&s_return.Param.registerBack_.register_result, register_result);
            RegisterSpace(&s_return.Param.registerBack_.description, description);
            s_return.type_ = CommunicationType::REGISTERBACKFAILED;

        } else if (json.get("communication_type") == "login") { // login
            std::string id = json.get("id"), password = json.get("password");
            s_return.Param.login_.id = static_cast<unsigned long long>(atoll(id.c_str()));
            RegisterSpace(&s_return.Param.login_.password, password);
            s_return.type_ = CommunicationType::LOGIN;

        } else if (json.get("communication_type") == "loginbacksucceed") { // login_succeed
            std::string customer = json.get("customer"), login_result = json.get("login_result");
            RegisterSpace(&s_return.Param.loginBack_.customer, customer);
            RegisterSpace(&s_return.Param.loginBack_.login_result, login_result);
            s_return.type_ = CommunicationType::LOGINBACKSUCCEED;

        } else if (json.get("communication_type") == "loginbackfailed") { // login_failed
            std::string customer = json.get("customer"), login_result = json.get("login_result"),
                description = json.get("description");
            RegisterSpace(&s_return.Param.loginBack_.customer, customer);
            RegisterSpace(&s_return.Param.loginBack_.login_result, login_result);
            RegisterSpace(&s_return.Param.loginBack_.description, description);
            s_return.type_ = CommunicationType::LOGINBACKFAILED;

        } else if (json.get("communication_type") == "loginboardcast") { // show_login
            std::string customer = json.get("customer");
            RegisterSpace(&s_return.Param.loginBoardcast_.customer, customer);
            s_return.Param.loginBoardcast_.flag = ((json.get("first_login") == "0") ? true : false);
            s_return.type_ = CommunicationType::LOGINBOARDCAST;

        } else if (json.get("communication_type") == "delcustomer") { // delete_customer
            std::string customer = json.get("customer");
            RegisterSpace(&s_return.Param.delCustomer_.customer, customer);
            s_return.type_ = CommunicationType::DELETECUSTOMER;

        } else if (json.get("communication_type") == "chat") { // chat
            std::string content = json.get("content"), source = json.get("source"), target = json.get("target");
            RegisterSpace(&s_return.Param.chat_.content, content);
            RegisterSpace(&s_return.Param.chat_.source, source);
            RegisterSpace(&s_return.Param.chat_.target, target);
            s_return.type_ = CommunicationType::CHAT;

        } else if (json.get("communication_type") == "transferfilerequest") { // transfer_filequest
            std::string file_name = json.get("file_name"), source = json.get("source"),
                target = json.get("target"), file_length = json.get("file_length");
            RegisterSpace(&s_return.Param.transferFileRequest_.source, source);
            RegisterSpace(&s_return.Param.transferFileRequest_.target, target);
            RegisterSpace(&s_return.Param.transferFileRequest_.file_name, file_name);
            s_return.Param.transferFileRequest_.file_length = static_cast<unsigned long long>(atoll(file_length.c_str()));
            s_return.type_ = CommunicationType::TRANSFERFILEREQUEST;

        } else if (json.get("communication_type") == "transferfilerespond") { // transfer_filerespond
            std::string file_name = json.get("file_name"), source = json.get("source"),
                target = json.get("target"), transfer_result = json.get("transfer_result");
            RegisterSpace(&s_return.Param.transferFileRespond_.source, source);
            RegisterSpace(&s_return.Param.transferFileRespond_.target, target);
            RegisterSpace(&s_return.Param.transferFileRespond_.file_name, file_name);
            RegisterSpace(&s_return.Param.transferFileRespond_.transfer_result, transfer_result);
            s_return.type_ = CommunicationType::TRANSFERFILERESPOND;

        } else if (json.get("communication_type") == "transferfile") { // transfer_file
            std::string file_name = json.get("file_name"), source = json.get("source"),
                target = json.get("target"), file_content = json.get("file_content"),
                file_block = json.get("file_block"), current_block = json.get("current_block");
            s_return.Param.transferFile_.file_block = static_cast<unsigned int>(atoi(file_block.c_str()));
            s_return.Param.transferFile_.current_block = static_cast<unsigned int>(atoi(current_block.c_str()));
            RegisterSpace(&s_return.Param.transferFile_.source, source);
            RegisterSpace(&s_return.Param.transferFile_.target, target);
            RegisterSpace(&s_return.Param.transferFile_.file_name, file_name);
            RegisterSpace(&s_return.Param.transferFile_.file_content, file_content);
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

std::string Encryption(const std::string& param)
{
    std::string str_return;

    unsigned int len = param.length();
    for (unsigned int i = 0; i < len; ++i) {
        str_return += (~(~(param[i] ^ 0xff) + i));
    }
    return str_return;
}

std::string Decryption(const std::string& param)
{
    std::string str_return;

    unsigned int len = param.length();
    for (unsigned int i = 0; i < len; ++i) {
        str_return += (~(~param[i] - i) ^ 0xff);
    }
    return str_return;
}

bool VerifyCode(const std::string& code, const std::string& code_verify)
{
    std::string temp1(code), temp2(code_verify);
    cwy::ToLow(temp1), cwy::ToLow(temp2);
    if (temp1.compare(temp2) == 0) {
        return true;
    } else {
        return false;
    }
}

bool VerifyPassWord(const char* password, const unsigned int len)
{
    unsigned int smChar = 0, biChar = 0, number = 0;
    bool flag = false;
    for (unsigned int i = 0; i < len; ++i) {
        if ((password[i] >= 'a') && (password[i] <= 'z')) {
            if (biChar == 0 || number == 0) {
                ++smChar;
            } else {
                flag = true;
                break;
            }
        } else if ((password[i] >= 'A') && (password[i] <= 'Z')) {
            if (smChar == 0 || number == 0) {
                ++biChar;
            } else {
                flag = true;
                break;
            }
        } else if ((password[i] >= '0') && (password[i] <= '9')) {
            if (smChar == 0 || biChar == 0) {
                ++number;
            } else {
                flag = true;
                break;
            }
        }
    }
    if (!flag && ((smChar != 0) && (biChar != 0) && (number != 0))) {
        flag = true;
    }
    return flag;
}

void ToLow(std::string& code)
{
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

std::string CombineString(char** be_combined, const int size)
{
    std::string s_return("");
    for (int i = 0; i < size; ++i) {
        s_return += be_combined[i];
        s_return += "|";
    }
    return s_return.substr(0, s_return.length() - 1);
}

void SplitString(const std::string& be_converted, const char separator, std::vector<std::string>& dest)
{
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

void SplitString(const char* be_converted, const char separator, char** dest, int& size)
{
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
    if (cwy::CheckSqlValid(src)) {
        result << "'" << src << "'";
    }
    return result.str();
}

std::string DbJoin(const std::vector<long long>& srcList)
{
    std::string result("(");
    for (const auto& srcStr : srcList) {
        std::string src = std::to_string(srcStr);
        if (cwy::CheckSqlValid(src)) {
            result += "'" + src + "', ";
        } else {
            return "";
        }
    }
    result += ")";
    return result;
}

std::string DbJoin(const std::vector<std::string>& srcList)
{
    std::string result("(");
    for (const auto& src : srcList) {
        if (cwy::CheckSqlValid(src)) {
            result += "'" + src + "', ";
        } else {
            return "";
        }
    }
    result = result.substr(0, result.length() - 2);
    result += ")";
    return result;
}

std::string DbJoin(const StringMap& stringMap, const IntMap& intMap, bool hasBrackets/* = true*/)
{
    std::string result;
    result += (hasBrackets ? "(" : "");
    auto itor1 = stringMap.cbegin();
    auto itor2 = intMap.cbegin();
    unsigned int nowCount = 0;
    for (;;) {
        if (itor1 != stringMap.cend() && itor1->first == nowCount) {
            if (cwy::CheckSqlValid(itor1->second)) {
                result += "'" + itor1->second + "'";
            } else {
                return "";
            }
            ++itor1;
        } else if (itor2 != intMap.cend() && itor2->first == nowCount) {
            std::string src = std::to_string(itor2->second);
            result += src;
            ++itor2;
        } else if (itor1 == stringMap.cend() && itor2 == intMap.cend()) {
            break;
        } else {
            return "";
        }
        if (nowCount != (stringMap.size() + intMap.size() - 1)) {
            result += ", ";
        }
        ++nowCount;
    }
    result += (hasBrackets ? ")" : "");
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
    std::string key[14] = {"and", "*", "=", " ", "%0a", "%", "/", "union", "|", "&", "^", "#", "/*", "*/"};
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
}
