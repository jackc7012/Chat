#include "CJson.h"

#include <sstream>

namespace cwy {
CJson::CJson(const std::string& src)
{
    parse(src);
}

void CJson::parse(const std::string& src)
{
    if (!CheckValid(src)) {
        return;
    }
    size_t i = 0;
    while (i < src.length()) {
        std::string key, value;
        // find key
        int beginQut = src.find('\"', i);
        int endQut = src.find('\"', beginQut + 1);
        if (beginQut != std::string::npos && endQut != std::string::npos) {
            key = src.substr(beginQut + 1, endQut - beginQut - 1);
        }
        // find value
        beginQut = src.find('\"', endQut + 1);
        endQut = src.find('\"', beginQut + 1);
        if (beginQut != std::string::npos && endQut != std::string::npos) {
            value = src.substr(beginQut + 1, endQut - beginQut - 1);
        }
        jsonValue.insert(std::make_pair(key, value));
        i = endQut + 1;
    }
}

bool CJson::isMember(const std::string& key) const
{
    return (jsonValue.find(key) != jsonValue.end());
}

std::string CJson::get(const std::string& key) const
{
    std::string result = "";
    auto itor = jsonValue.find(key);
    if (itor != jsonValue.end()) {
        result = itor->second;
    }
    return result;
}

void CJson::set(const std::string& key, const std::string& value)
{
    jsonValue.insert(std::make_pair(key, value));
}

void CJson::set(const std::string& key, const unsigned long long value)
{
    jsonValue.insert(std::make_pair(key, std::to_string(value)));
}

std::string CJson::write() const
{
    std::ostringstream ss("{");
    auto itor = jsonValue.cbegin();
    for (unsigned int i = 0; i < jsonValue.size(); ++i) {
        std::string key = itor->first;
        std::string value = itor->second;
        ss << "\"" << key << "\":\"" << value << "\"";
        if (i != (jsonValue.size() - 1)) {
            ss << ",";
        }
        ++itor;
    }
    ss << "}";
    return ss.str();
}

bool CJson::CheckValid(const std::string& src)
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

