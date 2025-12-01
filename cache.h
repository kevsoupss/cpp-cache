#ifndef CACHE_H_
#define CACHE_H_

#include <list>
#include <string>
#include <unordered_map>

class Cache {
public:
    Cache(int capacity);

    void Insert(const std::string& key, const std::string& value);
    bool Exists(const std::string& key) const;
    size_t Size() const;

private:
    void Erase(std::list<std::string>::iterator it);
    void Evict();

    size_t capacity_;
    std::list<std::string> key_order_;
    std::unordered_map<
        std::string,
        std::pair<std::string, std::list<std::string>::iterator>
    > internal_cache_;
};

#endif  // CACHE_H_
