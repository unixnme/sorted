#include <random>
#include <string>
#include <unordered_map>
#include "Utils.h"
#include "Sorted.h"

int main() {
    constexpr int N = 1000000;
    constexpr int num_char = 10;
    using pair = std::pair<std::string, int>;

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> char_dis(0, 25);
    std::uniform_int_distribution<> int_dis{0, 100000000};
    std::uniform_int_distribution<> idx_dis{0, N - 1};

    std::unordered_map<std::string, int> map;
    for (int i = 0; i < N; ++i) {
        std::string key;
        for (int j = 0; j < num_char; ++j) {
            key.push_back('a' + char_dis(gen));
        }
        map[std::move(key)] = int_dis(gen);
        key.clear();
    }

    std::vector<pair> vector{map.begin(), map.end()};
    map.clear();

    SortedInterface<PriorityQueueSorted<std::string, int>> pqueue{vector.begin(), vector.end()};
    SortedInterface<SetSorted<std::string, int>> set{vector.begin(), vector.end()};

    for (int i = 0; i < N; ++i) {
        auto idx = idx_dis(gen);
        vector[idx].second = int_dis(gen);
        pqueue.InsertOrUpdate(vector[idx]);
        set.InsertOrUpdate(vector[idx]);
    }
    std::sort(vector.begin(), vector.end(), [](const pair& a, const pair& b) {
        return a.second > b.second || (a.second == b.second && a.first > b.first);
    });

    for (auto &p : vector) {
        Assert (!pqueue.Empty() && !set.Empty());
        Assert (pqueue.Top() == p && set.Top() == p);
        pqueue.Pop();
        set.Pop();
    }
    Assert (pqueue.Empty() && set.Empty());

    return 0;
}
