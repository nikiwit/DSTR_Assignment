#include "array.hpp"

#include <iostream>  // cout, cerr
#include <fstream>   // ifstream (reading CSV files)
#include <sstream>   // stringstream (parsing CSV lines)
#include <cstring>   // strncpy (copying C-style strings)
#include <iomanip>   // setw, left (formatting console output)
#include <cctype>    // tolower
#include <sys/resource.h>  // getrusage (memory tracking)

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


// ---------- small helpers ----------

void copy_text(char dst[], const char* src) {
    strncpy(dst, src, MAX_TRANSPORT_LEN - 1);
    dst[MAX_TRANSPORT_LEN - 1] = '\0';
}

void swap_resident(Resident& a, Resident& b) {
    Resident temp = a;
    a = b;
    b = temp;
}

void copy_records(Resident dst[], const Resident src[], int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

bool same_ignore_case(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            return false;
        }
        i++;
    }
    return a[i] == b[i];
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

double get_memory_usage_mb() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return (double)usage.ru_maxrss / 1024.0;  // ru_maxrss is in KB on macOS
}


// ---------- normalize transport ----------

void normalize_transport(const char* input, char out[MAX_TRANSPORT_LEN]) {
    const char* modes[] = {
        "Car", "Bus", "Local Bus", "Bicycle", "Walking", "School Bus", "Carpool"
    };

    for (int i = 0; i < 7; i++) {
        if (same_ignore_case(input, modes[i])) {
            copy_text(out, modes[i]);
            return;
        }
    }

    copy_text(out, input);
}


// ---------- quick sort ----------

int partition_emission(Resident arr[], int lo, int hi) {
    double pivot = arr[hi].monthly_emission;
    int i = lo;

    for (int j = lo; j < hi; j++) {
        if (arr[j].monthly_emission <= pivot) {
            swap_resident(arr[i], arr[j]);
            i++;
        }
    }

    swap_resident(arr[i], arr[hi]);
    return i;
}

void quick_sort_by_emission(Resident arr[], int lo, int hi) {
    if (lo >= hi) return;

    int p = partition_emission(arr, lo, hi);
    quick_sort_by_emission(arr, lo, p - 1);
    quick_sort_by_emission(arr, p + 1, hi);
}

int partition_age(Resident arr[], int lo, int hi) {
    int pivot = arr[hi].age;
    int i = lo;

    for (int j = lo; j < hi; j++) {
        if (arr[j].age <= pivot) {
            swap_resident(arr[i], arr[j]);
            i++;
        }
    }

    swap_resident(arr[i], arr[hi]);
    return i;
}

void quick_sort_by_age(Resident arr[], int lo, int hi) {
    if (lo >= hi) return;

    int p = partition_age(arr, lo, hi);
    quick_sort_by_age(arr, lo, p - 1);
    quick_sort_by_age(arr, p + 1, hi);
}

int partition_transport(Resident arr[], int lo, int hi) {
    const char* pivot = arr[hi].transport;
    int i = lo;

    for (int j = lo; j < hi; j++) {
        if (strcmp(arr[j].transport, pivot) <= 0) {
            swap_resident(arr[i], arr[j]);
            i++;
        }
    }

    swap_resident(arr[i], arr[hi]);
    return i;
}

void quick_sort_by_transport(Resident arr[], int lo, int hi) {
    if (lo >= hi) return;

    int p = partition_transport(arr, lo, hi);
    quick_sort_by_transport(arr, lo, p - 1);
    quick_sort_by_transport(arr, p + 1, hi);
}


// ---------- display ----------

// shows first 5 + last record to confirm sort order at both ends
void display_first_records(Resident arr[], int n) {
    if (n == 0) return;
    print_table_header();
    int show = (n <= 6) ? n : 5;
    for (int i = 0; i < show; i++) {
        cout << "  [" << (i + 1) << "] ";
        print_record(arr[i]);
    }
    if (n > 6) {
        cout << "  ... (" << (n - 6) << " more records) ..." << endl;
        cout << "  [" << n << "] ";
        print_record(arr[n - 1]);
    }
}

// shows first 5 + last match when there are many results
void print_search_preview(Resident matches[], int count) {
    if (count == 0) { cout << "  (no matches found)" << endl; return; }
    print_table_header();
    int show = (count <= 6) ? count : 5;
    for (int i = 0; i < show; i++) print_record(matches[i]);
    if (count > 6) {
        cout << "  ... (" << (count - 6) << " more records) ..." << endl;
        print_record(matches[count - 1]);
    }
}

