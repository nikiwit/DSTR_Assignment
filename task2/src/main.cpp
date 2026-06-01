#include "order_queue.hpp"
#include "robot_assignment.hpp"
#include "item_management.hpp"
#include "navigation.hpp"
#include "warehouse_layout.hpp"
#include <iostream>

using namespace std;

int main()
{

    // int choice;
    // do
    // {
    //     cout << "\n=============================================" << endl;
    //     cout << "    WAREHOUSE CENTRAL OPERATION DASHBOARD     " << endl;
    //     cout << "=============================================" << endl;
    //     cout << "1. Order Management Module" << endl;
    //     cout << "2. Robot Assignment Module" << endl;
    //     cout << "3. Item Search & Management Module" << endl;
    //     cout << "4. Robot Navigation & Path Tracking Module" << endl;
    //     cout << "5. Warehouse Layout & Navigation Module" << endl;
    //     cout << "0. Exit" << endl;
    //     cout << "=============================================" << endl;
    //     cout << "Choice: ";
    //     cin >> choice;

    //     switch (choice)
    //     {
    //     case 1:
    //         runOrderDemo();
    //         break;
    //     case 2:
    //         runRobotDemo();
    //         break;
    //     case 3:
    //         runItemDemo();
    //         break;
    //     case 4:
    //         runNavigationDemo();
    //         break;
    //     case 5:
    //         runLayoutDemo();
    //         break;
    //     case 0:
    //         cout << "Exiting system. Goodbye!" << endl;
    //         break;
    //     default:
    //         cout << "Invalid option. Try again." << endl;
    //     }
    // } while (choice != 0);


    initWarehouse();

    cout << "\n=============================================" << endl;
    cout << "    WAREHOUSE ROBOT NAVIGATION SYSTEM RUN     " << endl;
    cout << "=============================================" << endl;
    cout << "Initializing warehouse robots..." << endl;
    addRobot("Alpha");
    addRobot("Beta");
    addRobot("Gamma");

    cout << "\nLoading starter item database..." << endl;
    addItem(101, "USB-C Cable", "Shelf 1");
    addItem(102, "Mechanical Keyboard", "Shelf 3");
    addItem(105, "Wireless Mouse", "Shelf 1");
    addItem(108, "Gaming Monitor", "Shelf 5");

    int choice;
    int nextSystemOrderID = 1;

    do
    {
        cout << "\n=============================================" << endl;
        cout << "        INTEGRATED ORDER FULFILMENT           " << endl;
        cout << "=============================================" << endl;
        cout << "1. Receive and fulfil new order" << endl;
        cout << "2. View pending orders" << endl;
        cout << "3. View completed orders" << endl;
        cout << "4. View robot status" << endl;
        cout << "5. View warehouse layout" << endl;
        cout << "6. View navigation log" << endl;
        cout << "0. Exit" << endl;
        cout << "=============================================" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {

            //  if order queue is full (classic check)
            if (isFull())
            {
                cout << "System overload: order queue is full. Please process existing orders first." << endl;
                break;
            }

            Order order;
            order.orderID = nextSystemOrderID++;
            order.completed = false;

            cin.ignore();
            cout << "\nEnter ordered item name: ";
            getline(cin, order.item);
            cout << "Enter packing destination: ";
            getline(cin, order.destination);

            cout << "Enter item warehouse location for route planning (e.g., Shelf 1): ";
            string targetLocation;
            getline(cin, targetLocation);

            if (!locationExists(targetLocation))
            {
                cout << "Invalid warehouse location. Order rejected because the shelf does not exist." << endl;
                break;
            }

            cout << "\n[1] Receiving order..." << endl;
            enqueue(order);

            cout << "\n[2] Assigning robot in circular rotation..." << endl;
            int assignedRobotID = assignNextTask("Pick '" + order.item + "' and deliver to " + order.destination);
            if (assignedRobotID == -1)
            {
                cout << "Order cannot continue until a robot becomes available." << endl;
                break;
            }
            cout << "\nRobot status after assignment:" << endl;
            displayRobots();

            cout << "\n[3] Identifying item location..." << endl;
            findItemByName(order.item);

            cout << "\n[4] Generating route to item location..." << endl;
            bool routeFound = planRoute(targetLocation);
            if (!routeFound)
            {
                cout << "Task paused: robot cannot move because the target location was not found." << endl;
                setRobotStatus(assignedRobotID, "available");
                cout << "\nRobot released because route planning failed:" << endl;
                displayRobots();
                break;
            }

            cout << "\n[5] Robot forward movement path..." << endl;
            displayPath();

            cout << "\n[6] Robot return path using reverse route..." << endl;
            displayReturnPath();

            cout << "\n[7] Returning robot step-by-step..." << endl;
            while (!isPathEmpty())
            {
                backtrack();
            }
            cout << "Robot returned to the starting point." << endl;

            cout << "\n[8] Completing order..." << endl;
            dequeue();
            setRobotStatus(assignedRobotID, "available");
            cout << "Task completed successfully. Robot is now available for the next order." << endl;
            cout << "\nFinal robot status after task completion:" << endl;
            displayRobots();
            break;
        }
        case 2:
            displayPending();
            break;
        case 3:
            displayCompleted();
            break;
        case 4:
            displayRobots();
            break;
        case 5:
            displayLayout();
            break;
        case 6:
            displayLog();
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
