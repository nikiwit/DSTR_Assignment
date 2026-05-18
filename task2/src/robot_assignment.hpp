#ifndef ROBOT_ASSIGNMENT_HPP
#define ROBOT_ASSIGNMENT_HPP

#include <string>
using namespace std;

const int MAX_ROBOTS = 6;

struct Robot {
    int    robotID;
    string name;
    string status;    // available, busy, maintenance
    int    taskCount;
};

// use singly-linked list for assignment history — mirrors how order_queue tracks completed orders
struct AssignmentNode {
    int             robotID;
    string          robotName;
    string          taskDesc;
    AssignmentNode* next;
};

void addRobot(string name);
void assignNextTask(string taskDesc);
void setRobotStatus(int robotID, string newStatus);
void displayRobots();
void displayAssignments();
void runRobotDemo();

#endif
