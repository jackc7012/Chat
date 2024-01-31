#ifndef __JSON_H__
#define __JSON_H__

#include <string>
#include <sstream>
#include <unordered_map>

namespace cwy {
    const char CHAR_COMMA = ',';
    const char CHAR_COLON = ':';
    const char CHAR_QUOTATION = '\"';

    class Json {
    public:
        Json();
        ~Json();
        Json(const Json&) = delete;
        Json(Json&&) = delete;
        Json& operator = (const Json&) = delete;
        Json& operator = (Json&&) = delete;

        std::string& operator [](const std::string& key);

    public:
        bool Parse(const std::string& src);
        void Write(std::stringstream& out);
        bool IsMember(const std::string& key);
        void Clear();

    private:
        std::unordered_map<std::string, std::string> jsonValue_;
    };
}

#endif // !__JSON_H__