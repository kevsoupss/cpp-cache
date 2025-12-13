//
// Created by kevin on 12/11/2025.
//

#include "handler.h"
#include "resp.h"
#include <algorithm>
#include <iostream>

RespValue Handler::handler(const RespValue &req) {
    if (req.respType != RespType::ARRAY) {
        return RespValue::makeProtocolError("Request must be an array of bulk strings.");
    }

    using ArrayOpt = std::optional<std::vector<RespValue>>;
    if (const ArrayOpt* opt_ptr = std::get_if<ArrayOpt>(&req.value)) {

        if (opt_ptr->has_value()) {

            const std::vector<RespValue> &array = opt_ptr->value();
            if (array.empty()) {
                return RespValue::makeProtocolError("Command array cannot be empty.");
            }

            const RespValue commandValue = array[0];
            if (commandValue.respType != RespType::BULK_STRING) {
                return RespValue::makeProtocolError("Command must be a bulk string.");
            }

            using StringOpt = std::optional<std::string>;
            const StringOpt* commandOptPtr = std::get_if<StringOpt>(&commandValue.value);

            if (!commandOptPtr || !commandOptPtr->has_value()) {
                return RespValue::makeProtocolError("Internal error: Command bulk string empty.");
            }

            std::string commandName = commandOptPtr->value();

            std::transform(commandName.begin(), commandName.end(), commandName.begin(),
                           [](unsigned char c){ return std::toupper(c); });

            if (commandName == "GET") {
                return handleGet(array[1]);
            }
        } else {
            return RespValue::makeProtocolError("Array is null or empty.");
        }
    } else {
        return RespValue::makeProtocolError("Internal error: Array value misconfigured.");
    }
    return RespValue::makeProtocolError("Internal error: Array value misconfigured.");
}

RespValue Handler::handleGet(const RespValue &value) {
    std::cout << "Handled";
    return value ;
}
