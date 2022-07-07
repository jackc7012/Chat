#ifndef __MY_JSON_H__
#define __MY_JSON_H__

#include <string>
#include <unordered_map>

namespace cwy {
class CJson {
public:
    CJson() = default;

    CJson(const std::string& src);

    void parse(const std::string& src);

    bool isMember(const std::string& key) const;

    std::string get(const std::string& key) const;

    void set(const std::string& key, const std::string& value);

    void set(const std::string& key, const unsigned long long value);

    std::string write() const;
private:
    CJson(const CJson&) = delete;
    CJson(CJson&&) = delete;
    CJson& operator=(const CJson&) = delete;
    CJson& operator=(CJson&&) = delete;

    bool CheckValid(const std::string& src);

private:
    std::unordered_map<std::string, std::string> jsonValue;
};
}
#endif