void sort_by_monthly_emission_merge_sort(Resident arr[], int n) {
    cout << "\n=============================================================" << endl;
    cout << "                     TASK 6: Sorting                       " << endl;
    cout << "=============================================================" << endl;
    cout << "\n--------------------------------------------------------------" << endl;
    cout << "  SORT: Monthly Emission  [Merge Sort / Ascending]" << endl;
    cout << "  Theoretical: O(n log n) time  |  O(n) space" << endl;
    cout << "--------------------------------------------------------------" << endl;

    double mem_before = get_memory_usage_mb();
    Timer t;
    t.start();

    Resident tmp[MAX_RECORDS];
    for (int width = 1; width < n; width *= 2) {
        for (int lo = 0; lo < n; lo += 2 * width) {
            int mid = (lo + width < n) ? lo + width : n;
            int hi  = (lo + 2 * width < n) ? lo + 2 * width : n;

            int i = lo, j = mid, k = lo;

            while (i < mid && j < hi) {
                if (arr[i].monthly_emission <= arr[j].monthly_emission)
                    tmp[k++] = arr[i++];
                else
                    tmp[k++] = arr[j++];
            }
            while (i < mid) tmp[k++] = arr[i++];
            while (j < hi)  tmp[k++] = arr[j++];

            for (int x = lo; x < hi; x++) {
                arr[x] = tmp[x];
            }
        }
    }

    double elapsed = t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    display_first_records(arr, n);
    cout << "  Measured sort time: " << fixed << setprecision(4) << elapsed << " ms  (n=" << n << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4)
         << (sizeof(tmp) / (1024.0 * 1024.0)) << " MB" << endl;
}

void sort_by_age_merge_sort(Resident arr[], int n) {
    cout << "\n--------------------------------------------------------------" << endl;
    cout << "  SORT: Age  [Bubble Sort / Ascending]" << endl;
    cout << "  Theoretical: O(n^2) time  |  O(1) space" << endl;
    cout << "--------------------------------------------------------------" << endl;

    double mem_before = get_memory_usage_mb();
    Timer t;
    t.start();

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j].age > arr[j + 1].age) {
                swap_resident(arr[j], arr[j + 1]);
            }
        }
    }

    double elapsed = t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    display_first_records(arr, n);
    cout << "  Measured sort time: " << fixed << setprecision(4) << elapsed << " ms  (n=" << n << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): O(1)" << endl;
}

void sort_by_transport_mode_merge_sort(Resident arr[], int n) {
    cout << "\n--------------------------------------------------------------" << endl;
    cout << "  SORT: Transport Mode  [Merge Sort / Alphabetical]" << endl;
    cout << "  Theoretical: O(n log n) time  |  O(n) space" << endl;
    cout << "--------------------------------------------------------------" << endl;

    double mem_before = get_memory_usage_mb();
    Timer t;
    t.start();

    Resident tmp[MAX_RECORDS];
    for (int width = 1; width < n; width *= 2) {
        for (int lo = 0; lo < n; lo += 2 * width) {
            int mid = (lo + width < n) ? lo + width : n;
            int hi  = (lo + 2 * width < n) ? lo + 2 * width : n;

            int i = lo, j = mid, k = lo;

            while (i < mid && j < hi) {
                if (strcmp(arr[i].transport, arr[j].transport) <= 0)
                    tmp[k++] = arr[i++];
                else
                    tmp[k++] = arr[j++];
            }
            while (i < mid) tmp[k++] = arr[i++];
            while (j < hi)  tmp[k++] = arr[j++];

            for (int x = lo; x < hi; x++) {
                arr[x] = tmp[x];
            }
        }
    }

    double elapsed = t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    display_first_records(arr, n);
    cout << "  Measured sort time: " << fixed << setprecision(4) << elapsed << " ms  (n=" << n << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4)
         << (sizeof(tmp) / (1024.0 * 1024.0)) << " MB" << endl;
}

