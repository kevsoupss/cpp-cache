#include <iostream>

#include "cache.h"

int main() {
    Cache cache {2};

    std::cout<< cache.size() << '\n';
    cache.insert("testkey", "testval");
    std::cout << std::boolalpha;
    std::cout << cache.exists("testkey") << cache.exists("fakekey") << '\n';
}
