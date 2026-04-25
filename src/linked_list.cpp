#include "linked_list.hpp"

#include <iostream>  // cout, cerr
#include <fstream>   // ifstream (reading CSV files)
#include <sstream>   // stringstream (parsing CSV lines)
#include <cstring>   // strncpy (copying C-style strings)
#include <iomanip>   // setw, left (formatting console output)
#include <cctype>    // tolower
#include <sys/resource.h>  // getrusage (memory tracking)

using namespace std;

// constants
const char* TRANSPORT_MODES[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
const int NUM_TRANSPORTS = 6;

const char* AGE_LABELS[] = {
    "6-17 : Children & Teenagers",
    "18-25 : University Students / Young Adults",
    "26-45 : Working Adults - Early Career",
    "46-60 : Working Adults - Late Career",
    "61-100 : Senior Citizens / Retirees"
};

// helper functions
int get_transport_index(const char* transport) {
    for (int i = 0; i < NUM_TRANSPORTS; i++) {
        if (strcmp(transport, TRANSPORT_MODES[i]) == 0)
            return i;
    }
    return -1;
}

int get_age_group_index(int age) {
    if (age >= 6 && age <= 17) return 0;
    if (age >= 18 && age <= 25) return 1;
    if (age >= 26 && age <= 45) return 2;
    if (age >= 46 && age <= 60) return 3;
    if (age >= 61 && age <= 100) return 4;
    return -1;
}

// ─── data loading ─────────────────────────────────────────────────────────────

int load_csv_to_list(const char* filename, LinkedList& list, char city_label) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "error: could not open file: " << filename << endl;
        return -1;
    }

    string line;
    getline(file, line); // skips header row

    int count = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        Resident r;

        getline(ss, token, ',');
        strncpy(r.id, token.c_str(), MAX_ID_LEN - 1);
        r.id[MAX_ID_LEN - 1] = '\0';

        getline(ss, token, ',');
        r.age = stoi(token);

        getline(ss, token, ',');
        strncpy(r.transport, token.c_str(), MAX_TRANSPORT_LEN - 1);
        r.transport[MAX_TRANSPORT_LEN - 1] = '\0';

        getline(ss, token, ',');
        r.daily_distance = stod(token);

        getline(ss, token, ',');
        r.emission_factor = stod(token);

        getline(ss, token, ',');
        r.avg_days_per_month = stoi(token);

        r.monthly_emission = r.daily_distance * r.emission_factor * r.avg_days_per_month;

        r.city = city_label;
        list.insert(r);
        count++;
    }

    file.close();
    return count;
}

int load_all_datasets_to_list(LinkedList& list) {
    int count_a = load_csv_to_list(DATASET_A, list, 'A');
    if (count_a < 0) count_a = 0;

    int count_b = load_csv_to_list(DATASET_B, list, 'B');
    if (count_b < 0) count_b = 0;

    int count_c = load_csv_to_list(DATASET_C, list, 'C');
    if (count_c < 0) count_c = 0;

    cout << "\n====== Dataset Load Summary ======" << endl;
    cout << left << setw(23) << "City A (Metropolitan)"  << count_a << " records" << endl;
    cout << left << setw(23) << "City B (Uni Town)"      << count_b << " records" << endl;
    cout << left << setw(23) << "City C (Suburban)"       << count_c << " records" << endl;
    cout << string(34, '-') << endl;
    cout << left << setw(23) << "Total" << count_a + count_b + count_c << " records" << endl;
    cout << "==================================" << "\n" << endl;

    return count_a + count_b + count_c;
}


// ─── analysis ─────────────────────────────────────────────────────────────────

/**
 * --- VIONNA ---
 * implement age group categorization and carbon emission analysis here.
 */

// task 4
 void display_age_group_categorization(const LinkedList& list) {

    cout << "**********---- TASK 4: Age Group Categorization ----**********\n\n";
    cout << "=======================================================\n";
    cout << "            AGE GROUP CATEGORIZATION RESULT\n";
    cout << "=======================================================\n\n";

    cout << left << setw(44) << "Age Group"
         << setw(12) << "Residents"
         << setw(27) << "Most Preferred Transport"
         << setw(17) << "Total Emission"
         << "Avg Emission per Resident" << endl;

    cout << string(130, '-') << endl;

    int count[5] = {0};
    double total[5] = {0};
    int transport_count[5][NUM_TRANSPORTS] = {0};

    Node* curr = list.getHead();

    while (curr) {
        int a = get_age_group_index(curr->data.age);
        int t = get_transport_index(curr->data.transport);

        if (a != -1) {
            count[a]++;
            total[a] += curr->data.monthly_emission;
            if (t != -1) transport_count[a][t]++;
        }
        curr = curr->next;
    }

    for (int i = 0; i < 5; i++) {

        int best = -1, max_val = -1;

        for (int j = 0; j < NUM_TRANSPORTS; j++) {
            if (transport_count[i][j] > max_val) {
                max_val = transport_count[i][j];
                best = j;
            }
        }

        string best_transport = (max_val > 0) ? TRANSPORT_MODES[best] : "No Data";

        cout << left << setw(44) << AGE_LABELS[i]
             << setw(12) << count[i]
             << setw(27) << best_transport
             << fixed << setprecision(2) << setw(17) << total[i]
             << (count[i] > 0 ? total[i] / count[i] : 0.00) << endl;
    }

    cout << string(130, '=') << "\n\n";
}

