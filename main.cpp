#include <iostream>
#include "cache.h"

int main() {
    Cache cache(2);

    std::cout << "Initial cache size: " << cache.Size() << "\n";

    cache.Insert("testkey", "testval");
    std::cout << "Inserted 'testkey'. Size: " << cache.Size() << "\n";

    std::cout << std::boolalpha;
    std::cout << "'testkey' exists: " << cache.Exists("testkey") << "\n";
    std::cout << "'fakekey' exists: " << cache.Exists("fakekey") << "\n";

    cache.Insert("anotherkey", "anotherval");
    std::cout << "Inserted 'anotherkey'. Size: " << cache.Size() << "\n";

    cache.Insert("thirdkey", "thirdval");
    std::cout << "Inserted 'thirdkey'. Size: " << cache.Size() << "\n";

    std::cout << "'testkey' exists after eviction: " << cache.Exists("testkey") << "\n";
    std::cout << "'anotherkey' exists: " << cache.Exists("anotherkey") << "\n";
    std::cout << "'thirdkey' exists: " << cache.Exists("thirdkey") << "\n";

    return 0;
}
