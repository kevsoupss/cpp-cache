//
// Created by kevin on 12/11/2025.
//

#ifndef CACHE_HANDLER_H
#define CACHE_HANDLER_H
#include <functional>

#include "resp.h"


class Handler {
public:
    static RespValue handler(const RespValue& req);

private:

    using CommandFunction = std::function<RespValue(const std::vector<RespValue> &array)>;
    static std::unordered_map<std::string, CommandFunction> commandMap_;
    static std::unordered_map<std::string, RespValue> dataStore_;

    static RespValue routeCommand(std::string &commandName, const std::vector<RespValue> &array);
    static RespValue handleGet(const std::vector<RespValue> &array);
    static RespValue handleSet(const std::vector<RespValue> &array);
};


#endif //CACHE_HANDLER_H