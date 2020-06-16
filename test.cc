#include "priority_queue.h"
#include "priority_queue_impl.h"

int main(int argc, const char** argv) {
    struct Data {
        int data;
    };
    struct Compare {
        bool operator()(const Data &a, const Data &b) const { return a.data < b.data; }
    };

    PriorityQueue<PriorityQueueSorted<int, Data, Compare>> queue1;
    PriorityQueue<SetSorted<int, Data, Compare>> queue2;
    PriorityQueue<MapSorted<int, Data, Compare>> queue3;

    return 0;
}