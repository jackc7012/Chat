#ifndef __MY_JSON_H__
#define __MY_JSON_H__

#include <string>
#include <sstream>
#include <unordered_map>

const char CHAR_COMMA = ',';
const char CHAR_COLON = ':';
const char CHAR_QUOTATION = '\"';

namespace cwy {
    class Json {
    public:
        Json();
        ~Json();
        Json(const Json&) = delete;
        Json(Json&&) = delete;
        Json& operator = (const Json&) = delete;
        Json& operator = (Json&&) = delete;

        std::string& operator [](const std::string& value);

    public:
        bool Parse(const std::string& src);
        void Write(std::stringstream& out);
        bool IsMember(const std::string& value);

    private:
        std::unordered_map<std::string, std::string> jsonValue_;
    };
}

#endif // !__MY_JSON_H__