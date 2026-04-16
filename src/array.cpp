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
 * Age Group Categorization
 */

// return the array index for the age group
int get_age_group_index(int age) {
    if (age >= 6 && age <= 17)
        return 0;
    else if (age >= 18 && age <= 25)
        return 1;
    else if (age >= 26 && age <= 45)
        return 2;
    else if (age >= 46 && age <= 60)
        return 3;
    else if (age >= 61 && age <= 100)
        return 4;
    else
        return -1; // invalid or if it is out of required range
}

// helper function to copy age group label into a char array
void copy_age_group_label(int group_index, char label[]) {
    if (group_index == 0)
        strcpy(label, "6-17 : Children & Teenagers");
    else if (group_index == 1)
        strcpy(label, "18-25 : University Students / Young Adults");
    else if (group_index == 2)
        strcpy(label, "26-45 : Working Adults - Early Career");
    else if (group_index == 3)
        strcpy(label, "46-60 : Working Adults - Late Career");
    else if (group_index == 4)
        strcpy(label, "61-100 : Senior Citizens / Retirees");
    else
        strcpy(label, "Unknown");
}

// helper function to convert transport mode into index
int get_transport_index(const char transport[]) {
    if (strcmp(transport, "Car") == 0)
        return 0;
    else if (strcmp(transport, "Bus") == 0)
        return 1;
    else if (strcmp(transport, "Local Bus") == 0)
        return 2;
    else if (strcmp(transport, "Bicycle") == 0)
        return 3;
    else if (strcmp(transport, "Walking") == 0)
        return 4;
    else if (strcmp(transport, "School Bus") == 0)
        return 5;
    else if (strcmp(transport, "Carpool") == 0)
        return 6;
    else
        return -1;
}

// helper function to copy transport label into char array
void copy_transport_label(int transport_index, char label[]) {
    if (transport_index == 0)
        strcpy(label, "Car");
    else if (transport_index == 1)
        strcpy(label, "Bus");
    else if (transport_index == 2)
        strcpy(label, "Local Bus");
    else if (transport_index == 3)
        strcpy(label, "Bicycle");
    else if (transport_index == 4)
        strcpy(label, "Walking");
    else if (transport_index == 5)
        strcpy(label, "School Bus");
    else if (transport_index == 6)
        strcpy(label, "Carpool");
    else
        strcpy(label, "Unknown");
}

// display age group categorization result
void display_age_group_categorization(Resident arr[], int n) {
    const int AGE_GROUPS = 5;
    const int TRANSPORT_TYPES = 7;

    int resident_count[AGE_GROUPS] = {0};
    double total_emission[AGE_GROUPS] = {0};

    // store count of each transport type inside each age group
    int transport_count[AGE_GROUPS][TRANSPORT_TYPES] = {0};

    // step 1: go through all residents
    for (int i = 0; i < n; i++) {
        int group_index = get_age_group_index(arr[i].age);

        if (group_index != -1) {
            resident_count[group_index]++;
            total_emission[group_index] += arr[i].monthly_emission;

            int transport_index = get_transport_index(arr[i].transport);
            if (transport_index != -1) {
                transport_count[group_index][transport_index]++;
            }
        }
    }

    // step 2: display result
    cout << "\n**********---- TASK 4: Age Group Categorization ----**********" << endl;
    cout << "\n==============================================================" << endl;
    cout << "               AGE GROUP CATEGORIZATION RESULT                " << endl;
    cout << "==============================================================" << endl;

    cout << left
         << setw(45) << "Age Group"
         << setw(12) << "Residents"
         << setw(30) << "Most Preferred Transport"
         << setw(25) << "Total Emission"
        << setw(30) << "Avg Emission per Resident" << endl;

    cout << "--------------------------------------------------------------------"
         << "--------------------------------------------------------------------" << endl;

    for (int i = 0; i < AGE_GROUPS; i++) {
        char age_label[60];
        char preferred_transport[20];

        copy_age_group_label(i, age_label);

        // find most preferred mode of transport
        int max_count = 0;
        int preferred_index = -1;

        for (int j = 0; j < TRANSPORT_TYPES; j++) {
            if (transport_count[i][j] > max_count) {
                max_count = transport_count[i][j];
                preferred_index = j;
            }
        }

        if (preferred_index != -1)
            copy_transport_label(preferred_index, preferred_transport);
        else
            strcpy(preferred_transport, "No Data");

        double average_emission = 0.0;
        if (resident_count[i] > 0) {
            average_emission = total_emission[i] / resident_count[i];
        }

        
        cout << left
             << setw(45) << age_label
            << setw(12) << resident_count[i]
            << setw(30) << preferred_transport
            << setw(25) << fixed << setprecision(2) << total_emission[i]
            << setw(30) << fixed << setprecision(2) << average_emission
         << endl;
    }

    cout << "==============================================================" << endl;
}

