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
    using pair_type = std::pair<Key, Value>;
    using key_type = Key;
    using value_type = Value;

    flatmap(){}

    template <typename InputIterator>
    flatmap(InputIterator first, InputIterator last)
    :data{first, last}{
        std::sort(std::begin(data), std::end(data));
    }
    flatmap(std::initializer_list<pair_type> elems)
    :flatmap(std::begin(elems), std::end(elems)){}

    std::pair<iterator, bool> insert(const pair_type& elem){
        //find + insert
        auto it = find(elem.first);
        if (it != end(data)){
            return {it, false};
        } else {
            return {insert(it, elem), true};
        }
    }

    iterator insert(iterator position, const pair_type& elem){
        data.insert(position, elem);
    }

    iterator find(const key_type& key){
        auto it = std::lower_bound(begin(data), end(data), key);
        if (it != end(data) && *it == key){
            return it;
        } else {
            return end(data);
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
    std::vector<pair_type> data;
};

#endif
