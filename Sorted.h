
#ifndef SORTED_SORTED_H
#define SORTED_SORTED_H

#include <vector>
#include <queue>
#include <set>
#include <map>

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

    void Insert(std::pair<K, V> pair) { impl->Insert(std::move(pair)); }

private:
    Impl *impl;
};

template<typename K, typename V>
class SortedImplInterface {
public:
    using Key = K;
    using Value = V;

    virtual ~SortedImplInterface() = default;

    virtual const std::pair<K, V> &Top() const = 0;

    virtual void Pop() = 0;

    virtual bool Empty() const = 0;

    virtual void Insert(std::pair<K, V> pair) = 0;

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

    const std::pair<K, V> &Top() const override { return queue.top().x; }

    void Pop() override {
        valid.erase(queue.top().x.first);
        queue.pop();
        PopTillValid();
    }

    bool Empty() const override { return queue.empty(); }

    void Insert(std::pair<K, V> pair) override {
        auto it = valid.find(pair.first);
        if (it == valid.end())
            valid.emplace(pair);
        else
            it->second = pair.second;
        queue.emplace(std::move(pair));
        PopTillValid();
    }

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

    const std::pair<K, V> &Top() const override { return set.begin()->x; }

    void Pop() override { set.erase(set.begin()); }

    bool Empty() const override { return set.empty(); }

    void Insert(std::pair<K, V> pair) override {
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

private:
    using Pair = typename SortedImplInterface<K, V>::Pair;
    std::set<Pair, std::greater<Pair>> set;
    std::map<K, V> valid;
};

#endif //SORTED_SORTED_H
