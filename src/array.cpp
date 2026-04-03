#include "array.hpp"

#include <iostream>  // cout, cerr
#include <fstream>   // ifstream (reading CSV files)
#include <sstream>   // stringstream (parsing CSV lines)
#include <cstring>   // strncpy (copying C-style strings)
#include <iomanip>   // setw, left (formatting console output)

using namespace std;

// ─── data loading ─────────────────────────────────────────────────────────────

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
    cout << left << setw(23) << "City C (Suburban)"       << count_c << " records" << endl;
    cout << string(34, '-') << endl;
    cout << left << setw(23) << "Total" << count_a + count_b + count_c << " records" << endl;
    cout << "==================================" << "\n" << endl;

    return count_a + count_b + count_c;
}

// ─── sorting ──────────────────────────────────────────────────────────────────

/**
 * --- WINGY ---
 * implement sorting functions here.
 */

// ─── searching ────────────────────────────────────────────────────────────────

/**
 * --- WINGY ---
 * implement searching functions here.
 */

// ─── analysis ─────────────────────────────────────────────────────────────────

/**
 * --- AYESHA ---
 * implement age group categorization and carbon emission analysis here.
 */