void sort_by_monthly_emission_quick_sort(Resident arr[], int n) {
    cout << "\n--------------------------------------------------------------" << endl;
    cout << "  SORT: Monthly Emission  [Quick Sort / Ascending]" << endl;
    cout << "  Theoretical: O(n log n) avg time  |  O(log n) avg space" << endl;
    cout << "--------------------------------------------------------------" << endl;

    double mem_before = get_memory_usage_mb();
    Timer t;
    t.start();

    quick_sort_by_emission(arr, 0, n - 1);

    double elapsed = t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    display_first_records(arr, n);
    cout << "  Measured sort time: " << fixed << setprecision(4) << elapsed << " ms  (n=" << n << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): O(log n) avg stack space" << endl;
}
void sort_by_age_quick_sort(Resident arr[], int n) {
    cout << "\n--------------------------------------------------------------" << endl;
    cout << "  SORT: Age  [Quick Sort / Ascending]" << endl;
    cout << "  Theoretical: O(n log n) avg time  |  O(log n) avg space" << endl;
    cout << "--------------------------------------------------------------" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();
    quick_sort_by_age(arr, 0, n - 1);
    double elapsed = t.get_elapsed_ms();

    double mem_after = get_memory_usage_mb();

    display_first_records(arr, n);
    cout << "  Measured sort time: " << fixed << setprecision(4) << elapsed << " ms  (n=" << n << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): O(log n) avg stack space" << endl;
}

void sort_by_transport_mode_quick_sort(Resident arr[], int n) {
    cout << "\n--------------------------------------------------------------" << endl;
    cout << "  SORT: Transport Mode  [Quick Sort / Alphabetical]" << endl;
    cout << "  Theoretical: O(n log n) avg time  |  O(log n) avg space" << endl;
    cout << "--------------------------------------------------------------" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();
    quick_sort_by_transport(arr, 0, n - 1);
    double elapsed = t.get_elapsed_ms();

    double mem_after = get_memory_usage_mb();

    display_first_records(arr, n);
    cout << "  Measured sort time: " << fixed << setprecision(4) << elapsed << " ms  (n=" << n << ")" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): O(log n) avg stack space" << endl;
}

// ---------- search intro ----------

void searching_experiment_introduction() {
    cout << "\n=============================================================" << endl;
    cout << "              TASK 7: Searching Experiments                 " << endl;
    cout << "=============================================================" << endl;
    // cout << "\nSorted searches: sort a working copy of the array, then apply binary search." << endl;
    // cout << "  -> Sort time and binary search time are reported separately." << endl;
    // cout << "Unsorted searches: scan the array linearly — no pre-sorting required." << endl;
    // cout << "  -> Only linear scan time is reported.\n" << endl;
}

void search_experiment_age_group_sorted(Resident arr[], int n, int min_age, int max_age) {
    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Age Group " << min_age << "-" << max_age << "  [Sorted / Binary Search]" << endl;
    cout << "  Theoretical: O(n log n) sort + O(log n) binary search" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer sort_t;
    sort_t.start();

    Resident sorted[MAX_RECORDS];
    copy_records(sorted, arr, n);
    quick_sort_by_age(sorted, 0, n - 1);

    double sort_elapsed = sort_t.get_elapsed_ms();

    Timer search_t;
    search_t.start();

    int lo = 0, hi = n - 1, start = n;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (sorted[mid].age >= min_age) {
            start = mid;
            hi = mid - 1;
        } else {
            lo = mid + 1;
        }
    }

    Resident matches[MAX_RECORDS];
    int count = 0;
    for (int i = start; i < n && sorted[i].age <= max_age; i++) {
        matches[count++] = sorted[i];
    }

    double search_elapsed = search_t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    size_t estimated_bytes = 2ULL * MAX_RECORDS * sizeof(Resident);

    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Sort time  (copy + quick sort)  : " << fixed << setprecision(4) << sort_elapsed << " ms" << endl;
    cout << "  Search time (binary search)     : " << fixed << setprecision(4) << search_elapsed << " ms" << endl;
    cout << "  Total time                      : " << fixed << setprecision(4) << (sort_elapsed + search_elapsed) << " ms" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4)
         << (estimated_bytes / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "  Auxiliary space note: O(log n) stack space for quick sort" << endl;
    cout << "==============================================================" << endl;
}

