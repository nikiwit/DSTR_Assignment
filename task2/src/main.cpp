#include "order_queue.hpp"
#include "robot_assignment.hpp"
#include "item_management.hpp"
#include "navigation.hpp"
#include "warehouse_layout.hpp"
#include <iostream>

using namespace std;

int main()
{
    int choice;
    do
    {
        cout << "\n=============================================" << endl;
        cout << "    WAREHOUSE CENTRAL OPERATION DASHBOARD     " << endl;
        cout << "=============================================" << endl;
        cout << "1. Order Management Module" << endl;
        cout << "2. Robot Assignment Module" << endl;
        cout << "3. Item Search & Management Module" << endl;
        cout << "4. Robot Navigation & Path Tracking Module" << endl;
        cout << "5. Warehouse Layout & Navigation Module" << endl;
        cout << "0. Exit" << endl;
        cout << "=============================================" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            runOrderDemo();
            break;
        case 2:
            runRobotDemo();
            break;
        case 3:
            runItemDemo();
            break;
        case 4:
            runNavigationDemo();
            break;
        case 5:
            runLayoutDemo();
            break;
        case 0:
            cout << "Exiting system. Goodbye!" << endl;
            break;
        default:
            cout << "Invalid option. Try again." << endl;
        }
    } while (choice != 0);

    return 0;
}