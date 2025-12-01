#include "cache.h"
#include <iterator>

Cache::Cache(int capacity) : capacity_(capacity) {}

void Cache::Insert(const std::string& key, const std::string& value) {
    if (!Exists(key)) {
        if (Size() >= capacity_) {
            Evict();
        }
        key_order_.push_front(key);
        internal_cache_[key] = {value, key_order_.begin()};
    } else {
        // TODO: move key to front of key_order_
        internal_cache_[key].first = value;
    }
}

bool Cache::Exists(const std::string& key) const {
    // TODO: move key to front of key_order_
    return internal_cache_.contains(key);
}

size_t Cache::Size() const {
    return internal_cache_.size();
}

void Cache::Erase(std::list<std::string>::iterator it) {
    const std::string& key = *it;

    internal_cache_.erase(key);

    key_order_.erase(it);
}

void Cache::Evict() {
    if (key_order_.empty()) return;

    auto last_it = std::prev(key_order_.end());

    Erase(last_it);
}