void search_experiment_transport_mode_sorted(Resident arr[], int n, const char* transport_mode) {
    char key[MAX_TRANSPORT_LEN];
    normalize_transport(transport_mode, key);

    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Transport \"" << key << "\"  [Sorted / Binary Search]" << endl;
    cout << "  Theoretical: O(n log n) sort + O(log n) binary search" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer sort_t;
    sort_t.start();

    Resident sorted[MAX_RECORDS];
    copy_records(sorted, arr, n);
    quick_sort_by_transport(sorted, 0, n - 1);

    double sort_elapsed = sort_t.get_elapsed_ms();

    Timer search_t;
    search_t.start();

    int lo = 0, hi = n - 1, found = -1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        int cmp = strcmp(sorted[mid].transport, key);

        if (cmp == 0) {
            found = mid;
            break;
        } else if (cmp < 0) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }

    Resident matches[MAX_RECORDS];
    int count = 0;
    if (found != -1) {
        int start = found;
        while (start > 0 && strcmp(sorted[start - 1].transport, key) == 0) {
            start--;
        }
        for (int i = start; i < n && strcmp(sorted[i].transport, key) == 0; i++) {
            matches[count++] = sorted[i];
        }
    }

    double search_elapsed = search_t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    size_t estimated_bytes = 2ULL * MAX_RECORDS * sizeof(Resident);

    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Sort time  (copy + quick sort)  : " << fixed << setprecision(4) << sort_elapsed << " ms" << endl;
    cout << "  Search time (binary search)     : " << fixed << setprecision(4) << search_elapsed << " ms" << endl;
    cout << "  Total time                      : " << fixed << setprecision(4) << (sort_elapsed + search_elapsed) << " ms" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4)
         << (estimated_bytes / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "  Auxiliary space note: O(log n) stack space for quick sort" << endl;
    cout << "==============================================================" << endl;
}

void search_experiment_distance_threshold_sorted(Resident arr[], int n, double distance_threshold) {
    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Daily Distance > " << distance_threshold << " km  [Sorted / Binary Search]" << endl;
    cout << "  Theoretical: O(n log n) sort + O(log n) binary search" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer sort_t;
    sort_t.start();

    Resident sorted[MAX_RECORDS];
    copy_records(sorted, arr, n);

    Resident tmp[MAX_RECORDS];
    for (int width = 1; width < n; width *= 2) {
        for (int lo = 0; lo < n; lo += 2 * width) {
            int mid = (lo + width < n) ? lo + width : n;
            int hi  = (lo + 2 * width < n) ? lo + 2 * width : n;

            int i = lo, j = mid, k = lo;

            while (i < mid && j < hi) {
                if (sorted[i].daily_distance <= sorted[j].daily_distance)
                    tmp[k++] = sorted[i++];
                else
                    tmp[k++] = sorted[j++];
            }
            while (i < mid) tmp[k++] = sorted[i++];
            while (j < hi)  tmp[k++] = sorted[j++];

            for (int x = lo; x < hi; x++) {
                sorted[x] = tmp[x];
            }
        }
    }

    double sort_elapsed = sort_t.get_elapsed_ms();

    Timer search_t;
    search_t.start();

    int lo = 0, hi = n - 1, start = n;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (sorted[mid].daily_distance > distance_threshold) {
            start = mid;
            hi = mid - 1;
        } else {
            lo = mid + 1;
        }
    }

    Resident matches[MAX_RECORDS];
    int count = 0;
    for (int i = start; i < n; i++) {
        matches[count++] = sorted[i];
    }

    double search_elapsed = search_t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    size_t estimated_bytes = 3ULL * MAX_RECORDS * sizeof(Resident); // sorted + tmp + matches

    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Sort time  (copy + merge sort)  : " << fixed << setprecision(4) << sort_elapsed << " ms" << endl;
    cout << "  Search time (binary search)     : " << fixed << setprecision(4) << search_elapsed << " ms" << endl;
    cout << "  Total time                      : " << fixed << setprecision(4) << (sort_elapsed + search_elapsed) << " ms" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4)
         << (estimated_bytes / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "==============================================================" << endl;
}


// ---------- unsorted searches ----------

void search_experiment_age_group_unsorted(Resident arr[], int n, int min_age, int max_age) {
    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Age Group " << min_age << "-" << max_age << "  [Unsorted / Linear Search]" << endl;
    cout << "  Theoretical: O(n) time  |  O(1) space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();

    Resident matches[MAX_RECORDS];
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i].age >= min_age && arr[i].age <= max_age)
            matches[count++] = arr[i];
    }

    double elapsed = t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    size_t estimated_bytes = 1ULL * MAX_RECORDS * sizeof(Resident); // matches only

    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Linear search time              : " << fixed << setprecision(4) << elapsed << " ms" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4)
         << (estimated_bytes / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "==============================================================" << endl;
}

void search_experiment_transport_mode_unsorted(Resident arr[], int n, const char* transport_mode) {
    char key[MAX_TRANSPORT_LEN];
    normalize_transport(transport_mode, key);

    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Transport \"" << key << "\"  [Unsorted / Linear Search]" << endl;
    cout << "  Theoretical: O(n) time  |  O(1) space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();

    Resident matches[MAX_RECORDS];
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (strcmp(arr[i].transport, key) == 0)
            matches[count++] = arr[i];
    }

    double elapsed = t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    size_t estimated_bytes = 1ULL * MAX_RECORDS * sizeof(Resident); // matches only

    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Linear search time              : " << fixed << setprecision(4) << elapsed << " ms" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4)
         << (estimated_bytes / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "==============================================================" << endl;
}

