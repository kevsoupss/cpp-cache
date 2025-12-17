//
// Created by kevin on 12/11/2025.
//

#include "handler.h"
#include "resp.h"
#include <algorithm>
#include <iostream>

// Command Mapper
std::unordered_map<std::string, Handler::CommandFunction> Handler::commandMap_ = {
    {"GET", &Handler::handleGet},
    {"SET", &Handler::handleSet},
};

std::unordered_map<std::string, RespValue> Handler::dataStore_ {};

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

            return routeCommand(commandName, array);
        } else {
            return RespValue::makeProtocolError("Array is null or empty.");
        }
    } else {
        return RespValue::makeProtocolError("Internal error: Array value misconfigured.");
    }
}

RespValue Handler::routeCommand(std::string &commandName, const std::vector<RespValue> &array) {
    auto it = commandMap_.find(commandName);

    if (it != commandMap_.end()) {
        return it->second(array);
    } else {
        return RespValue::makeProtocolError(commandName + " does not exist");
    }
}

RespValue Handler::handleGet(const std::vector<RespValue> &array) {
    if (array.size() < 2) {
        return RespValue::makeProtocolError("Wrong number of arguments for 'GET' command");
    }
    const std::string& key = array[1].getString();

    auto it = dataStore_.find(array[1].getString());

    if (it != dataStore_.end()) {
        return it->second;
    } else {
        return RespValue::makeNullBulkString();
    }
}

RespValue Handler::handleSet(const std::vector<RespValue> &array) {
    if (array.size() < 3) {
        return RespValue::makeProtocolError("Wrong number of arguments for 'SET' command");
    }
    const std::string& key = array[1].getString();
    const RespValue& value = array[2];
    dataStore_[key] = value;
    return RespValue::makeSimpleString("OK");
}
