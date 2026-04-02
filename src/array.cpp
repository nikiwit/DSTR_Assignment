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
const int MAX_RECORDS       = 500; // exact total across all 3 datasets

// dataset paths — run the program from the repository root
const char* DATASET_A = "../datasets/dataset1_city_a.csv";
const char* DATASET_B = "../datasets/dataset2_city_b.csv";
const char* DATASET_C = "../datasets/dataset3_city_c.csv";

// ─── resident struct ──────────────────────────────────────────────────────────

/**
 * stores one resident's survey record.
 * monthly_emission is computed on load: daily_distance * emission_factor * avg_days_per_month.
 * city holds 'A', 'B', or 'C' to identify the source dataset.
 */
struct Resident {
    char   id[MAX_ID_LEN];
    int    age;
    char   transport[MAX_TRANSPORT_LEN];
    double daily_distance;    // km per day
    double emission_factor;   // kg CO2 per km
    int    avg_days_per_month;
    double monthly_emission;  // computed: daily_distance * emission_factor * avg_days_per_month
    char   city;              // 'A', 'B', or 'C'
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
 * loads residents from a single CSV file into arr[] starting at start_index.
 * skips the header row and computes monthly_emission for each record.
 * returns the number of records loaded, or -1 on file error.
 */
int load_csv(const char* filename, Resident arr[], int start_index, char city_label) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "error: could not open file: " << filename << endl;
        return -1;
    }

    string line;
    getline(file, line); // skip header row

    int count = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        Resident& r = arr[start_index + count];

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
        count++;
    }

    file.close();
    return count;
}

// loads all three datasets into the array in order: A → B → C
int load_all_datasets(Resident arr[]) {
    int count_a = load_csv(DATASET_A, arr, 0, 'A');
    if (count_a < 0) count_a = 0;

    int count_b = load_csv(DATASET_B, arr, count_a, 'B');
    if (count_b < 0) count_b = 0;

    int count_c = load_csv(DATASET_C, arr, count_a + count_b, 'C');
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
    Resident residents[MAX_RECORDS];
    int total = load_all_datasets(residents);

    if (total <= 0) {
        cerr << "error: no records loaded. check dataset file paths." << endl;
        return 1;
    }

    /**
     * --- AYESHA ---
     * add age group categorization and carbon emission analysis here.
     * use: residents[], total
     */

    /**
     * --- WINGY ---
     * add sorting and searching experiments here.
     * use: residents[], total
     */

    return 0;
}