void search_experiment_distance_threshold_unsorted(Resident arr[], int n, double distance_threshold) {
    cout << "\n==============================================================" << endl;
    cout << "  SEARCH: Daily Distance > " << distance_threshold << " km  [Unsorted / Linear Search]" << endl;
    cout << "  Theoretical: O(n) time  |  O(1) space" << endl;
    cout << "==============================================================" << endl;

    double mem_before = get_memory_usage_mb();

    Timer t;
    t.start();

    Resident matches[MAX_RECORDS];
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i].daily_distance > distance_threshold)
            matches[count++] = arr[i];
    }

    double elapsed = t.get_elapsed_ms();
    double mem_after = get_memory_usage_mb();

    size_t estimated_bytes = 1ULL * MAX_RECORDS * sizeof(Resident); // matches only

    print_search_preview(matches, count);
    print_line();
    cout << "  Matches : " << count << endl;
    cout << "  Linear search time              : " << fixed << setprecision(4) << elapsed << " ms" << endl;
    cout << "  Memory used (measured): " << fixed << setprecision(4) << (mem_after - mem_before) << " MB" << endl;
    cout << "  Memory used (estimated): " << fixed << setprecision(4)
         << (estimated_bytes / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "==============================================================" << endl;
}

void display_sorting_comparison_table() {
    cout << "\n=============================================================" << endl;
    cout << "       TASK 6: Sorting - Complexity Comparison              " << endl;
    cout << "=============================================================" << endl;

    cout << "\n  THEORETICAL TIME COMPLEXITY" << endl;
    cout << "=============================================================" << endl;
    cout << left
         << setw(14) << ""
         << setw(13) << "MS/Emission"
         << setw(13) << "MS/Age*"
         << setw(13) << "MS/Transport"
         << setw(13) << "QS/Emission"
         << setw(13) << "QS/Age"
         << "QS/Transport" << endl;
    cout << string(91, '-') << endl;
    cout << left << setw(14) << "Array"
         << setw(13) << "O(n log n)"
         << setw(13) << "O(n^2)"
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
    cout << "=============================================================" << endl;

    cout << left
         << setw(14) << ""
         << setw(13) << "MS/Emission"
         << setw(13) << "MS/Age*"
         << setw(13) << "MS/Transport"
         << setw(13) << "QS/Emission"
         << setw(13) << "QS/Age"
         << "QS/Transport" << endl;
    cout << string(91, '-') << endl;
    cout << left << setw(14) << "Array"
         << setw(13) << "O(n)"
         << setw(13) << "O(1)"
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
    cout << "  MS = Merge Sort | QS = Quick Sort" << endl;
    // cout << "  * Array MS/Age is Bubble Sort: time O(n^2), space O(1) in-place." << endl;
    // cout << "  Array MS: uses O(n) auxiliary tmp[] array. Array QS: in-place => O(log n) stack." << endl;
    // cout << "  LL MS: rewires node pointers => O(log n) stack. LL QS: data swaps in place => O(log n)." << endl;
    cout << "  Note: Numerical measures of time are printed above each sort result." << endl;
    cout << "========================================================================" << endl;
}

void display_searching_comparison_table() {
    cout << "\n=============================================================" << endl;
    cout << "     TASK 7: Searching - Complexity Comparison              " << endl;
    cout << "=============================================================" << endl;

    cout << "\n  THEORETICAL TIME COMPLEXITY" << endl;
    cout << "=============================================================" << endl;
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
    cout << "=============================================================" << endl;
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
    cout << "  Bin = Binary Search | Lin = Linear Search | Dist = Distance" << endl;
    // cout << "  * Array Binary: O(n log n) sort + O(log n) search => O(n log n) total." << endl;
    // cout << "  * LL Binary: O(n log n) sort + O(n log n) LL binary search => O(n log n) total." << endl;
    // cout << "  Array Binary space: O(n) copy array. LL Binary space: O(n) cloned list." << endl;
    // cout << "  Linear space: O(1) — scan only, no auxiliary storage." << endl;
    cout << "  Note: Numerical values of sort time, search time, and total are printed above each search result." << endl;
    cout << "========================================================================" << endl;
}

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
