#ifndef SORTED_SORTEDIMPL_H
#define SORTED_SORTEDIMPL_H

template<typename K, typename V>
class SortedImpl {
public:
    using Key = K;
    using Value = V;

    virtual ~SortedImpl() = default;

    virtual const std::pair<K, V> &Top() const = 0;

    virtual void Pop() = 0;

    virtual bool Empty() const = 0;

    virtual void InsertOrUpdate(std::pair<K, V> pair) = 0;

    virtual void Erase(const K &key) = 0;

    virtual bool Contain(const K &key) const = 0;

    virtual const V &Peek(const K &key) const = 0;

protected:
    struct Pair {
        explicit Pair(std::pair<K, V> x) : x{std::move(x)} {}

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
class PriorityQueueSorted : public SortedImpl<K, V> {
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

    /**
     * Complexity: O(1)
     */
    const std::pair<K, V> &Top() const override {
        Assert (!Empty());
        return queue.top().x;
    }

    /**
     * Complexity: Amortized O(1)
     */
    void Pop() override {
        if (Empty()) return;
        valid.erase(queue.top().x.first);
        queue.pop();
        PopTillValid();
    }

    bool Empty() const override { return queue.empty(); }

    /**
     * Complexity: O(lg(N))
     */
    void InsertOrUpdate(std::pair<K, V> pair) override {
        auto it = valid.find(pair.first);
        if (it == valid.end())
            valid.emplace(pair);
        else
            it->second = pair.second;
        queue.emplace(std::move(pair));
        PopTillValid();
    }

    /**
     * Complexity: O(lg(N))
     */
    void Erase(const K &key) override {
        auto it = valid.find(key);
        if (it == valid.end()) return;

        valid.erase(it);
        PopTillValid();
    }

    /**
     * Complexity: O(lg(N))
     */
    bool Contain(const K &key) const override {
        return valid.find(key) != valid.end();
    }

    /**
     * Complexity: O(lg(N))
     */
    const V &Peek(const K &key) const override { return valid.at(key); }

private:
    /**
     * Complexity: Amortized O(1)
     */
    void PopTillValid() {
        while (!queue.empty()) {
            auto it = valid.find(queue.top().x.first);
            if (it == valid.end() || it->second != queue.top().x.second)
                // this is a spurious element
                queue.pop();
            else break;
        }
    }

    using Pair = typename SortedImpl<K, V>::Pair;
    std::priority_queue<Pair> queue;
    std::map<K, V> valid;
};

template<typename K, typename V>
class SetSorted : public SortedImpl<K, V> {
public:
    SetSorted() = default;

    template<typename Iterator>
    explicit SetSorted(Iterator begin, Iterator end)
            : set{begin, end} {
        for (auto it = begin; it != end; ++it)
            valid.insert(*it);
    }

    ~SetSorted() override = default;

    /**
     * Complexity: O(1)
     */
    const std::pair<K, V> &Top() const override {
        Assert (!Empty());
        return set.begin()->x;
    }

    /**
     * Complexity: O(lg(N))
     */
    void Pop() override {
        if (Empty()) return;
        valid.erase(set.begin()->x.first);
        set.erase(set.begin());
    }

    bool Empty() const override { return set.empty(); }

    /**
     * Complexity: O(lg(N))
     */
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

    /**
     * Complexity: O(lg(N))
     */
    void Erase(const K &key) override {
        auto it = valid.find(key);
        if (it == valid.end()) return;

        set.erase(Pair{*it});
        valid.erase(it);
    }

    /**
     * Complexity: O(lg(N))
     */
    bool Contain(const K &key) const override {
        return valid.find(key) != valid.end();
    }

    /**
     * Complexity: O(lg(N))
     */
    const V &Peek(const K &key) const override { return valid.at(key); }

private:
    using Pair = typename SortedImpl<K, V>::Pair;
    std::set<Pair, std::greater<Pair>> set;
    std::map<K, V> valid;
};

template<typename K, typename V>
class MapSorted : public SortedImpl<K, V> {
public:
    MapSorted() = default;

    template<typename Iterator>
    explicit MapSorted(Iterator begin, Iterator end) {
        for (auto it = begin; it != end; ++it) {
            map.emplace(it->first, *it);
        }
    }

    ~MapSorted() override = default;

    /**
     * Complexity: O(N)
     */
    const std::pair<K, V> &Top() const override {
        Assert (!Empty());
        using pair = std::pair<const K, Pair>;
        auto it = std::max_element(map.begin(), map.end(), [](const pair &a, const pair &b) {
            return a.second.second < b.second.second ||
                   (a.second.second == b.second.second && a.second.first < b.second.first);
        });
        return it->second;
    }

    /**
     * Complexity: O(N)
     */
    void Pop() override {
        if (Empty()) return;
        const auto pair = Top();
        map.erase(pair.first);
    }

    bool Empty() const override { return map.empty(); }

    /**
     * Complexity: O(lg(N))
     */
    void InsertOrUpdate(std::pair<K, V> pair) override {
        auto it = map.find(pair.first);
        if (it == map.end()) {
            map.emplace(pair.first, pair);
        } else {
            it->second = pair;
        }
    }

    /**
     * Complexity: O(lg(N))
     */
    void Erase(const K &key) override { map.erase(key); }

    /**
     * Complexity: O(lg(N))
     */
    bool Contain(const K &key) const override {
        return map.find(key) != map.end();
    }

    /**
     * Complexity: O(lg(N))
     */
    const V &Peek(const K &key) const override { return map.at(key).second; }

private:
    using Pair = std::pair<K, V>;
    std::map<K, Pair> map;
};

#endif //SORTED_SORTEDIMPL_H
