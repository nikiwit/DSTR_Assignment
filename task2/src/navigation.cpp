#include "navigation.hpp"
#include <iostream>
using namespace std;

// stack: top is the head of the linked list
StepNode* top      = NULL;
int       stepCount = 0;

LogNode* logHead = NULL;
LogNode* logTail = NULL;

bool isPathEmpty() {
    return top == NULL;
}

bool isPathFull() {
    return stepCount == MAX_STEPS;
}

static void appendLog(string message) {
    LogNode* node = new LogNode;
    node->message = message;
    node->next    = NULL;
    if (logHead == NULL) {
        logHead = logTail = node;
    } else {
        logTail->next = node;
        logTail       = node;
    }
}

void pushStep(string direction) {
    if (isPathFull()) {
        cout << "Path is full: cannot record more steps." << endl;
        return;
    }
    StepNode* node  = new StepNode;
    node->direction = direction;
    node->next      = top;
    top             = node;
    stepCount++;
    cout << "Step recorded: " << direction << endl;
    appendLog("MOVE: " + direction);
}

void backtrack() {
    if (isPathEmpty()) {
        cout << "Nothing to backtrack: path is empty." << endl;
        return;
    }
    StepNode* temp = top;
    cout << "Backtracking: undoing step '" << top->direction << "'" << endl;
    appendLog("BACKTRACK: undid '" + top->direction + "'");
    top = top->next;
    delete temp;
    stepCount--;
}

// recursive helper to print the stack bottom-to-top (forward order)
static void printForward(StepNode* node, int step) {
    if (node == NULL) return;
    printForward(node->next, step - 1);
    cout << step << ". " << node->direction << endl;
}

void displayPath() {
    cout << "\n=== Forward Path (" << stepCount << " steps) ===" << endl;
    if (isPathEmpty()) {
        cout << "(no steps recorded)" << endl;
        return;
    }
    printForward(top, stepCount);
}

void displayReturnPath() {
    cout << "\n=== Return Path (" << stepCount << " steps) ===" << endl;
    if (isPathEmpty()) {
        cout << "(no steps recorded)" << endl;
        return;
    }
    // natural top-to-bottom traversal of the stack is already the reverse route
    StepNode* cur = top;
    int step = 1;
    while (cur != NULL) {
        cout << step++ << ". " << cur->direction << endl;
        cur = cur->next;
    }
}

void displayLog() {
    cout << "\n=== Navigation Log ===" << endl;
    if (logHead == NULL) {
        cout << "(empty)" << endl;
        return;
    }
    LogNode* cur = logHead;
    int pos = 1;
    while (cur != NULL) {
        cout << pos++ << ". " << cur->message << endl;
        cur = cur->next;
    }
}

void runNavigationDemo() {
    cout << "\n===== ROBOT NAVIGATION AND PATH TRACKING MODULE =====" << endl;

    int choice;
    do {
        cout << "\n--- Navigation ---" << endl;
        cout << "1. Record step" << endl;
        cout << "2. Backtrack (undo last step)" << endl;
        cout << "3. View forward path" << endl;
        cout << "4. View return path" << endl;
        cout << "5. View navigation log" << endl;
        cout << "0. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string dir;
                cin.ignore();
                cout << "Direction (forward / left / right / backward): ";
                getline(cin, dir);
                pushStep(dir);
                break;
            }
            case 2:
                backtrack();
                break;
            case 3:
                displayPath();
                break;
            case 4:
                displayReturnPath();
                break;
            case 5:
                displayLog();
                break;
            case 0:
                cout << "Exiting navigation." << endl;
                break;
            default:
                cout << "Invalid option." << endl;
        }
    } while (choice != 0);
}
