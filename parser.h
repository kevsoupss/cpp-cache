#ifndef CACHE_PARSER_H
#define CACHE_PARSER_H
#include <stdexcept>

#include "resp.h"

RespValue parseValue(const std::string&, size_t&);

RespValue parseSimpleString(const std::string&, size_t&);

RespValue parseError(const std::string&, size_t&);

RespValue parseInteger(const std::string&, size_t&);

RespValue parseBulkString(const std::string&, size_t&);

RespValue parseArray(const std::string&, size_t&);


#endif //CACHE_PARSER_H