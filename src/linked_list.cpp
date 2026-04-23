#include "linked_list.hpp"

#include <iostream>  // cout, cerr
#include <fstream>   // ifstream (reading CSV files)
#include <sstream>   // stringstream (parsing CSV lines)
#include <cstring>   // strncpy (copying C-style strings)
#include <iomanip>   // setw, left (formatting console output)

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

// ─── sorting ──────────────────────────────────────────────────────────────────

/**
 * --- MARIIA ---
 * implement sorting functions here.
 */

// ─── searching ────────────────────────────────────────────────────────────────

/**
 * --- MARIIA ---
 * implement searching functions here.
 */

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

    char cities[] = {'A','B','C'};
    const char* labels[] = {"City A","City B","City C"};

    // store results first
    int counts[3][5][NUM_TRANSPORTS] = {0};
    double totals[3][5][NUM_TRANSPORTS] = {0};
    int residents[3][5] = {0};
    double emissions[3][5] = {0};

    Node* curr = list.getHead();

    // first pass: collect data
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

    // second pass: clean output
    for (int c = 0; c < 3; c++) {
        for (int a = 0; a < 5; a++) {

            if (residents[c][a] == 0) continue; // skip empty groups

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