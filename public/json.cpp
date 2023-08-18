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

    if (trimSrc.empty() || (trimSrc.find("communication_type") == std::string::npos))
    {
        return false;
    }

    size_t pos1 = trimSrc.find_first_of('{');
    size_t pos2 = trimSrc.find_last_of('}');
    trimSrc = trimSrc.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string key, value;
    while (!trimSrc.empty())
    {
        // find key
        int keyPos1 = trimSrc.find('\"');
        int keyPos2 = trimSrc.find('\"', keyPos1 + 1);
        key = trimSrc.substr(keyPos1 + 1, keyPos2 - keyPos1 - 1);

        // find value
        int valuePos1 = trimSrc.find('\"', keyPos2 + 1);
        int valuePos2 = trimSrc.find('\"', valuePos1 + 1);
        value = trimSrc.substr(valuePos1 + 1, valuePos2 - valuePos1 - 1);

        jsonValue_.insert(std::make_pair(key, value));

        trimSrc = trimSrc.substr(valuePos2 + 1);
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