// task 5a
void display_total_emission_per_dataset(const LinkedList& list) {

    cout << "**********---- TASK 5: Carbon Emission Analysis ----**********\n\n";
    cout << "==============================================================\n";
    cout << "            5a: TOTAL CARBON EMISSION PER DATASET\n";
    cout << "==============================================================\n";

    cout << left << setw(23) << "Dataset" << "Total Emission (kg CO2)" << endl;
    cout << string(62, '-') << endl;

    double A = 0, B = 0, C = 0;

    Node* curr = list.getHead();

    while (curr) {
        if (curr->data.city == 'A') A += curr->data.monthly_emission;
        else if (curr->data.city == 'B') B += curr->data.monthly_emission;
        else if (curr->data.city == 'C') C += curr->data.monthly_emission;
        curr = curr->next;
    }

    cout << left << setw(23) << "City A" << fixed << setprecision(2) << A << endl;
    cout << left << setw(23) << "City B" << B << endl;
    cout << left << setw(23) << "City C" << C << endl;

    cout << "==============================================================\n\n";
}

// task 5b
void display_emission_per_transport_mode(const LinkedList& list) {

    cout << "=============================================================================\n";
    cout << "            5b: CARBON EMISSION PER MODE OF TRANSPORT\n";
    cout << "=============================================================================\n";

    cout << left << setw(21) << "Mode of Transport"
         << setw(10) << "Count"
         << setw(28) << "Total Emission (kg CO2)"
         << "Avg per Resident" << endl;

    cout << string(77, '-') << endl;

    int count[NUM_TRANSPORTS] = {0};
    double total[NUM_TRANSPORTS] = {0};

    Node* curr = list.getHead();

    while (curr) {
        int t = get_transport_index(curr->data.transport);
        if (t != -1) {
            count[t]++;
            total[t] += curr->data.monthly_emission;
        }
        curr = curr->next;
    }

    for (int i = 0; i < NUM_TRANSPORTS; i++) {
        if (count[i] > 0) {
            cout << left << setw(21) << TRANSPORT_MODES[i]
                 << setw(10) << count[i]
                 << fixed << setprecision(2) << setw(28) << total[i]
                 << (total[i] / count[i]) << endl;
        }
    }

    cout << string(77, '=') << "\n\n";
}

// task 5c
void display_emission_comparison_by_dataset_and_age_group(const LinkedList& list) {

    cout << "===========================================================================\n";
    cout << "            5c: EMISSION COMPARISON BY DATASET AND AGE GROUP\n";
    cout << "===========================================================================\n";

    cout << left << setw(18) << "Dataset"
         << setw(12) << "6-17"
         << setw(12) << "18-25"
         << setw(12) << "26-45"
         << setw(12) << "46-60"
         << setw(12) << "61-100" << endl;

    cout << string(75, '-') << endl;

    double A[5] = {0}, B[5] = {0}, C[5] = {0};

    Node* curr = list.getHead();

    while (curr) {
        int a = get_age_group_index(curr->data.age);

        if (a != -1) {
            if (curr->data.city == 'A') A[a] += curr->data.monthly_emission;
            else if (curr->data.city == 'B') B[a] += curr->data.monthly_emission;
            else if (curr->data.city == 'C') C[a] += curr->data.monthly_emission;
        }

        curr = curr->next;
    }

    cout << left << setw(18) << "City A";
    for (int i = 0; i < 5; i++) cout << setw(12) << fixed << setprecision(2) << A[i];
    cout << endl;

    cout << left << setw(18) << "City B";
    for (int i = 0; i < 5; i++) cout << setw(12) << B[i];
    cout << endl;

    cout << left << setw(18) << "City C";
    for (int i = 0; i < 5; i++) cout << setw(12) << C[i];
    cout << endl;

    cout << string(75, '=') << "\n\n";
}

