#ifndef CACHE_SERIALIZER_H
#define CACHE_SERIALIZER_H
#include "resp.h"

std::string serialize(const RespValue&);

std::string serializeSimpleString(const std::string&);

std::string serializeError(const std::string&);

std::string serializeInteger(long long);

std::string serializeBulkString(const std::optional<std::string>&);

std::string serializeArray(const std::optional<std::vector<RespValue>>&);

#endif //CACHE_SERIALIZER_H