#include "order_queue.hpp"
#include "robot_assignment.hpp"
#include "navigation.hpp"
#include <iostream>
using namespace std;

int main() {
    int choice;
    do {
        cout << "\n===== WAREHOUSE ROBOT NAVIGATION SYSTEM =====" << endl;
        cout << "1. Order Management Module" << endl;
        cout << "2. Robot Assignment Module" << endl;
        cout << "3. Robot Navigation and Path Tracking Module" << endl;
        cout << "0. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: runOrderDemo();      break;
            case 2: runRobotDemo();      break;
            case 3: runNavigationDemo(); break;
            case 0: cout << "Goodbye." << endl; break;
            default: cout << "Invalid option." << endl;
        }
    } while (choice != 0);

    return 0;
}
