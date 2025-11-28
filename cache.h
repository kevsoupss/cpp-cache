#ifndef CACHE_CACHE_H
#define CACHE_CACHE_H
#include <string>
#include <unordered_map>


class Cache {
private:
    int capacity{};
    std::unordered_map<std::string, std::string> internal_cache;

public:
    Cache(int capacity);
    void insert(const std::string& key, const std::string& value);
    void erase(const std::string& key);
    bool exists(const std::string& key);
    std::unordered_map<std::string,std::string>::size_type size();
};


#endif //CACHE_CACHE_H