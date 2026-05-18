#include "robot_assignment.hpp"
#include <iostream>
using namespace std;

// robots logical representation is an array of robot's status
// currently max is 6 robots
Robot robots[MAX_ROBOTS];
int robotCount  = 0;
int current     = 0;
int nextRobotID = 1;

AssignmentNode* assignHead = NULL;
int assignCount = 0;

bool isRobotsEmpty() {
    return robotCount == 0;
}

bool isRobotsFull() {
    return robotCount == MAX_ROBOTS;
}

void addRobot(string name) {
    if (isRobotsFull()) {
        cout << "Cannot add robot: capacity reached (" << MAX_ROBOTS << ")." << endl;
        return;
    }
    robots[robotCount].robotID   = nextRobotID++;
    robots[robotCount].name      = name;
    robots[robotCount].status    = "available";
    robots[robotCount].taskCount = 0;
    robotCount++;
    cout << "Robot [" << robots[robotCount - 1].robotID << "] " << name << " added." << endl;
}

void assignNextTask(string taskDesc) {
    if (isRobotsEmpty()) {
        cout << "No robots in the system." << endl;
        return;
    }

    // сheck one by one, skip robots under maintenance or busy; assign to the first available one
    for (int checked = 0; checked < robotCount; checked++) {
        int idx  = current % robotCount;
        current  = (current + 1) % robotCount;

        if (robots[idx].status == "available") {
            robots[idx].status = "busy";
            robots[idx].taskCount++;

            cout << "Task assigned to Robot [" << robots[idx].robotID << "] "
                 << robots[idx].name << ": " << taskDesc << endl;

            AssignmentNode* node = new AssignmentNode;
            node->robotID   = robots[idx].robotID;
            node->robotName = robots[idx].name;
            node->taskDesc  = taskDesc;
            node->next      = NULL;

            if (assignHead == NULL) {
                assignHead = node;
            } else {
                AssignmentNode* cur = assignHead;
                while (cur->next != NULL)
                    cur = cur->next;
                cur->next = node;
            }
            assignCount++;
            return;
        }
    }

    cout << "No available robots to assign the task." << endl;
}

void setRobotStatus(int robotID, string newStatus) {
    for (int i = 0; i < robotCount; i++) {
        if (robots[i].robotID == robotID) {
            robots[i].status = newStatus;
            cout << "Robot [" << robotID << "] " << robots[i].name
                 << " status set to: " << newStatus << endl;
            return;
        }
    }
    cout << "Robot ID " << robotID << " not found." << endl;
}

void displayRobots() {
    cout << "\n=== Robot Status (" << robotCount << " robots) ===" << endl;
    if (isRobotsEmpty()) {
        cout << "(none)" << endl;
        return;
    }
    for (int i = 0; i < robotCount; i++) {
        cout << i + 1 << ". [ID " << robots[i].robotID << "] "
             << robots[i].name
             << " | status: " << robots[i].status
             << " | tasks assigned: " << robots[i].taskCount << endl;
    }
}

void displayAssignments() {
    cout << "\n=== Assignment History (" << assignCount << ") ===" << endl;
    if (assignHead == NULL) {
        cout << "(none)" << endl;
        return;
    }
    AssignmentNode* cur = assignHead;
    int pos = 1;
    while (cur != NULL) {
        cout << pos++ << ". Robot [" << cur->robotID << "] "
             << cur->robotName << " -> " << cur->taskDesc << endl;
        cur = cur->next;
    }
}

void runRobotDemo() {
    cout << "\n===== ROBOT ASSIGNMENT MODULE =====" << endl;

    // test
    addRobot("Alpha");
    addRobot("Beta");
    addRobot("Gamma");

    int choice;
    do {
        cout << "\n--- Robot Assignment ---" << endl;
        cout << "1. Add robot" << endl;
        cout << "2. Assign task to next robot" << endl;
        cout << "3. Set robot status" << endl;
        cout << "4. View all robots" << endl;
        cout << "5. View assignment history" << endl;
        cout << "0. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string name;
                cin.ignore();
                cout << "Robot name: ";
                getline(cin, name);
                addRobot(name);
                break;
            }
            case 2: {
                string task;
                cin.ignore();
                cout << "Task description: ";
                getline(cin, task);
                assignNextTask(task);
                break;
            }
            case 3: {
                int    id;
                string status;
                cout << "Robot ID: ";
                cin >> id;
                cout << "New status (available / busy / maintenance): ";
                cin >> status;
                setRobotStatus(id, status);
                break;
            }
            case 4:
                displayRobots();
                break;
            case 5:
                displayAssignments();
                break;
            case 0:
                cout << "Exiting robot assignment." << endl;
                break;
            default:
                cout << "Invalid option." << endl;
        }
    } while (choice != 0);
}
