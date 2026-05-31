#include "warehouse_layout.hpp"
#include "navigation.hpp" // connect to Task 3: Robot Navigation Module
#include <iostream>

using namespace std;

// warehouse starting point
LayoutNode *warehouseRoot = NULL;

// create a new location node
LayoutNode *createNode(string name, string type)
{
    LayoutNode *node = new LayoutNode;
    node->name = name;
    node->type = type;
    node->childCount = 0;

    // initialize all 10 child slots to NULL
    for (int i = 0; i < 10; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}

// search for a location by name
LayoutNode *searchLocation(LayoutNode *current, string searchName)
{
    if (current == NULL)
        return NULL;
    if (current->name == searchName)
        return current;

    // search through all child locations
    for (int i = 0; i < current->childCount; i++)
    {
        LayoutNode *found = searchLocation(current->children[i], searchName);
        if (found != NULL)
            return found; // location found
    }

    return NULL; // location not found
}

// model the warehouse structure
void addLocation(string parentName, string name, string type, bool showMessage)
{
    LayoutNode *newNode = createNode(name, type);

    if (warehouseRoot == NULL)
    {
        warehouseRoot = newNode;
        return;
    }

    LayoutNode *parent = searchLocation(warehouseRoot, parentName);
    if (parent != NULL)
    {
        if (parent->childCount < 10)
        {
            parent->children[parent->childCount] = newNode;
            parent->childCount++;

            //  only print to the terminal if showMessage is true
            if (showMessage)
            {
                cout << "Added " << type << " '" << name << "' inside '" << parentName << "'." << endl;
            }
        }
        else
        {
            cout << "Error: " << parentName << " is full!" << endl;
            delete newNode;
        }
    }
    else
    {
        cout << "Error: Parent location '" << parentName << "' does not exist." << endl;
        delete newNode;
    }
}

// print the warehouse tree
void printTree(LayoutNode *node, int spacing)
{
    if (node == NULL)
        return;

    // indent based on tree level
    for (int i = 0; i < spacing; i++)
        cout << "  ";
    cout << "- [" << node->type << "] " << node->name << endl;

    // print all child in this location
    for (int i = 0; i < node->childCount; i++)
    {
        printTree(node->children[i], spacing + 1);
    }
}

void displayLayout()
{
    cout << "\n=== WAREHOUSE STRUCTURAL LAYOUT ===" << endl;
    if (warehouseRoot == NULL)
    {
        cout << "(Warehouse is empty)" << endl;
        return;
    }
    printTree(warehouseRoot, 0);
}

// find a route to the target location
bool findPathLogic(LayoutNode *current, string target, string path[], int &stepCount)
{
    if (current == NULL)
        return false;

    // add current location to the route
    path[stepCount] = current->name;
    stepCount++;

    // if target reached
    if (current->name == target)
        return true;

    // if not, continue check all children
    for (int i = 0; i < current->childCount; i++)
    {
        if (findPathLogic(current->children[i], target, path, stepCount))
        {
            return true;
        }
    }

    // wrong path, backtrack
    stepCount--;
    return false;
}

// send route steps to robot navigation
void planRoute(string targetLocation)
{
    string path[20]; // store route steps
    int steps = 0;

    if (findPathLogic(warehouseRoot, targetLocation, path, steps))
    {
        cout << "\nRoute found to " << targetLocation << "! Sending to Robot Navigation..." << endl;

        // skip root and send remaining steps
        for (int i = 1; i < steps; i++)
        {
            string movementInstruction = "Enter " + path[i];
            pushStep(movementInstruction); // calling Task 3 function
        }
    }
    else
    {
        cout << "\nLocation '" << targetLocation << "' could not be found." << endl;
    }
}

// create default warehouse locations
void initWarehouse()
{
    if (warehouseRoot != NULL)
        return;

    // layer 0 - root
    addLocation("", "Main Hub", "Warehouse", false);

    // layer 1 - zone
    addLocation("Main Hub", "Zone A", "Zone", false);
    addLocation("Main Hub", "Zone B", "Zone", false);
    addLocation("Main Hub", "Zone C", "Zone", false);

    // layer 2 - aisle
    addLocation("Zone A", "Aisle 1", "Aisle", false);
    addLocation("Zone A", "Aisle 2", "Aisle", false);
    addLocation("Zone B", "Aisle 3", "Aisle", false);
    addLocation("Zone B", "Aisle 4", "Aisle", false);
    addLocation("Zone C", "Aisle 5", "Aisle", false);

    // layer 3 - shelf
    addLocation("Aisle 1", "Shelf 1", "Shelf", false);
    addLocation("Aisle 1", "Shelf 2", "Shelf", false);
    addLocation("Aisle 2", "Shelf 3", "Shelf", false);
    addLocation("Aisle 3", "Shelf 4", "Shelf", false);
    addLocation("Aisle 4", "Shelf 5", "Shelf", false);
    addLocation("Aisle 5", "Shelf 6", "Shelf", false);
}

void runLayoutDemo()
{
    initWarehouse();
    cout << "\n===== WAREHOUSE LAYOUT AND NAVIGATION MODULE =====" << endl;

    int choice;
    do
    {
        cout << "\n=== WAREHOUSE LAYOUT & NAVIGATION MANAGEMENT ===" << endl;
        cout << "1. Display Warehouse Structure" << endl;
        cout << "2. Add New Warehouse Location" << endl;
        cout << "3. Plan Route to Target Location" << endl;
        cout << "0. Return to Main Menu" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            displayLayout();
            break;

        case 2:
        {
            string parent, name, type;
            cin.ignore();
            cout << "Enter Parent Location Name (e.g., Aisle 1): ";
            getline(cin, parent);

            // check if parent location exists
            LayoutNode *parentNode = searchLocation(warehouseRoot, parent);
            if (parentNode == NULL)
            {
                cout << "Error: Parent location '" << parent << "' does not exist. Operation aborted." << endl;
                break;
            }

            // check if parent is a shelf (shelf cannot hold more locations)
            if (parentNode->type == "Shelf")
            {
                cout << "Error: Base layer conflict. Operation aborted." << endl;
                break;
            }

            cout << "Enter New Location Name (e.g., Shelf 3): ";
            getline(cin, name);

            // prevent duplicate location names
            if (searchLocation(warehouseRoot, name) != NULL)
            {
                cout << "Error: Name conflict. Location '" << name << "' already exists in the system layout." << endl;
                break;
            }

            // automatically set the new location type based on its parent
            string deducedType = "";
            if (parentNode->type == "Warehouse")
                deducedType = "Zone";
            else if (parentNode->type == "Zone")
                deducedType = "Aisle";
            else if (parentNode->type == "Aisle")
                deducedType = "Shelf";

            addLocation(parent, name, deducedType, true);
            break;
        }

        case 3:
        {
            string target;
            cin.ignore();
            cout << "Enter Target Location to generate route (e.g., Shelf 2): ";
            getline(cin, target);
            planRoute(target);
            break;
        }

        case 0:
            cout << "Exiting Warehouse Layout & Navigation Module..." << endl;
            break;

        default:
            cout << "Invalid option." << endl;
        }

    } while (choice != 0);
}