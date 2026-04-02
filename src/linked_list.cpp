#include <iostream>  // cout, cerr
#include <fstream>   // ifstream (reading CSV files)
#include <sstream>   // stringstream (parsing CSV lines)
#include <cstring>   // strncpy (copying C-style strings)
#include <iomanip>   // setw, left (formatting console output)
#include <ctime>     // clock(), CLOCKS_PER_SEC (timing)

using namespace std;

// ─── constants ────────────────────────────────────────────────────────────────

const int MAX_ID_LEN        = 10;
const int MAX_TRANSPORT_LEN = 20;
// no max records here because link list allocates the nodes dynamically

// dataset paths — run the program from the repository root
const char* DATASET_A = "../datasets/dataset1_city_a.csv";
const char* DATASET_B = "../datasets/dataset2_city_b.csv";
const char* DATASET_C = "../datasets/dataset3_city_c.csv";

// ─── resident struct ──────────────────────────────────────────────────────────

/**
 * stores one resident's survey record.
 * monthly_emission is calculated on load: daily_distance * emission_factor * avg_days_per_month.
 * city holds 'A', 'B', or 'C' to identify the source dataset.
 */
struct Resident {
    char   id[MAX_ID_LEN];
    int    age;
    char   transport[MAX_TRANSPORT_LEN];
    double daily_distance;    // km per day
    double emission_factor;   // kg CO2 per km
    int    avg_days_per_month;
    double monthly_emission;  // calculated as: daily_distance * emission_factor * avg_days_per_month
    char   city;              // 'A', 'B', or 'C'
};

// ─── linked list ──────────────────────────────────────────────────────────────

// a single node holding one resident record
struct Node {
    Resident data;
    Node*    next;
};

/**
 * singly linked list of residents.
 * insert() appends to the tail using a tail pointer so it runs in O(1) – not necessary, we can remove it later, but it is better algorithmically wise
 * always call clear() before the program exits to free heap memory.
 */
struct LinkedList {
    Node* head;
    Node* tail; // kept to avoid traversing the whole list on every insert
    int   size;

    // initializes an empty list
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

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

// ─── timer ────────────────────────────────────────────────────────────────────

/**
 * simple timer for measuring sort and search execution time.
 * please utilize it and call start() before the operation, then get_elapsed_ms() after.
 */
struct Timer {
    clock_t start_time;

    void start() {
        start_time = clock();
    }

    // returns elapsed time in milliseconds
    double get_elapsed_ms() {
        return (double)(clock() - start_time) / CLOCKS_PER_SEC * 1000.0;
    }
};

// ─── data loading ─────────────────────────────────────────────────────────────

/**
 * loads residents from a single CSV file into the linked list.
 * skips the header row and computes monthly_emission for each record.
 * returns the number of nodes inserted, or -1 on file error.
 */
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

        // parse resident id
        getline(ss, token, ',');
        strncpy(r.id, token.c_str(), MAX_ID_LEN - 1);
        r.id[MAX_ID_LEN - 1] = '\0';

        // parse age
        getline(ss, token, ',');
        r.age = stoi(token);

        // parse transport mode (may contain spaces e.g. "School Bus")
        getline(ss, token, ',');
        strncpy(r.transport, token.c_str(), MAX_TRANSPORT_LEN - 1);
        r.transport[MAX_TRANSPORT_LEN - 1] = '\0';

        // parse daily distance in km
        getline(ss, token, ',');
        r.daily_distance = stod(token);

        // parse emission factor in kg CO2 per km (0 for bicycle and walking)
        getline(ss, token, ',');
        r.emission_factor = stod(token);

        // parse average days per month
        getline(ss, token, ',');
        r.avg_days_per_month = stoi(token);

        // compute monthly emission: distance * factor * days
        r.monthly_emission = r.daily_distance * r.emission_factor * r.avg_days_per_month;

        r.city = city_label;
        list.insert(r);
        count++;
    }

    file.close();
    return count;
}

// loads all three datasets into the linked list in order: A → B → C
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
    cout << left << setw(23) << "City C (Suburban)"      << count_c << " records" << endl;
    cout << string(34, '-') << endl;
    cout << left << setw(23) << "Total" << count_a + count_b + count_c << " records" << endl;
    cout << "==================================" << "\n" << endl;

    return count_a + count_b + count_c;
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main() {
    LinkedList residents;
    int total = load_all_datasets_to_list(residents);

    if (total <= 0) {
        cerr << "error: no records loaded. check dataset file paths." << endl;
        residents.clear();
        return 1;
    }

    /**
     * --- VIONNA ---
     * add age group categorization and carbon emission analysis here.
     * use: residents.head to traverse, residents.size for total count
     */

    /**
     * --- MARIIA ---
     * add sorting and searching experiments here.
     * use: residents.head to traverse, residents.size for total count
     */

    residents.clear(); // free all heap memory before exit
    return 0;
}
