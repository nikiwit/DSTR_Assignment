#include "item_management.hpp"
#include <iostream>
using namespace std;

ItemNode* itemTreeRoot = NULL;

ItemNode* findMinValueNode(ItemNode* node) {
    ItemNode* current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}

ItemNode* insertItemNode(ItemNode* root, Item item) {
    if (root == NULL) {
        ItemNode* newNode = new ItemNode;
        newNode->data = item;
        newNode->left = newNode->right = NULL;
        return newNode;
    }
    if (item.itemID < root->data.itemID) {
        root->left = insertItemNode(root->left, item);
    } else if (item.itemID > root->data.itemID) {
        root->right = insertItemNode(root->right, item);
    } else {
        cout << "Error: Item ID " << item.itemID << " already exists." << endl;
    }
    return root;
}

ItemNode* deleteItemNode(ItemNode* root, int id) {
    if (root == NULL) return root;

    if (id < root->data.itemID) {
        root->left = deleteItemNode(root->left, id);
    } else if (id > root->data.itemID) {
        root->right = deleteItemNode(root->right, id);
    } else {
        if (root->left == NULL) {
            ItemNode* temp = root->right;
            delete root;
            return temp;
        } else if (root->right == NULL) {
            ItemNode* temp = root->left;
            delete root;
            return temp;
        }

        ItemNode* temp = findMinValueNode(root->right);
        root->data = temp->data;
        root->right = deleteItemNode(root->right, temp->data.itemID);
    }
    return root;
}

ItemNode* searchItemNodeByID(ItemNode* root, int id) {
    if (root == NULL || root->data.itemID == id)
        return root;
    if (id < root->data.itemID)
        return searchItemNodeByID(root->left, id);
    return searchItemNodeByID(root->right, id);
}

void searchItemNodesByName(ItemNode* root, string name, bool& found) {
    if (root == NULL) return;
    searchItemNodesByName(root->left, name, found);
    if (root->data.name == name) {
        cout << " Found: [ID " << root->data.itemID << "] " << root->data.name 
             << " | Location: " << root->data.location << endl;
        found = true;
    }
    searchItemNodesByName(root->right, name, found);
}

void displayItemsInOrder(ItemNode* root) {
    if (root == NULL) return;
    displayItemsInOrder(root->left);
    cout << "  -> [ID: " << root->data.itemID << "] " << root->data.name 
         << " | Location: " << root->data.location << endl;
    displayItemsInOrder(root->right);
}

void addItem(int id, string name, string location) {
    Item item = {id, name, location};
    itemTreeRoot = insertItemNode(itemTreeRoot, item);
    cout << "Item record integrated successfully." << endl;
}

void removeItem(int id) {
    if (searchItemNodeByID(itemTreeRoot, id) == NULL) {
        cout << "Item ID " << id << " not found in system storage." << endl;
        return;
    }
    itemTreeRoot = deleteItemNode(itemTreeRoot, id);
    cout << "Item ID " << id << " wiped from inventory log successfully." << endl;
}

void updateItemDetails(int id, string newName, string newLocation) {
    ItemNode* node = searchItemNodeByID(itemTreeRoot, id);
    if (node != NULL) {
        node->data.name = newName;
        node->data.location = newLocation;
        cout << "Item ID " << id << " record updated successfully." << endl;
    } else {
        cout << "Item ID " << id << " not found." << endl;
    }
}

void findItemByID(int id) {
    ItemNode* node = searchItemNodeByID(itemTreeRoot, id);
    if (node != NULL) {
        cout << " Found: [ID " << node->data.itemID << "] " << node->data.name 
             << " | Location: " << node->data.location << endl;
    } else {
        cout << "Item ID " << id << " could not be resolved." << endl;
    }
}

void findItemByName(string name) {
    bool found = false;
    searchItemNodesByName(itemTreeRoot, name, found);
    if (!found) {
        cout << "No matching records found for name \"" << name << "\"." << endl;
    }
}

void displayAllItems() {
    cout << "\n=== Structured Warehouse Inventory (Sorted In-Order by ID) ===" << endl;
    if (itemTreeRoot == NULL) {
        cout << "(The warehouse database inventory is empty)" << endl;
        return;
    }
    displayItemsInOrder(itemTreeRoot);
}

void runItemDemo() {
    cout << "\n===== ITEM SEARCH AND MANAGEMENT MODULE =====" << endl;
    if (itemTreeRoot == NULL) {
        addItem(105, "Wireless Mouse", "Zone A, Aisle 2, Shelf 1");
        addItem(102, "Mechanical Keyboard", "Zone B, Aisle 1, Shelf 3");
        addItem(108, "Gaming Monitor", "Zone A, Aisle 4, Shelf 2");
        addItem(101, "USB-C Cable", "Zone C, Aisle 1, Shelf 1");
    }

    int choice;
    do {
        cout << "\n--- Item Inventory Control ---" << endl;
        cout << "1. Insert new item" << endl;
        cout << "2. Search item by ID" << endl;
        cout << "3. Search item by Name" << endl;
        cout << "4. Update item details" << endl;
        cout << "5. Delete item record" << endl;
        cout << "6. View structured/sorted inventory list" << endl;
        cout << "0. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int id;
                string name, loc;
                cout << "Enter Item ID: ";
                cin >> id;
                cin.ignore();
                cout << "Enter Item Name: ";
                getline(cin, name);
                cout << "Enter Warehouse Location: ";
                getline(cin, loc);
                addItem(id, name, loc);
                break;
            }
            case 2: {
                int id;
                cout << "Enter Item ID to locate: ";
                cin >> id;
                findItemByID(id);
                break;
            }
            case 3: {
                string name;
                cin.ignore();
                cout << "Enter Item Name to locate: ";
                getline(cin, name);
                findItemByName(name);
                break;
            }
            case 4: {
                int id;
                string name, loc;
                cout << "Enter Item ID to update: ";
                cin >> id;
                cin.ignore();
                cout << "Enter New Item Name: ";
                getline(cin, name);
                cout << "Enter New Warehouse Location: ";
                getline(cin, loc);
                updateItemDetails(id, name, loc);
                break;
            }
            case 5: {
                int id;
                cout << "Enter Item ID to delete: ";
                cin >> id;
                removeItem(id);
                break;
            }
            case 6:
                displayAllItems();
                break;
            case 0:
                cout << "Exiting item management." << endl;
                break;
            default:
                cout << "Invalid processing selection choice." << endl;
        }
    } while (choice != 0);
}