// task 5d
void display_transport_breakdown_by_age_group(const LinkedList& list) {

    const char* labels[] = {"City A","City B","City C"};

    int counts[3][5][NUM_TRANSPORTS] = {0};
    double totals[3][5][NUM_TRANSPORTS] = {0};
    int residents[3][5] = {0};
    double emissions[3][5] = {0};

    Node* curr = list.getHead();

    while (curr) {
        int c = -1;
        if (curr->data.city == 'A') c = 0;
        else if (curr->data.city == 'B') c = 1;
        else if (curr->data.city == 'C') c = 2;

        int a = get_age_group_index(curr->data.age);
        int t = get_transport_index(curr->data.transport);

        if (c != -1 && a != -1) {
            if (t != -1) {
                counts[c][a][t]++;
                totals[c][a][t] += curr->data.monthly_emission;
            }
            residents[c][a]++;
            emissions[c][a] += curr->data.monthly_emission;
        }

        curr = curr->next;
    }

    for (int c = 0; c < 3; c++) {
        for (int a = 0; a < 5; a++) {

            if (residents[c][a] == 0) continue;

            cout << string(76,'*') << endl;
            cout << "\nDataset: " << labels[c] << endl;
            cout << "Age Group: " << AGE_LABELS[a] << endl;
            cout << string(76,'-') << endl;

            cout << left << setw(22) << "Mode of Transport"
                 << setw(10) << "Count"
                 << setw(28) << "Total Emission (kg CO2)"
                 << "Average/Resident" << endl;

            int best = -1;
            int max_val = -1;

            for (int t = 0; t < NUM_TRANSPORTS; t++) {
                if (counts[c][a][t] > 0) {

                    cout << left << setw(22) << TRANSPORT_MODES[t]
                         << setw(10) << counts[c][a][t]
                         << fixed << setprecision(2) << setw(28) << totals[c][a][t]
                         << (totals[c][a][t] / counts[c][a][t]) << endl;

                    if (counts[c][a][t] > max_val) {
                        max_val = counts[c][a][t];
                        best = t;
                    }
                }
            }

            cout << string(76,'-') << endl;
            cout << "Total Residents in Age Group : " << residents[c][a] << endl;
            cout << "Most Preferred Mode : "
                 << (best != -1 ? TRANSPORT_MODES[best] : "No Data") << endl;
            cout << "Total Emission for Age Group: "
                 << fixed << setprecision(2) << emissions[c][a] << " kg CO2" << endl;
            cout << "Average Emission per Resident: "
                 << (emissions[c][a] / residents[c][a]) << " kg CO2\n\n";
        }
    }
}


// ─── sort / search helpers ─────────────────────────────────────────────────────
// MARIIA
// Task 6 and Task 7 — all operations work directly on linked list nodes.

// ---- display utilities ----

// ---- memory tracking ----

double get_memory_usage_mb() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return (double)usage.ru_maxrss / 1024.0;  // Convert to MB (ru_maxrss is in KB on macOS)
}

bool same_ignore_case(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) return false;
        i++;
    }
    return a[i] == b[i];
}

