#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

#include <string>
using namespace std;

const int MAX_STEPS = 20;

struct StepNode {
    string    direction;
    StepNode* next;
};

struct LogNode {
    string   message;
    LogNode* next;
};

void pushStep(string direction);
void backtrack();
bool isPathEmpty();
void displayPath();
void displayReturnPath();
void displayLog();
void runNavigationDemo();

#endif
