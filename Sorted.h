
#ifndef SORTED_SORTED_H
#define SORTED_SORTED_H

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

    std::pair<K, V> Top() const { return impl->Top(); }

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
    V Peek(const K &key) const { return impl->Peek(key); }

private:
    Impl* const impl;
};

template<typename K, typename V>
class SortedImplInterface {
public:
    using Key = K;
    using Value = V;

    virtual ~SortedImplInterface() = default;

    virtual std::pair<K, V> Top() const = 0;

    virtual void Pop() = 0;

    virtual bool Empty() const = 0;

    virtual void InsertOrUpdate(std::pair<K, V> pair) = 0;

    virtual void Erase(const K &key) = 0;

    virtual bool Contain(const K &key) const = 0;

    virtual V Peek(const K &key) const = 0;

protected:
    struct Pair {
        explicit Pair(std::pair<K, V> x) : x{x} {}

        std::pair<K, V> x;

        bool operator<(const Pair &that) const {
            return x.second < that.x.second ||
                    (x.second == that.x.second && x.first < that.x.first);
        }
        bool operator>(const Pair &that) const {
            return x.second > that.x.second ||
                   (x.second == that.x.second && x.first > that.x.first);
        }
    };
};

/**
 * The pqueue should always be in a state where the top element is valid
 * @tparam K
 * @tparam V
 */
template<typename K, typename V>
class PriorityQueueSorted : public SortedImplInterface<K, V> {
public:
    PriorityQueueSorted() = default;

    template<typename Iterator>
    explicit PriorityQueueSorted(Iterator begin, Iterator end)
            : queue{begin, end} {
        for (auto it = begin; it != end; ++it) {
            valid.emplace(*it);
        }
    }

    ~PriorityQueueSorted() override = default;

    std::pair<K, V> Top() const override {
        Assert (!Empty());
        return queue.top().x;
    }

    void Pop() override {
        if (Empty()) return;
        valid.erase(queue.top().x.first);
        queue.pop();
        PopTillValid();
    }

    bool Empty() const override { return queue.empty(); }

    void InsertOrUpdate(std::pair<K, V> pair) override {
        auto it = valid.find(pair.first);
        if (it == valid.end())
            valid.emplace(pair);
        else
            it->second = pair.second;
        queue.emplace(std::move(pair));
        PopTillValid();
    }

    void Erase(const K &key) override {
        auto it = valid.find(key);
        if (it == valid.end()) return;

        valid.erase(it);
        PopTillValid();
    }

    bool Contain(const K &key) const override {
        return valid.find(key) != valid.end();
    }

    V Peek(const K &key) const override { return valid.at(key); }

private:
    void PopTillValid() {
        while (!queue.empty()) {
            auto it = valid.find(queue.top().x.first);
            if (it == valid.end() || it->second != queue.top().x.second)
                // this is a spurious element
                queue.pop();
            else break;
        }
    }

    using Pair = typename SortedImplInterface<K, V>::Pair;
    std::priority_queue<Pair> queue;
    std::map<K, V> valid;
};

template<typename K, typename V>
class SetSorted : public SortedImplInterface<K, V> {
public:
    SetSorted() = default;

    template<typename Iterator>
    explicit SetSorted(Iterator begin, Iterator end)
            : set{begin, end} {
        for (auto it = begin; it != end; ++it)
            valid.insert(*it);
    }

    ~SetSorted() override = default;

    std::pair<K, V> Top() const override {
        Assert (!Empty());
        return set.begin()->x;
    }

    void Pop() override {
        if (Empty()) return;
        valid.erase(set.begin()->x.first);
        set.erase(set.begin());
    }

    bool Empty() const override { return set.empty(); }

    void InsertOrUpdate(std::pair<K, V> pair) override {
        auto it = valid.find(pair.first);
        if (it == valid.end()) {
            valid.insert(pair);
            set.emplace(std::move(pair));
        } else {
            set.erase(Pair{*it});
            it->second = pair.second;
            set.emplace(std::move(pair));
        }
    }

    void Erase(const K &key) override {
        auto it = valid.find(key);
        if (it == valid.end()) return;

        set.erase(Pair{*it});
        valid.erase(it);
    }

    bool Contain(const K &key) const override {
        return valid.find(key) != valid.end();
    }

    V Peek(const K &key) const override { return valid.at(key); }

private:
    using Pair = typename SortedImplInterface<K, V>::Pair;
    std::set<Pair, std::greater<Pair>> set;
    std::map<K, V> valid;
};

template<typename K, typename V>
class MapSorted : public SortedImplInterface<K, V> {
public:
    MapSorted() = default;

    template<typename Iterator>
    explicit MapSorted(Iterator begin, Iterator end)
    : map{begin, end} {}

    ~MapSorted() override = default;

    std::pair<K, V> Top() const override {
        Assert (!Empty());
        using pair = std::pair<const K, V>;
        auto it = std::max_element(map.begin(), map.end(), [](const pair& a, const pair& b) {
            return a.second < b.second || (a.second == b.second && a.first < b.first);
        });
        return *it;
    }

    void Pop() override {
        if (Empty()) return;
        const auto pair = Top();
        map.erase(pair.first);
    }

    bool Empty() const override { return map.empty(); }

    void InsertOrUpdate(std::pair<K, V> pair) override {
        auto it = map.find(pair.first);
        if (it == map.end()) {
            map.insert(std::move(pair));
        } else {
            it->second = pair.second;
        }
    }

    void Erase(const K &key) override { map.erase(key); }

    bool Contain(const K &key) const override {
        return map.find(key) != map.end();
    }

    V Peek(const K &key) const override { return map.at(key); }

private:
    std::map<K, V> map;
};

#endif //SORTED_SORTED_H
