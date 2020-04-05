#include <iostream>
#include <string>
#include "Sorted.h"

int main() {
    std::vector<std::pair<std::string, int>> vector;
    vector.emplace_back("hello", 4);
    vector.emplace_back("hi", 2);
    vector.emplace_back("world", 5);
    vector.emplace_back("c++", 3);

    SortedInterface<PriorityQueueSorted<std::string, int>> pq_sorted{vector.begin(), vector.end()};
    pq_sorted.Insert({"hello", 0});
    pq_sorted.Insert({"c", 2});

    while (!pq_sorted.Empty()) {
        auto &x = pq_sorted.Top();
        std::cout << x.first << " " << x.second << std::endl;
        pq_sorted.Pop();
    }

    SortedInterface<SetSorted<std::string, int>> set_sorted{vector.begin(), vector.end()};
    set_sorted.Insert({"hello", 0});
    set_sorted.Insert({"c", 2});

    while (!set_sorted.Empty()) {
        auto &x = set_sorted.Top();
        std::cout << x.first << " " << x.second << std::endl;
        set_sorted.Pop();
    }

    return 0;
}