void normalize_transport(const char* input, char out[MAX_TRANSPORT_LEN]) {
    const char* modes[] = {"Car", "Bus", "Local Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
    for (int i = 0; i < 7; i++) {
        if (same_ignore_case(input, modes[i])) {
            strncpy(out, modes[i], MAX_TRANSPORT_LEN - 1);
            out[MAX_TRANSPORT_LEN - 1] = '\0';
            return;
        }
    }
    strncpy(out, input, MAX_TRANSPORT_LEN - 1);
    out[MAX_TRANSPORT_LEN - 1] = '\0';
}

void print_line() {
    cout << "--------------------------------------------------------------" << endl;
}

void print_table_header() {
    cout << left << setw(12) << "ID"
         << setw(6)  << "Age"
         << setw(14) << "Transport"
         << setw(12) << "Distance"
         << setw(14) << "Emission"
         << "City" << endl;
    print_line();
}

void print_record(const Resident& r) {
    cout << left << setw(12) << r.id
         << setw(6)  << r.age
         << setw(14) << r.transport
         << setw(12) << fixed << setprecision(2) << r.daily_distance
         << setw(14) << fixed << setprecision(2) << r.monthly_emission
         << r.city << endl;
}

// shows first 5 nodes + last node to confirm sort order at both ends
void display_first_list_records(const LinkedList& list) {
    int n = list.size;
    print_table_header();
    if (n == 0) return;
    int show = (n <= 6) ? n : 5;
    Node* curr = list.head;
    for (int i = 0; i < show && curr != nullptr; i++) {
        cout << "  [" << (i + 1) << "] ";
        print_record(curr->data);
        curr = curr->next;
    }
    if (n > 6) {
        cout << "  ... (" << (n - 6) << " more records) ..." << endl;
        cout << "  [" << n << "] ";
        print_record(list.tail->data);
    }
}

// shows first 5 + last match when there are many results
void print_search_preview(Resident matches[], int count) {
    if (count == 0) { cout << "  (no matches found)" << endl; return; }
    int show = (count <= 6) ? count : 5;
    for (int i = 0; i < show; i++) print_record(matches[i]);
    if (count > 6) {
        cout << "  ... (" << (count - 6) << " more records) ..." << endl;
        print_record(matches[count - 1]);
    }
}

// ---- node data swap (used by quick sort) ----

static void swap_node_data(Node* a, Node* b) {
    Resident tmp = a->data;
    a->data = b->data;
    b->data = tmp;
}

// walks n steps forward from a node
static Node* ll_advance(Node* node, int n) {
    while (n-- > 0 && node) node = node->next;
    return node;
}

// finds the last node in the list
static Node* ll_find_tail(Node* head) {
    if (!head) return nullptr;
    while (head->next) head = head->next;
    return head;
}

// ---- merge sort: splits list at midpoint, returns the second half ----

static Node* ll_split(Node* head) {
    Node* slow = head;
    Node* fast = head->next;
    while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
    Node* second = slow->next;
    slow->next = nullptr;
    return second;
}

// merge sort by monthly emission (ascending)

static Node* ll_merge_emission(Node* a, Node* b) {
    if (!a) return b;
    if (!b) return a;
    if (a->data.monthly_emission <= b->data.monthly_emission) {
        a->next = ll_merge_emission(a->next, b);
        return a;
    }
    b->next = ll_merge_emission(a, b->next);
    return b;
}

static Node* ll_merge_sort_emission(Node* head) {
    if (!head || !head->next) return head;
    Node* second = ll_split(head);
    return ll_merge_emission(ll_merge_sort_emission(head), ll_merge_sort_emission(second));
}

// merge sort by age (ascending)

static Node* ll_merge_age(Node* a, Node* b) {
    if (!a) return b;
    if (!b) return a;
    if (a->data.age <= b->data.age) {
        a->next = ll_merge_age(a->next, b);
        return a;
    }
    b->next = ll_merge_age(a, b->next);
    return b;
}

static Node* ll_merge_sort_age(Node* head) {
    if (!head || !head->next) return head;
    Node* second = ll_split(head);
    return ll_merge_age(ll_merge_sort_age(head), ll_merge_sort_age(second));
}

// merge sort by transport mode (alphabetical)

static Node* ll_merge_transport(Node* a, Node* b) {
    if (!a) return b;
    if (!b) return a;
    if (strcmp(a->data.transport, b->data.transport) <= 0) {
        a->next = ll_merge_transport(a->next, b);
        return a;
    }
    b->next = ll_merge_transport(a, b->next);
    return b;
}

static Node* ll_merge_sort_transport(Node* head) {
    if (!head || !head->next) return head;
    Node* second = ll_split(head);
    return ll_merge_transport(ll_merge_sort_transport(head), ll_merge_sort_transport(second));
}

// merge sort by daily distance (ascending) — used only by sorted distance search

static Node* ll_merge_distance(Node* a, Node* b) {
    if (!a) return b;
    if (!b) return a;
    if (a->data.daily_distance <= b->data.daily_distance) {
        a->next = ll_merge_distance(a->next, b);
        return a;
    }
    b->next = ll_merge_distance(a, b->next);
    return b;
}

static Node* ll_merge_sort_distance(Node* head) {
    if (!head || !head->next) return head;
    Node* second = ll_split(head);
    return ll_merge_distance(ll_merge_sort_distance(head), ll_merge_sort_distance(second));
}

// ---- quick sort: swaps Resident data between nodes (no pointer rewiring needed) ----

static Node* ll_qs_partition_emission(Node* lo, Node* hi) {
    double pivot = hi->data.monthly_emission;
    Node* i = lo;
    for (Node* j = lo; j != hi; j = j->next) {
        if (j->data.monthly_emission <= pivot) { swap_node_data(i, j); i = i->next; }
    }
    swap_node_data(i, hi);
    return i;
}

static void ll_quick_sort_emission(Node* lo, Node* hi) {
    if (!lo || lo == hi) return;
    Node* p = ll_qs_partition_emission(lo, hi);
    if (p != lo) {
        Node* prev = lo;
        while (prev->next != p) prev = prev->next;
        ll_quick_sort_emission(lo, prev);
    }
    if (p != hi) ll_quick_sort_emission(p->next, hi);
}

static Node* ll_qs_partition_age(Node* lo, Node* hi) {
    int pivot = hi->data.age;
    Node* i = lo;
    for (Node* j = lo; j != hi; j = j->next) {
        if (j->data.age <= pivot) { swap_node_data(i, j); i = i->next; }
    }
    swap_node_data(i, hi);
    return i;
}

static void ll_quick_sort_age(Node* lo, Node* hi) {
    if (!lo || lo == hi) return;
    Node* p = ll_qs_partition_age(lo, hi);
    if (p != lo) {
        Node* prev = lo;
        while (prev->next != p) prev = prev->next;
        ll_quick_sort_age(lo, prev);
    }
    if (p != hi) ll_quick_sort_age(p->next, hi);
}

static Node* ll_qs_partition_transport(Node* lo, Node* hi) {
    const char* pivot = hi->data.transport;
    Node* i = lo;
    for (Node* j = lo; j != hi; j = j->next) {
        if (strcmp(j->data.transport, pivot) <= 0) { swap_node_data(i, j); i = i->next; }
    }
    swap_node_data(i, hi);
    return i;
}

static void ll_quick_sort_transport(Node* lo, Node* hi) {
    if (!lo || lo == hi) return;
    Node* p = ll_qs_partition_transport(lo, hi);
    if (p != lo) {
        Node* prev = lo;
        while (prev->next != p) prev = prev->next;
        ll_quick_sort_transport(lo, prev);
    }
    if (p != hi) ll_quick_sort_transport(p->next, hi);
}

// ---- sorted search helpers ----

// deep-copies the list so sorted searches never modify the original
static LinkedList clone_list(const LinkedList& src) {
    LinkedList copy;
    Node* curr = src.head;
    while (curr) { copy.insert(curr->data); curr = curr->next; }
    return copy;
}

// binary search: returns first node with age >= min_age in [start, len nodes] sorted by age
static Node* ll_lower_bound_age(Node* start, int len, int min_age) {
    if (len <= 0 || !start) return nullptr;
    if (len == 1) return (start->data.age >= min_age) ? start : nullptr;
    int half = len / 2;
    Node* mid = ll_advance(start, half);
    if (mid->data.age >= min_age) {
        Node* left = ll_lower_bound_age(start, half, min_age);
        return left ? left : mid;
    }
    return ll_lower_bound_age(mid->next, len - half - 1, min_age);
}

// binary search: returns first node where transport == key in [start, len nodes] sorted alphabetically
static Node* ll_lower_bound_transport(Node* start, int len, const char* key) {
    if (len <= 0 || !start) return nullptr;
    if (len == 1) return (strcmp(start->data.transport, key) == 0) ? start : nullptr;
    int half = len / 2;
    Node* mid = ll_advance(start, half);
    int cmp = strcmp(mid->data.transport, key);
    if (cmp == 0) {
        Node* left = ll_lower_bound_transport(start, half, key);
        return left ? left : mid;
    }
    if (cmp < 0) return ll_lower_bound_transport(mid->next, len - half - 1, key);
    return ll_lower_bound_transport(start, half, key);
}

// binary search: returns first node with daily_distance > threshold in [start, len nodes] sorted by distance
static Node* ll_lower_bound_distance(Node* start, int len, double threshold) {
    if (len <= 0 || !start) return nullptr;
    if (len == 1) return (start->data.daily_distance > threshold) ? start : nullptr;
    int half = len / 2;
    Node* mid = ll_advance(start, half);
    if (mid->data.daily_distance > threshold) {
        Node* left = ll_lower_bound_distance(start, half, threshold);
        return left ? left : mid;
    }
    return ll_lower_bound_distance(mid->next, len - half - 1, threshold);
}


double bytes_to_mb(size_t bytes) {
    return bytes / (1024.0 * 1024.0);
}

void print_memory_mb(size_t bytes) {
    cout << fixed << setprecision(4) << bytes_to_mb(bytes) << " MB";
}


// ---------- sorting ----------
void sort_by_monthly_emission_merge_sort(LinkedList& list) {
    cout << "\n===========================================================================\n";
    cout << "                       TASK 6: SORTING\n";
    cout << "===========================================================================\n";
    cout << "\n==============================================================" << endl;

    cout << "  SORT: Monthly Emission  [Merge Sort / Ascending]" << endl;
    cout << "  Theoretical: O(n log n) time  |  O(log n) space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();
    list.head = ll_merge_sort_emission(list.head);
    list.tail = ll_find_tail(list.head);
    double elapsed = t.get_elapsed_ms();

    double mem_after = get_memory_usage_mb();

    display_first_list_records(list);
    cout << "  Measured sort time: " << fixed << setprecision(4) << elapsed
         << " ms  (n=" << list.size << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4)
         << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (auxiliary): O(log n) stack space" << endl;
}

