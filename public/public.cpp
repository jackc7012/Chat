#include "public.h"

std::string Encryption(const std::string& pwd)
{
    const static std::string src = "qmpa2wlz7sg5oef3bngk1yt8ud6cfg0hj";
    const static unsigned int srcLength = src.length();
    std::string str_return;

    unsigned int len = pwd.length();
    for (unsigned int i = 0; i < len; ++i)
    {
        str_return += src.at((srcLength - i - 1) % srcLength);
        str_return += pwd.at(i);
        str_return += src.at(i % srcLength);
        str_return += pwd.at(len - i - 1);
    }
    return str_return;
}

std::string Decryption(const std::string& pwd)
{
    std::string str_return;

    unsigned int len = pwd.length();
    if (len % 4 != 0)
    {
        return "";
    }
    for (unsigned int i = 1; i < len; i += 4)
    {
        str_return += pwd.at(i);
    }
    return str_return;
}

bool VerifyCode(const std::string& code, const std::string& code_verify)
{
    std::string temp1(code), temp2(code_verify);
    ToLow(temp1), ToLow(temp2);
    if (temp1.compare(temp2) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ToLow(std::string& code)
{
    int len = code.length();
    std::string temp("");
    char low = 0;
    for (int i = 0; i < len; ++i)
    {
        low = tolower(code[i]);
        temp += (low);
    }
    code = temp;

    return;
}

std::string CombineString(const DataRecords& dataRecords)
{
    std::string result;
    for (size_t i = 0; i < dataRecords.size(); ++i)
    {
        DataRecordLine dataRecordLine = dataRecords.at(i);
        for (size_t j = 0; j < dataRecordLine.size(); ++j)
        {
            result += dataRecordLine.at(j);
            if (j != dataRecordLine.size() - 1)
            {
                result += ":";
            }
        }
        result += "|";
    }
    return result;
}

void SplitString(const std::string& be_converted, const char separator, std::vector<std::string>& dest)
{
    int pos = -1, old_pos = -1;
    pos = be_converted.find(separator, pos + 1);
    while (pos != be_converted.npos)
    {
        std::string temp(trim(be_converted.substr(old_pos + 1, pos - old_pos - 1)));
        dest.emplace_back(temp);
        old_pos = pos;
        pos = be_converted.find(separator, pos + 1);
    }
    std::string temp(trim(be_converted.substr(old_pos + 1)));
    dest.emplace_back(temp);

    return;
}

void SplitString(const char* be_converted, const char separator, char** dest, int& size)
{
    int pos = -1, old_pos = 0, i = 0;
    std::string temp(be_converted);
    pos = temp.find(separator, pos + 1);
    if (pos == temp.npos)
    {
        dest[0] = new char[temp.length() + 1];
        memset(dest[0], 0, temp.length() + 1);
        memcpy_s(dest[0], temp.length() + 1, temp.c_str(), temp.length() + 1);
        size = 1;
        return;
    }
    while (pos != temp.npos)
    {
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

std::string GetSystemTime(const int mode/* = 0*/)
{
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    char res[50] = { 0 };
    if (mode == 0)
    {
        sprintf_s(res, 50, "%04d.%02d.%02d %02d:%02d:%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute,
            sysTime.wSecond);
    }
    else if (mode == 1)
    {
        sprintf_s(res, 50, "%04d.%02d.%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay);
    }
    return std::string(res);
}

std::string toDbString(const std::string& src)
{
    std::ostringstream result;
    if (checkValid(src))
    {
        result << "'" << src << "'";
    }
    return result.str();
}

std::string dbJoin(const std::vector<long long>& srcList)
{
    std::string result("(");
    for (const auto& srcStr : srcList)
    {
        std::string src = std::to_string(srcStr);
        if (checkValid(src))
        {
            result += "'" + src + "', ";
        }
    }
    result += ")";
    return result;
}

std::string dbJoin(const std::vector<std::string>& srcList)
{
    std::string result("(");
    for (const auto& src : srcList)
    {
        if (checkValid(src))
        {
            result += "'" + src + "', ";
        }
    }
    result = result.substr(0, result.length() - 2);
    result += ")";
    return result;
}

bool checkValid(const std::string& src)
{
    std::string key[] = { "and", "*", "=", "%", "/", "union", "|", "&", "^" ,"#", "/*", "*/" };
    for (int i = 0; i < 13; i++)
    {
        if (src.find(key[i]) != std::string::npos)
        {
            return false;
        }
    }
    return true;
}

bool checkSqlValid(const std::string& src)
{
    std::string key[] = { "%", "/", "union", "|", "&", "^", "#", "/*", "*/" };
    for (int i = 0; i < 9; i++)
    {
        if (src.find(key[i]) != std::string::npos)
        {
            return false;
        }
    }
    return true;
}

std::string trim(const std::string& src)
{
    std::string temp(src);
    if (!temp.empty())
    {
        temp.erase(0, temp.find_first_not_of(" "));
        temp.erase(temp.find_last_not_of(" ") + 1);
    }
    return temp;
}
