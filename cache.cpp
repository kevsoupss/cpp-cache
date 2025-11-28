#include "cache.h"

Cache::Cache(int capacity) {
    this->capacity = capacity;
}

void Cache::insert(const std::string& key, const std::string& value) {
    internal_cache[key] = value;
}

void Cache::erase(const std::string& key) {
    internal_cache.erase(key);
}

bool Cache::exists(const std::string& key) {
    return internal_cache.contains(key);
}

std::unordered_map<std::string,std::string>::size_type Cache::size() {
    return internal_cache.size();
}