void sort_by_age_merge_sort(LinkedList& list) {
    cout << "\n==============================================================" << endl;
    cout << "  SORT: Age  [Merge Sort / Ascending]" << endl;
    cout << "  Theoretical: O(n log n) time  |  O(log n) space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();
    list.head = ll_merge_sort_age(list.head);
    list.tail = ll_find_tail(list.head);
    double elapsed = t.get_elapsed_ms();

    double mem_after = get_memory_usage_mb();

    display_first_list_records(list);
    cout << "  Measured sort time: " << fixed << setprecision(4) << elapsed
         << " ms  (n=" << list.size << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4)
         << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (auxiliary): O(log n) stack space" << endl;
}

void sort_by_transport_mode_merge_sort(LinkedList& list) {
    cout << "\n==============================================================" << endl;
    cout << "  SORT: Transport Mode  [Merge Sort / Alphabetical]" << endl;
    cout << "  Theoretical: O(n log n) time  |  O(log n) space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();
    list.head = ll_merge_sort_transport(list.head);
    list.tail = ll_find_tail(list.head);
    double elapsed = t.get_elapsed_ms();

    double mem_after = get_memory_usage_mb();

    display_first_list_records(list);
    cout << "  Measured sort time: " << fixed << setprecision(4) << elapsed
         << " ms  (n=" << list.size << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4)
         << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (auxiliary): O(log n) stack space" << endl;
}

