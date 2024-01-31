#include "protocol.h"

using namespace cwy;

std::string HandleRecv::EncodeJson()
{
    std::stringstream str_json("");
    jsonHandle_.Write(str_json);

    return str_json.str();
}

bool HandleRecv::DecodeJson(const std::string& value)
{
    if (!jsonHandle_.Parse(value)) {
        return false;
    }

    type_ = typeStrToInt[jsonHandle_["communication_type"]];

    return true;
}