// helper function to convert city label into array index
int get_city_index(char city) {
    if (city == 'A')
        return 0;
    else if (city == 'B')
        return 1;
    else if (city == 'C')
        return 2;
    else
        return -1;
}

// helper function to copy city label into char array
void copy_city_label(char city, char label[]) {
    if (city == 'A')
        strcpy(label, "City A");
    else if (city == 'B')
        strcpy(label, "City B");
    else if (city == 'C')
        strcpy(label, "City C");
    else
        strcpy(label, "Unknown");
}

// 5a. display total carbon emission per dataset
void display_total_emission_per_dataset(Resident arr[], int n) {
    double city_total[3] = {0};

    for (int i = 0; i < n; i++) {
        int city_index = get_city_index(arr[i].city);
        if (city_index != -1) {
            city_total[city_index] += arr[i].monthly_emission;
        }
    }
    cout << "\n**********---- TASK 5: Carbon Emission Analysis ----**********" << endl;
    cout << "\n============================================================" << endl;
    cout << "            5a: TOTAL CARBON EMISSION PER DATASET                " << endl;
    cout << "==============================================================" << endl;

    cout << left
         << setw(15) << "Dataset"
         << setw(25) << "Total Emission (kg CO2)" << endl;

    cout << "--------------------------------------------------------------" << endl;

    for (int i = 0; i < 3; i++) {
        char label[20];
        copy_city_label('A' + i, label);

    cout << left
         << setw(15) << label
         << setw(25) << fixed << setprecision(2) << city_total[i]
         << endl;
    }

    cout << "==============================================================" << endl;
}

// 5b. display carbon emission per mode of transport
void display_emission_per_transport_mode(Resident arr[], int n) {
    const int TRANSPORT_TYPES = 7;

    int transport_count[TRANSPORT_TYPES] = {0};
    double transport_total[TRANSPORT_TYPES] = {0};

    for (int i = 0; i < n; i++) {
        int transport_index = get_transport_index(arr[i].transport);

        if (transport_index != -1) {
            transport_count[transport_index]++;
            transport_total[transport_index] += arr[i].monthly_emission;
        }
    }

    cout << "\n============================================================" << endl;
    cout << "           5b:  CARBON EMISSION PER MODE OF TRANSPORT            " << endl;
    cout << "==============================================================" << endl;

    cout << left
         << setw(25) << "Mode of Transport"
         << setw(12) << "Count"
         << setw(25) << "Total Emission (kg CO2)"
         << setw(25) << "Avg per Resident" << endl;

    cout << "--------------------------------------------------------------"
         << "------------------------------" << endl;

    
    for (int i = 0; i < TRANSPORT_TYPES; i++) {

    // only show transport modes that are actually used
    if (transport_count[i] > 0) {

        char transport_label[20];
        copy_transport_label(i, transport_label);

        double average_emission = 0.0;
        average_emission = transport_total[i] / transport_count[i];

        cout << left
             << setw(25) << transport_label
             << setw(12) << transport_count[i]
             << setw(25) << fixed << setprecision(2) << transport_total[i]
             << setw(25) << fixed << setprecision(2) << average_emission
             << endl;
    }
}

    cout << "==============================================================" << endl;
}

