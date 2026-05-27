#ifndef ITEM_MANAGEMENT_HPP
#define ITEM_MANAGEMENT_HPP

#include <string>
using namespace std;

struct Item {
    int itemID;
    string name;
    string location;
};

struct ItemNode {
    Item data;
    ItemNode* left;
    ItemNode* right;
};

ItemNode* insertItemNode(ItemNode* root, Item item);
ItemNode* deleteItemNode(ItemNode* root, int id);
ItemNode* searchItemNodeByID(ItemNode* root, int id);
void searchItemNodesByName(ItemNode* root, string name, bool& found);
ItemNode* findMinValueNode(ItemNode* node);
void displayItemsInOrder(ItemNode* root);

void addItem(int id, string name, string location);
void removeItem(int id);
void updateItemDetails(int id, string newName, string newLocation);
void findItemByID(int id);
void findItemByName(string name);
void displayAllItems();
void runItemDemo();

#endif