#pragma once
#ifndef WTF_FLATMAP_IMPLEMENTATION
#define WTF_FLATMAP_IMPLEMENTATION

#include <vector>
#include <utility>
#include <algorithm>

//We only sort by operator<, for implementation simplicity
template <typename Key, typename Value>
class flatmap {

public:

    using iterator = typename std::vector<std::pair<Key, Value>>::iterator;
    using const_iterator = typename std::vector<std::pair<Key, Value>>::const_iterator;
    using value_type = std::pair<Key, Value>;
    using mapped_type = Value;
    using key_type = Key;

    flatmap(){}

    template <typename InputIterator>
    flatmap(InputIterator first, InputIterator last)
    :data{first, last}{
        std::sort(std::begin(data), std::end(data));
    }
    flatmap(std::initializer_list<value_type> elems)
    :flatmap(std::begin(elems), std::end(elems)){}

    std::pair<iterator, bool> insert(const value_type& elem){
        //find + insert
        auto it = find(elem.first);
        if (it != std::end(data)){
            return {it, false};
        } else {
            return {insert(it, elem), true};
        }
    }

    iterator insert(iterator position, const value_type& elem){
        return data.insert(position, elem);
    }

    iterator find(const key_type& key){
        value_type key_dummy (key, mapped_type());
        auto it = std::lower_bound(std::begin(data), std::end(data), key_dummy);
        if (it != std::end(data) && it->first == key){
            return it;
        } else {
            return std::end(data);
        }
    }

    value_type& operator[](const key_type& key){
        auto it = find(key);
        if (it == end(data)){
            insert(key);
        }
    }

    const_iterator begin() const {
        return begin(data);
    }

    const_iterator end() const {
        return end(data);
    }

    iterator begin() {
        return begin(data);
    }

    iterator end() {
        return end(data);
    }

private:
    std::vector<value_type> data;
};

#endif