// 5c. compare emissions across datasets and age groups
void display_emission_comparison_by_dataset_and_age_group(Resident arr[], int n) {
    const int AGE_GROUPS = 5;

    double emission_table[3][AGE_GROUPS] = {0};

    for (int i = 0; i < n; i++) {
        int city_index = get_city_index(arr[i].city);
        int age_group_index = get_age_group_index(arr[i].age);

        if (city_index != -1 && age_group_index != -1) {
            emission_table[city_index][age_group_index] += arr[i].monthly_emission;
        }
    }

    cout << "\n============================================================" << endl;
    cout << "          5c: EMISSION COMPARISON BY DATASET AND AGE GROUP        " << endl;
    cout << "==============================================================" << endl;

    cout << left
         << setw(10) << "Dataset"
         << setw(12) << "6-17"
         << setw(12) << "18-25"
         << setw(12) << "26-45"
         << setw(12) << "46-60"
         << setw(12) << "61-100" << endl;

    cout << "--------------------------------------------------------------"
         << "------------" << endl;

    cout << left
         << setw(10) << "City A"
         << setw(12) << fixed << setprecision(2) << emission_table[0][0]
         << setw(12) << fixed << setprecision(2) << emission_table[0][1]
         << setw(12) << fixed << setprecision(2) << emission_table[0][2]
         << setw(12) << fixed << setprecision(2) << emission_table[0][3]
         << setw(12) << fixed << setprecision(2) << emission_table[0][4]
         << endl;

    cout << left
         << setw(10) << "City B"
         << setw(12) << fixed << setprecision(2) << emission_table[1][0]
         << setw(12) << fixed << setprecision(2) << emission_table[1][1]
         << setw(12) << fixed << setprecision(2) << emission_table[1][2]
         << setw(12) << fixed << setprecision(2) << emission_table[1][3]
         << setw(12) << fixed << setprecision(2) << emission_table[1][4]
         << endl;

    cout << left
         << setw(10) << "City C"
         << setw(12) << fixed << setprecision(2) << emission_table[2][0]
         << setw(12) << fixed << setprecision(2) << emission_table[2][1]
         << setw(12) << fixed << setprecision(2) << emission_table[2][2]
         << setw(12) << fixed << setprecision(2) << emission_table[2][3]
         << setw(12) << fixed << setprecision(2) << emission_table[2][4]
         << endl;

    cout << "==============================================================" << endl;
}

// extra detailed table:
// shows breakdown of carbon emissions by transport mode
// for each age group inside each dataset

void display_transport_breakdown_by_age_group(Resident arr[], int n) {
    const int AGE_GROUPS = 5;
    const int TRANSPORT_TYPES = 7;

    // loop through each dataset: A, B, C
    for (int city = 0; city < 3; city++) {

        cout << "\n" << string(50, '*') << endl;
        char city_char = 'A' + city;
        char city_label[20];
        copy_city_label(city_char, city_label);

        // loop through each age group
        for (int age_group = 0; age_group < AGE_GROUPS; age_group++) {
            int transport_count[TRANSPORT_TYPES] = {0};
            double transport_total[TRANSPORT_TYPES] = {0};
            int total_residents = 0;
            double total_emission = 0.0;

            // collect data only for this dataset and this age group
            for (int i = 0; i < n; i++) {
                if (arr[i].city == city_char && get_age_group_index(arr[i].age) == age_group) {
                    total_residents++;
                    total_emission += arr[i].monthly_emission;

                    int transport_index = get_transport_index(arr[i].transport);
                    if (transport_index != -1) {
                        transport_count[transport_index]++;
                        transport_total[transport_index] += arr[i].monthly_emission;
                    }
                }
            }

            // skip the empty age groups in datasets
            if (total_residents == 0) {
                continue;
            }

            // find most preferred transport
            int max_count = 0;
            int preferred_index = -1;

            for (int j = 0; j < TRANSPORT_TYPES; j++) {
                if (transport_count[j] > max_count) {
                    max_count = transport_count[j];
                    preferred_index = j;
                }
            }

            char preferred_transport[20];
            if (preferred_index != -1)
                copy_transport_label(preferred_index, preferred_transport);
            else
                strcpy(preferred_transport, "No Data");

            // get age group label
            char age_label[60];
            copy_age_group_label(age_group, age_label);

            // display output
            cout << "\nDataset: " << city_label << endl;
            cout << "Age Group: " << age_label << endl;
            cout << string(90, '-') << endl;

            cout << left
                 << setw(25) << "Mode of Transport"
                 << setw(10) << "Count"
                 << setw(28) << "Total Emission (kg CO2)"
                 << setw(25) << "Average/Resident" << endl;

            for (int j = 0; j < TRANSPORT_TYPES; j++) {
                if (transport_count[j] > 0) {
                    char transport_label[20];
                    copy_transport_label(j, transport_label);

                    double average_emission = transport_total[j] / transport_count[j];

                    cout << left
                         << setw(25) << transport_label
                         << setw(10) << transport_count[j]
                         << setw(28) << fixed << setprecision(2) << transport_total[j]
                         << setw(25) << fixed << setprecision(2) << average_emission
                         << endl;
                }
            }

            double average_for_age_group = total_emission / total_residents;

            cout << string(90, '-') << endl;
            cout << "Total Residents in Age Group : " << total_residents << endl;
            cout << "Most Preferred Mode         : " << preferred_transport << endl;
            cout << "Total Emission for Age Group: " << fixed << setprecision(2)
                 << total_emission << " kg CO2" << endl;
            cout << "Average Emission per Resident: " << fixed << setprecision(2)
                 << average_for_age_group << " kg CO2" << endl;

            // . separator between age groups in same dataset (for easier reading)
            cout << endl;
            cout << string(50, '.') << endl;
        }
    }
}
