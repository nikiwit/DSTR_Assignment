// include guard — prevents this header from being included more than once
#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "resident.hpp"

// ─── linked list ──────────────────────────────────────────────────────────────

// a single node holding one resident record
struct Node {
    Resident data;
    Node*    next;
};

/**
 * singly linked list of residents.
 * insert() appends to the tail using a tail pointer so it runs in O(1).
 * always call clear() before the program exits to free heap memory.
 */
struct LinkedList {
    Node* head;
    Node* tail; // kept to avoid traversing the whole list on every insert
    int   size;

    // initializes an empty list
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    // method to allow external functions to access head safely
    Node* getHead() const { return head; }

    // appends a new node at the tail in O(1)
    void insert(const Resident& r) {
        Node* new_node  = new Node;
        new_node->data  = r;
        new_node->next  = nullptr;

        if (head == nullptr) {
            head = new_node; // first insert — new node is both head and tail
            tail = new_node;
        } else {
            tail->next = new_node; // link current tail to the new node
            tail       = new_node;
        }
        size++;
    }

    // frees all heap-allocated nodes and resets the list
    void clear() {
        Node* curr = head;
        while (curr != nullptr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
        head = nullptr;
        tail = nullptr;
        size = 0;
    }
};

// ─── data loading ─────────────────────────────────────────────────────────────

/**
 * loads residents from a single CSV file into the linked list.
 * skips the header row and computes monthly_emission for each record.
 * returns the number of nodes inserted, or -1 on file error.
 */
int load_csv_to_list(const char* filename, LinkedList& list, char city_label);

// loads all three datasets into the linked list in order: A → B → C
int load_all_datasets_to_list(LinkedList& list);


// ─── complexity comparison tables ─────────────────────────────────────────────
void display_sorting_comparison_table();
void display_searching_comparison_table();

// ─── analysis ─────────────────────────────────────────────────────────────────

/**
 * --- VIONNA ---
 * declare age group categorization and carbon emission analysis prototypes here.
 */

// function prototypes
int load_csv_to_list(const char* filename, LinkedList& list, char city_label);
int load_all_datasets_to_list(LinkedList& list);

// analysis functions
void display_age_group_categorization(const LinkedList& list);
void display_total_emission_per_dataset(const LinkedList& list);
void display_emission_per_transport_mode(const LinkedList& list);
void display_emission_comparison_by_dataset_and_age_group(const LinkedList& list);
void display_transport_breakdown_by_age_group(const LinkedList& list);


// ─── sorting ──────────────────────────────────────────────────────────────────

void sort_by_monthly_emission_merge_sort(LinkedList& list);
void sort_by_age_merge_sort(LinkedList& list);
void sort_by_transport_mode_merge_sort(LinkedList& list);
void sort_by_monthly_emission_quick_sort(LinkedList& list);
void sort_by_age_quick_sort(LinkedList& list);
void sort_by_transport_mode_quick_sort(LinkedList& list);

// ─── searching ────────────────────────────────────────────────────────────────

void searching_experiment_introduction();
void search_experiment_age_group_sorted(const LinkedList& list, int min_age, int max_age);
void search_experiment_transport_mode_sorted(const LinkedList& list, const char* transport_mode);
void search_experiment_distance_threshold_sorted(const LinkedList& list, double distance_threshold);
void search_experiment_age_group_unsorted(const LinkedList& list, int min_age, int max_age);
void search_experiment_transport_mode_unsorted(const LinkedList& list, const char* transport_mode);
void search_experiment_distance_threshold_unsorted(const LinkedList& list, double distance_threshold);


// closes the #ifndef guard — everything above is skipped if this header was already included
#endif
