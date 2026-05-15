#ifndef ORDER_QUEUE_HPP
#define ORDER_QUEUE_HPP

#include <string>
using namespace std;

const int MAX_CAPACITY = 10;

struct Order {
    int    orderID;
    string item;
    string destination;
    bool   completed;
};

// i used doubly-linked nodes (next + back) so that it can traverse the queue in both directions
struct OrderNode {
    Order      data;
    OrderNode* next;  // toward rear
    OrderNode* back;  // toward front
};

void enqueue(Order order);
void dequeue();
void displayPending();
void displayCompleted();
bool isFull();
bool isEmpty();
void runOrderDemo();

#endif
