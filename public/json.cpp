#include "json.h"
#include "public.h"
using namespace cwy;

Json::Json()
{
}

Json::~Json()
{
    jsonValue_.clear();
}

std::string& Json::operator[](const std::string& value)
{
    return jsonValue_[value];
}

bool Json::Parse(const std::string& src)
{
    std::string trimSrc = trim(src);
    size_t len = trimSrc.length();
    if (trimSrc.empty() || (trimSrc.at(0) != '{') || (trimSrc.at(len - 1) != '}')
        || (trimSrc.find("communication_type") == std::string::npos))
    {
        return false;
    }

    std::vector<std::string> jsonValueOne;
    SplitString(trimSrc.substr(1, trimSrc.length() - 2), CHAR_COMMA, jsonValueOne);
    for (size_t i = 0; i < jsonValueOne.size(); ++i)
    {
        if (!HandleJsonValue(jsonValueOne.at(i)))
        {
            return false;
        }
    }

    return true;
}

void Json::Write(std::stringstream& out)
{
    out << "{";
    size_t len = jsonValue_.size(), i = 1;
    for (auto itor = jsonValue_.begin(); itor != jsonValue_.end(); ++itor)
    {
        out << "\"" << itor->first << "\" : \"" << itor->second << "\"";
        if (i != len)
        {
            out << " ,";
        }
        ++i;
    }
    out << "}";
}

bool Json::IsMember(const std::string& value)
{
    auto itor = jsonValue_.find(value);
    return (itor != jsonValue_.end());
}

bool Json::HandleJsonValue(const std::string& src)
{
    std::vector<std::string> jsonValueMap;
    SplitString(src, CHAR_COLON, jsonValueMap);
    bool isTime = jsonValueMap.at(0).find("time") != std::string::npos;
    if ((jsonValueMap.size() != 2) && (isTime && (jsonValueMap.size() != 4)))
    {
        return false;
    }

    size_t lenValue0 = jsonValueMap.at(0).length();
    if ((jsonValueMap.at(0).at(0) != CHAR_QUOTATION) || (jsonValueMap.at(0).at(lenValue0 - 1) != CHAR_QUOTATION))
    {
        return false;
    }

    std::string mapValue(jsonValueMap.at(0).substr(1, lenValue0 - 2));
    size_t lenValue1 = jsonValueMap.at(1).length();
    if (isTime)
    {
        jsonValue_[mapValue] = (jsonValueMap.at(1).substr(1) + ":" + jsonValueMap.at(2) + ":" +
            jsonValueMap.at(3).substr(0, jsonValueMap.at(3).length() - 1));
    }
    else if ((jsonValueMap.at(1).at(0) == CHAR_QUOTATION) && (jsonValueMap.at(1).at(lenValue1 - 1) == CHAR_QUOTATION))
    {
        jsonValue_[mapValue] = jsonValueMap.at(1).substr(1, lenValue1 - 2);
    }
    else if ((jsonValueMap.at(1).at(0) != CHAR_QUOTATION) && (jsonValueMap.at(1).at(lenValue1 - 1) != CHAR_QUOTATION))
    {
        jsonValue_[mapValue] = jsonValueMap.at(1);
    }
    else
    {
        return false;
    }

    return true;
}