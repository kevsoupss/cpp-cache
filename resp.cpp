#include "resp.h"

#include <stdexcept>

size_t RespValue::getArraySize() const {
    if (respType != RespType::ARRAY) {
        throw std::runtime_error("Cannot call getArraySize on non-ARRAY type.");
    }
    const auto& optional_vec = std::get<std::optional<std::vector<RespValue>>>(value);

    if (!optional_vec.has_value()) {
        return 0;
    }

    return optional_vec->size();
}

const RespValue& RespValue::getArrayElement(size_t index) const {
    if (respType != RespType::ARRAY) {
        throw std::runtime_error("Cannot call getArrayElement on non-ARRAY type.");
    }
    const auto& optional_vec = std::get<std::optional<std::vector<RespValue>>>(value);

    if (index >= getArraySize()) {
        throw std::runtime_error("Index too large.");
    }

    return optional_vec->at(index);
}

RespValue RespValue::makeProtocolError(const std::string& msg) {
    RespValue err;
    err.respType = RespType::ERR;
    err.value = std::string("ERR " + msg);
    return err;
}

