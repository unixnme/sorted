
#ifndef HARA_PRIORITY_QUEUE_H
#define HARA_PRIORITY_QUEUE_H

#include <vector>
#include <queue>
#include <set>
#include <map>
#include "Utils.h"

template<typename Impl>
class PriorityQueue {
public:
    using K = typename Impl::Key;
    using V = typename Impl::Value;

    PriorityQueue() : impl{new Impl} {};

    template<typename Iterator>
    PriorityQueue(Iterator begin, Iterator end) : impl{new Impl{begin, end}} {}

    ~PriorityQueue() { delete impl; };

    const std::pair<K, V> &Top() const { return impl->Top(); }

    void Pop() { impl->Pop(); }

    bool Empty() const { return impl->Empty(); }

    size_t Size() const { return impl->Size(); }

    void InsertOrUpdate(std::pair<K, V> pair) { impl->InsertOrUpdate(std::move(pair)); }

    void Erase(const K &key) { impl->Erase(key); }

    bool Contain(const K &key) const { return impl->Contain(key); }

    /**
     * throws exception if key not found
     * @param key
     * @return
     */
    const V& Peek(const K &key) const { return impl->Peek(key); }

private:
    Impl* const impl;
};


#endif //HARA_PRIORITY_QUEUE_H
