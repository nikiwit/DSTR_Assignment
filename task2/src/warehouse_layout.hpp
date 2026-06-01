#ifndef WAREHOUSE_LAYOUT_HPP
#define WAREHOUSE_LAYOUT_HPP

#include <string>
using namespace std;

// one location in the warehouse (ex: a Zone or a Shelf)
struct LayoutNode
{
    string name;
    string type; // ex: "Warehouse", "Zone", "Aisle", "Shelf"

    // up to 10 child locations (ex: a Zone can have multiple Aisles)
    LayoutNode *children[10];
    int childCount;
};

// function prototypes
void initWarehouse();
void addLocation(string parentName, string name, string type, bool showMessage = true);
void displayLayout();
bool locationExists(string targetLocation);
bool planRoute(string targetLocation);
void runLayoutDemo();

#endif
