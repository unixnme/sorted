#include <random>
#include <chrono>
#include <iostream>
#include "Sorted.h"
#include "Utils.h"

enum {
    INSERT = 0,
    ERASE = 1,
    TOP = 2,
    POP = 3,
    PEEK = 4
};

std::vector<std::string> keys;

struct Operation {
    explicit Operation(int op, int key, int value)
            : op{op}, key{key}, value{value} {}
    const int op;
    const int key;
    const int value;
};

template<typename Sorted>
std::vector<std::pair<std::string, int>>
PerformOperations(Sorted &sorted, const std::vector<Operation> &ops, long long int &duration) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<std::string, int>> result;
    for (auto &operation : ops) {
        const auto &key = keys[operation.key];
        switch (operation.op) {
            case INSERT:
                sorted.InsertOrUpdate({key, operation.value});
                break;
            case ERASE:
                sorted.Erase(key);
                break;
            case TOP:
                if (!sorted.Empty())
                    result.push_back(sorted.Top());
                break;
            case POP:
                sorted.Pop();
                break;
            case PEEK:
                if (sorted.Contain(key))
                    result.emplace_back(key, sorted.Peek(key));
                break;
            default:
                Assert(false);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    return result;
}

int main(int argc, const char** argv) {
    constexpr int NUM_OPERATIONS = 10000000;
    constexpr int NUM_KEYS = 1000000;
    constexpr int KEY_LENGTH = 5;

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> char_dis(0, 25);
    std::uniform_int_distribution<> op_dis(0, PEEK);
    std::uniform_int_distribution<> val_dis{0, 100000000};
    std::uniform_int_distribution<> idx_dis{0, NUM_KEYS};

    keys.reserve(NUM_KEYS);
    for (int i = 0; i < NUM_KEYS; ++i) {
        std::string key;
        for (int j = 0; j < KEY_LENGTH; ++j) {
            key.push_back(char_dis(gen));
        }
        keys.push_back(std::move(key));
        key.clear();
    }

    std::vector<Operation> ops;
    ops.reserve(NUM_OPERATIONS);
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        auto idx = idx_dis(gen);
        auto op = op_dis(gen);
        auto value = val_dis(gen);
        ops.emplace_back(op, idx, value);
    }

    long long int duration;
    SortedInterface<PriorityQueueSorted<std::string, int>> pqueue;
    auto result1 = PerformOperations(pqueue, ops, duration);
    std::cout << "pqueue: " << duration << "ms" << std::endl;

    SortedInterface<SetSorted<std::string, int>> set;
    auto result2 = PerformOperations(set, ops, duration);
    std::cout << "set: " << duration << "ms" << std::endl;

    SortedInterface<MapSorted<std::string, int>> map;
    auto result3 = PerformOperations(map, ops, duration);
    std::cout << "set: " << duration << "ms" << std::endl;

    Assert(result1 == result2 && result2 == result3);

    return 0;
}