void sort_by_monthly_emission_quick_sort(LinkedList& list) {
    cout << "\n==============================================================" << endl;
    cout << "  SORT: Monthly Emission  [Quick Sort / Ascending]" << endl;
    cout << "  Theoretical: O(n log n) avg time  |  O(log n) avg space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();
    ll_quick_sort_emission(list.head, list.tail);
    double elapsed = t.get_elapsed_ms();

    double mem_after = get_memory_usage_mb();

    display_first_list_records(list);
    cout << "  Measured sort time: " << fixed << setprecision(4) << elapsed
         << " ms  (n=" << list.size << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4)
         << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (auxiliary): O(log n) avg stack space" << endl;
}
void sort_by_age_quick_sort(LinkedList& list) {
    cout << "\n==============================================================" << endl;
    cout << "  SORT: Age  [Quick Sort / Ascending]" << endl;
    cout << "  Theoretical: O(n log n) avg time  |  O(log n) avg space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();
    ll_quick_sort_age(list.head, list.tail);
    double elapsed = t.get_elapsed_ms();

    double mem_after = get_memory_usage_mb();

    display_first_list_records(list);
    cout << "  Measured sort time: " << fixed << setprecision(4)
         << elapsed << " ms  (n=" << list.size << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4)
         << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (auxiliary): O(log n) avg stack space" << endl;
}

void sort_by_transport_mode_quick_sort(LinkedList& list) {
    cout << "\n==============================================================" << endl;
    cout << "  SORT: Transport Mode  [Quick Sort / Alphabetical]" << endl;
    cout << "  Theoretical: O(n log n) avg time  |  O(log n) avg space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();
    ll_quick_sort_transport(list.head, list.tail);
    double elapsed = t.get_elapsed_ms();

    double mem_after = get_memory_usage_mb();

    display_first_list_records(list);
    cout << "  Measured sort time: " << fixed << setprecision(4)
         << elapsed << " ms  (n=" << list.size << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4)
         << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (auxiliary): O(log n) avg stack space" << endl;
}

 void searching_experiment_introduction() {
    cout << "\n=============================================================" << endl;
    cout << "              TASK 7: Searching Experiments                 " << endl;
    cout << "=============================================================" << endl; }

void search_experiment_age_group_sorted(const LinkedList& list, int min_age, int max_age) {
    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Age Group " << min_age << "-" << max_age << "  [Sorted / Binary Search]" << endl;
    cout << "  Theoretical: O(n log n) sort + O(n log n) LL binary search" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer sort_t;
    sort_t.start();

    LinkedList sorted = clone_list(list);
    sorted.head = ll_merge_sort_age(sorted.head);
    sorted.tail = ll_find_tail(sorted.head);

    double sort_elapsed = sort_t.get_elapsed_ms();

    Timer search_t;
    search_t.start();

    Node* lb = ll_lower_bound_age(sorted.head, sorted.size, min_age);

    Resident matches[500];
    int count = 0;
    Node* curr = lb;
    while (curr && curr->data.age <= max_age) {
        matches[count++] = curr->data;
        curr = curr->next;
    }

    double search_elapsed = search_t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    print_table_header();
    print_search_preview(matches, count);
    print_line();

    cout << "  Matches : " << count << endl;
    cout << "  Sort time  (clone + merge sort) : " << fixed << setprecision(4)
         << sort_elapsed << " ms" << endl;
    cout << "  Search time (binary search)     : " << fixed << setprecision(4)
         << search_elapsed << " ms" << endl;
    cout << "  Total time                      : " << fixed << setprecision(4)
         << (sort_elapsed + search_elapsed) << " ms" << endl;

    cout << "  Memory used (measured): " << fixed << setprecision(4)
         << (mem_after - mem_before) << " MB" << endl;

    cout << "  Memory used (estimated): "
         << fixed << setprecision(4)
         << ((list.size * sizeof(Node)) + (500 * sizeof(Resident))) / (1024.0 * 1024.0)
         << " MB" << endl;

    cout << "==============================================================" << endl;

    sorted.clear();
}
void search_experiment_transport_mode_sorted(const LinkedList& list, const char* transport_mode) {
    char key[MAX_TRANSPORT_LEN];
    normalize_transport(transport_mode, key);

    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Transport \"" << key << "\"  [Sorted / Binary Search]" << endl;
    cout << "  Theoretical: O(n log n) sort + O(n log n) LL binary search" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer sort_t;
    sort_t.start();
    LinkedList sorted = clone_list(list);
    sorted.head = ll_merge_sort_transport(sorted.head);
    sorted.tail = ll_find_tail(sorted.head);
    double sort_elapsed = sort_t.get_elapsed_ms();

    Timer search_t;
    search_t.start();
    Node* lb = ll_lower_bound_transport(sorted.head, sorted.size, key);

    Resident matches[500];
    int count = 0;
    Node* curr = lb;
    while (curr && strcmp(curr->data.transport, key) == 0) {
        matches[count++] = curr->data;
        curr = curr->next;
    }

    double search_elapsed = search_t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    size_t estimated_bytes = (size_t)list.size * sizeof(Node) + 500 * sizeof(Resident);

    print_table_header();
    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Sort time  (clone + merge sort) : " << fixed << setprecision(4) << sort_elapsed << " ms" << endl;
    cout << "  Search time (binary search)     : " << fixed << setprecision(4) << search_elapsed << " ms" << endl;
    cout << "  Total time                      : " << fixed << setprecision(4) << (sort_elapsed + search_elapsed) << " ms" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4) << (estimated_bytes / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "==============================================================" << endl;

    sorted.clear();
}

