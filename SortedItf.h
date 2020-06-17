
#ifndef SORTED_SORTEDITF_H
#define SORTED_SORTEDITF_H

#include <vector>
#include <queue>
#include <set>
#include <map>
#include "Utils.h"

template<typename Impl>
class SortedInterface {
public:
    using K = typename Impl::Key;
    using V = typename Impl::Value;

    SortedInterface() : impl{new Impl} {};

    template<typename Iterator>
    SortedInterface(Iterator begin, Iterator end) : impl{new Impl{begin, end}} {}

    ~SortedInterface() { delete impl; };

    const std::pair<K, V> &Top() const { return impl->Top(); }

    void Pop() { impl->Pop(); }

    bool Empty() const { return impl->Empty(); }

    void InsertOrUpdate(std::pair<K, V> pair) { impl->InsertOrUpdate(std::move(pair)); }

    void Erase(const K &key) { impl->Erase(key); }

    bool Contain(const K &key) const { return impl->Contain(key); }

    /**
     * throws exception if key not found
     * @param key
     * @return
     */
    const V &Peek(const K &key) const { return impl->Peek(key); }

private:
    Impl *const impl;
};


#endif //SORTED_SORTEDITF_H
