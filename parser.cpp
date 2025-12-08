#include "parser.h"

#include <stdexcept>

static std::string readLine(const std::string& input, size_t& pos) {
    const size_t startIndex = pos;
    while (pos < input.size()) {
        if (input[pos] == '\r' && pos + 1 < input.size() && input[pos + 1] == '\n') {
            std::string line = input.substr(startIndex, pos - startIndex);
            pos += 2;
            return line;
        }
        pos ++;
    }
    throw std::runtime_error("No CRLF found.");
}

RespValue parseValue(const std::string& input, size_t& pos) {
    char type = input.at(0);

    switch (type) {
        case '+':
            return parseSimpleString(input, pos);
        case '-':
            return parseError(input, pos);
        case ':':
            return parseInteger(input, pos);
        case '$':
            return parseBulkString(input, pos);
        case '*':
            return parseArray(input, pos);
        default:
            throw std::runtime_error("Invalid RESP type prefix.");

    }
}

RespValue parseSimpleString(const std::string& input, size_t& pos) {
    if (pos >= input.size() || input[pos] != '+')  {
        throw std::runtime_error("Expected '+' at position " + std::to_string(pos));
    }
    pos++;
    std::string lineContent = readLine(input, pos);
    RespValue simpleString(SIMPLE_STRING, lineContent);
    return simpleString;
}