void search_experiment_distance_threshold_sorted(const LinkedList& list, double distance_threshold) {
    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Daily Distance > " << distance_threshold << " km  [Sorted / Binary Search]" << endl;
    cout << "  Theoretical: O(n log n) sort + O(n log n) LL binary search" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer sort_t;
    sort_t.start();
    LinkedList sorted = clone_list(list);
    sorted.head = ll_merge_sort_distance(sorted.head);
    sorted.tail = ll_find_tail(sorted.head);
    double sort_elapsed = sort_t.get_elapsed_ms();

    Timer search_t;
    search_t.start();
    Node* lb = ll_lower_bound_distance(sorted.head, sorted.size, distance_threshold);

    Resident matches[500];
    int count = 0;
    Node* curr = lb;
    while (curr) {
        matches[count++] = curr->data;
        curr = curr->next;
    }

    double search_elapsed = search_t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    size_t estimated_bytes = (size_t)list.size * sizeof(Node) + 500 * sizeof(Resident);

    print_table_header();
    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Sort time  (clone + merge sort) : " << fixed << setprecision(4) << sort_elapsed << " ms" << endl;
    cout << "  Search time (binary search)     : " << fixed << setprecision(4) << search_elapsed << " ms" << endl;
    cout << "  Total time                      : " << fixed << setprecision(4) << (sort_elapsed + search_elapsed) << " ms" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4) << (estimated_bytes / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "==============================================================" << endl;

    sorted.clear();
}

void search_experiment_age_group_unsorted(const LinkedList& list, int min_age, int max_age) {
    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Age Group " << min_age << "-" << max_age << "  [Unsorted / Linear Search]" << endl;
    cout << "  Theoretical: O(n) time  |  O(1) space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();

    Resident matches[500];
    int count = 0;
    Node* curr = list.head;
    while (curr != nullptr) {
        if (curr->data.age >= min_age && curr->data.age <= max_age) {
            matches[count++] = curr->data;
        }
        curr = curr->next;
    }

    double elapsed = t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    size_t estimated_bytes = 500 * sizeof(Resident);

    print_table_header();
    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Linear search time: " << fixed << setprecision(4) << elapsed << " ms" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4) << (estimated_bytes / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "==============================================================" << endl;
}

void search_experiment_distance_threshold_unsorted(const LinkedList& list, double distance_threshold) {
    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Daily Distance > " << distance_threshold << " km  [Unsorted / Linear Search]" << endl;
    cout << "  Theoretical: O(n) time  |  O(1) space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();

    Resident matches[500];
    int count = 0;
    Node* curr = list.head;

    while (curr != nullptr) {
        if (curr->data.daily_distance > distance_threshold) {
            matches[count++] = curr->data;
        }
        curr = curr->next;
    }

    double elapsed = t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    size_t estimated_bytes = 500 * sizeof(Resident);

    print_table_header();
    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Linear search time: " << fixed << setprecision(4) << elapsed << " ms" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4) << (estimated_bytes / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "==============================================================" << endl;
}


void search_experiment_transport_mode_unsorted(const LinkedList& list, const char* transport_mode) {
    char key[MAX_TRANSPORT_LEN];
    normalize_transport(transport_mode, key);

    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Transport \"" << key << "\"  [Unsorted / Linear Search]" << endl;
    cout << "  Theoretical: O(n) time  |  O(1) space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();
    Timer t;
    t.start();

    Resident matches[500];
    int count = 0;
    Node* curr = list.head;
    while (curr != nullptr) {
        if (strcmp(curr->data.transport, key) == 0)
            matches[count++] = curr->data;
        curr = curr->next;
    }

    double elapsed = t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    print_table_header();
    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Linear search time: " << fixed << setprecision(4) << elapsed << " ms" << endl;
    cout << "  Memory used: " << fixed << setprecision(2) << (mem_after - mem_before) << " MB" << endl;
    cout << "==============================================================" << endl;
}


// ---------- complexity comparison tables ----------

