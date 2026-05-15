#include "order_queue.hpp"
#include <iostream>
using namespace std;

// global queue state — front and rear follow the same pattern from lab 9 we did
OrderNode* front = NULL;
OrderNode* rear  = NULL;
int queueCount = 0;
int nextOrderID = 1;

// i keep completed orders in a separate singly-linked list so the queue only holds pending ones
struct CompletedNode {
    Order          data;
    CompletedNode* next;
};
CompletedNode* completedHead = NULL;
int completedCount = 0;

bool isEmpty() {
    return front == NULL;
}

bool isFull() {
    return queueCount == MAX_CAPACITY;
}

void enqueue(Order order) {
    if (isFull()) {
        cout << "Overflow: queue is full." << endl;
        return;
    }
    OrderNode* node = new OrderNode;
    node->data = order;
    node->next = NULL;
    node->back = rear;
    if (rear == NULL) {
        front = rear = node;
    } else {
        rear->next = node;
        rear = node;
    }
    queueCount++;
    cout << "Order #" << order.orderID << " added: "
         << order.item << " -> " << order.destination << endl;
}

void dequeue() {
    if (isEmpty()) {
        cout << "Underflow: queue is empty." << endl;
        return;
    }
    OrderNode* temp = front;
    Order processed  = temp->data;
    front = front->next;
    if (front != NULL)
        front->back = NULL;
    else
        rear = NULL;
    delete temp;
    queueCount--;

    processed.completed = true;

    // append to the completed list so the history stays in processing order
    CompletedNode* cn = new CompletedNode;
    cn->data = processed;
    cn->next = NULL;
    if (completedHead == NULL) {
        completedHead = cn;
    } else {
        CompletedNode* cur = completedHead;
        while (cur->next != NULL)
            cur = cur->next;
        cur->next = cn;
    }
    completedCount++;

    cout << "Processed order #" << processed.orderID << ": "
         << processed.item << " -> " << processed.destination << endl;
}

void displayPending() {
    cout << "\n=== Pending Orders (" << queueCount << ") ===" << endl;
    if (isEmpty()) {
        cout << "(none)" << endl;
        return;
    }
    OrderNode* cur = front;
    int pos = 1;
    while (cur != NULL) {
        cout << pos++ << ". [ID " << cur->data.orderID << "] "
             << cur->data.item << " -> " << cur->data.destination << endl;
        cur = cur->next;
    }
}

void displayCompleted() {
    cout << "\n=== Completed Orders (" << completedCount << ") ===" << endl;
    if (completedHead == NULL) {
        cout << "(none)" << endl;
        return;
    }
    CompletedNode* cur = completedHead;
    int pos = 1;
    while (cur != NULL) {
        cout << pos++ << ". [ID " << cur->data.orderID << "] "
             << cur->data.item << " -> " << cur->data.destination << endl;
        cur = cur->next;
    }
}

void runOrderDemo() {
    cout << "\n===== ORDER MANAGEMENT MODULE =====" << endl;

    int choice;
    do {
        cout << "\n--- Order Management ---" << endl;
        cout << "1. Add new order" << endl;
        cout << "2. Process next order" << endl;
        cout << "3. View pending orders" << endl;
        cout << "4. View completed orders" << endl;
        cout << "0. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                Order o;
                o.orderID   = nextOrderID++;
                o.completed = false;
                cin.ignore();
                cout << "Item name: ";
                getline(cin, o.item);
                cout << "Destination: ";
                getline(cin, o.destination);
                enqueue(o);
                break;
            }
            case 2:
                dequeue();
                break;
            case 3:
                displayPending();
                break;
            case 4:
                displayCompleted();
                break;
            case 0:
                cout << "Exiting order management." << endl;
                break;
            default:
                cout << "Invalid option." << endl;
        }
    } while (choice != 0);
}
