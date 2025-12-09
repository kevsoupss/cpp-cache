#include "parser.h"
#include <iostream>
#include <variant>
#include <optional>
#include <vector>
#include <string>
#include <stdexcept>

// Helper function to print RespValue recursively
void printRespValue(const RespValue& val, int indent = 0) {
    std::string pad(indent, ' ');
    switch (val.respType) {
        case RespType::SIMPLE_STRING:
            std::cout << pad << "Simple string: "
                      << std::get<std::string>(val.value) << "\n";
            break;
        case RespType::ERROR:
            std::cout << pad << "Error: "
                      << std::get<std::string>(val.value) << "\n";
            break;
        case RespType::INTEGER:
            std::cout << pad << "Integer: "
                      << std::get<long long>(val.value) << "\n";
            break;
        case RespType::BULK_STRING: {
            auto opt = std::get<std::optional<std::string>>(val.value);
            if (opt) std::cout << pad << "Bulk string: " << *opt << "\n";
            else std::cout << pad << "Bulk string: null\n";
            break;
        }
        case RespType::ARRAY: {
            auto arrOpt = std::get<std::optional<std::vector<RespValue>>>(val.value);
            if (!arrOpt) {
                std::cout << pad << "Array: null\n";
            } else {
                std::cout << pad << "Array[" << arrOpt->size() << "]:\n";
                for (const auto& elem : *arrOpt) {
                    printRespValue(elem, indent + 2);
                }
            }
            break;
        }
    }
}

int main() {
    std::vector<std::string> testInputs = {
        "+OK\r\n",
        "-ERROR message\r\n",
        ":123\r\n",
        "$5\r\nhello\r\n",
        "$0\r\n\r\n",
        "$-1\r\n",
        "*2\r\n+foo\r\n:42\r\n",
        "*3\r\n$3\r\nfoo\r\n$-1\r\n:7\r\n",
        "*-1\r\n",
        "*2\r\n*2\r\n:1\r\n:2\r\n*0\r\n"
    };

    for (const auto& input : testInputs) {
        size_t pos = 0;
        std::cout << "Parsing input: " << input << "\n";
        try {
            RespValue val = parseValue(input, pos);
            printRespValue(val);
            std::cout << "Finished parsing, final pos: " << pos << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Parse error: " << e.what() << "\n";
        }
        std::cout << "-------------------------\n";
    }

    return 0;
}