void display_sorting_comparison_table() {
    cout << "\n===========================================================================\n";
    cout << "        TASK 6: SORTING - COMPLEXITY COMPARISON\n";
    cout << "===========================================================================\n";

    cout << "\n  THEORETICAL TIME COMPLEXITY" << endl;
    cout << "===========================================================================\n";

    cout << left
         << setw(14) << ""
         << setw(13) << "MS/Emission"
         << setw(13) << "MS/Age"
         << setw(13) << "MS/Transport"
         << setw(13) << "QS/Emission"
         << setw(13) << "QS/Age"
         << "QS/Transport" << endl;
    cout << string(91, '-') << endl;
    cout << left << setw(14) << "Array"
         << setw(13) << "O(n log n)"
         << setw(13) << "O(n log n)"
         << setw(13) << "O(n log n)"
         << setw(13) << "O(n log n)"
         << setw(13) << "O(n log n)"
         << "O(n log n)" << endl;
    cout << left << setw(14) << "Linked List"
         << setw(13) << "O(n log n)"
         << setw(13) << "O(n log n)"
         << setw(13) << "O(n log n)"
         << setw(13) << "O(n log n)"
         << setw(13) << "O(n log n)"
         << "O(n log n)" << endl;
    cout << string(91, '-') << endl;

    cout << "\n  THEORETICAL SPACE COMPLEXITY" << endl;
    cout << "===========================================================================\n";

    cout << left
         << setw(14) << ""
         << setw(13) << "MS/Emission"
         << setw(13) << "MS/Age"
         << setw(13) << "MS/Transport"
         << setw(13) << "QS/Emission"
         << setw(13) << "QS/Age"
         << "QS/Transport" << endl;
    cout << string(91, '-') << endl;
    cout << left << setw(14) << "Array"
         << setw(13) << "O(n)"
         << setw(13) << "O(n)"
         << setw(13) << "O(n)"
         << setw(13) << "O(log n)"
         << setw(13) << "O(log n)"
         << "O(log n)" << endl;
    cout << left << setw(14) << "Linked List"
         << setw(13) << "O(log n)"
         << setw(13) << "O(log n)"
         << setw(13) << "O(log n)"
         << setw(13) << "O(log n)"
         << setw(13) << "O(log n)"
         << "O(log n)" << endl;
    cout << string(91, '-') << endl;
    // cout << "  MS = Merge Sort | QS = Quick Sort" << endl;
    // cout << "  MS: sorts by relinking node pointers — O(log n) recursion stack, no auxiliary array." << endl;
    // cout << "  QS: sorts by swapping Resident data in place — O(log n) avg recursion stack." << endl;
    cout << "  Note: Numerical measures of time are printed above each sort result." << endl;
    cout << "===========================================================================" << endl;
}

void display_searching_comparison_table() {
    cout << "\n===========================================================================\n";
    cout << "        TASK 7: SEARCHING - COMPLEXITY COMPARISON\n";
    cout << "===========================================================================\n";

    cout << "\n  THEORETICAL TIME COMPLEXITY" << endl;
    cout << "===========================================================================\n";

    cout << left
         << setw(14) << ""
         << setw(16) << "Bin/Age"
         << setw(16) << "Bin/Transport"
         << setw(16) << "Bin/Dist"
         << setw(12) << "Lin/Age"
         << setw(15) << "Lin/Transport"
         << "Lin/Dist" << endl;
    cout << string(89, '-') << endl;
    cout << left << setw(14) << "Array"
         << setw(16) << "O(n log n)*"
         << setw(16) << "O(n log n)*"
         << setw(16) << "O(n log n)*"
         << setw(12) << "O(n)"
         << setw(15) << "O(n)"
         << "O(n)" << endl;
    cout << left << setw(14) << "Linked List"
         << setw(16) << "O(n log n)*"
         << setw(16) << "O(n log n)*"
         << setw(16) << "O(n log n)*"
         << setw(12) << "O(n)"
         << setw(15) << "O(n)"
         << "O(n)" << endl;
    cout << string(89, '-') << endl;

    cout << "\n  THEORETICAL SPACE COMPLEXITY" << endl;
    cout << "===========================================================================\n";

    cout << left
         << setw(14) << ""
         << setw(12) << "Bin/Age"
         << setw(16) << "Bin/Transport"
         << setw(12) << "Bin/Dist"
         << setw(12) << "Lin/Age"
         << setw(16) << "Lin/Transport"
         << "Lin/Dist" << endl;
    cout << string(82, '-') << endl;
    cout << left << setw(14) << "Array"
         << setw(12) << "O(n)"
         << setw(16) << "O(n)"
         << setw(12) << "O(n)"
         << setw(12) << "O(1)"
         << setw(16) << "O(1)"
         << "O(1)" << endl;
    cout << left << setw(14) << "Linked List"
         << setw(12) << "O(n)"
         << setw(16) << "O(n)"
         << setw(12) << "O(n)"
         << setw(12) << "O(1)"
         << setw(16) << "O(1)"
         << "O(1)" << endl;
    cout << string(82, '-') << endl;
    // cout << "  Bin = Binary Search | Lin = Linear Search | Dist = Distance" << endl;
    // cout << "  * Binary time = O(n log n) sort + O(n log n) LL binary search => O(n log n) total." << endl;
    // cout << "  Binary space: O(n) cloned list + O(log n) recursion stack. Linear: O(1) scan only." << endl;
    cout << "  Note: Numerical values of sort time, search time, and total are printed above each search result." << endl;
    cout << "===========================================================================" << endl;
}
