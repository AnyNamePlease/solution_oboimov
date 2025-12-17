#pragma once
#include "ICache.h"
#include <list>
#include <unordered_map>
#include <stdexcept>

template<typename Key, typename Value>
class ExampleCache : public ICache<Key, Value> {
public:
    explicit ExampleCache(std::size_t capacity) : capacity_(capacity) {
        if (capacity_ == 0) {
            throw std::invalid_argument("Capacity must be positive");
        }
    }

    void set(const Key& key, const Value& value) override {
        auto it = cache_map_.find(key);
        if (it != cache_map_.end()) {
            // Обновляем значение и перемещаем в начало
            it->second->second = value;
            cache_list_.splice(cache_list_.begin(), cache_list_, it->second);
        } else {
            // Нужно вставить новый элемент
            if (cache_list_.size() >= capacity_) {
                // Удаляем самый старый (последний в списке)
                auto last = cache_list_.end();
                --last;
                cache_map_.erase(last->first);
                cache_list_.pop_back();
            }
            cache_list_.emplace_front(key, value);
            cache_map_[key] = cache_list_.begin();
        }
    }

    Value get(const Key& key) override {
        auto it = cache_map_.find(key);
        if (it != cache_map_.end()) {
            // Делаем элемент "недавно использованным"
            cache_list_.splice(cache_list_.begin(), cache_list_, it->second);
            return it->second->second;
        }
        return Value{}; // возвращаем дефолтное значение, как требует интерфейс
    }

    bool contains(const Key& key) const override {
        return cache_map_.find(key) != cache_map_.end();
    }

    void clear() override {
        cache_map_.clear();
        cache_list_.clear();
    }

    std::size_t size() const override {
        return cache_list_.size();
    }

private:
    using List = std::list<std::pair<Key, Value>>;
    std::size_t capacity_;
    List cache_list_;
    std::unordered_map<Key, typename List::iterator> cache_map_;
};
