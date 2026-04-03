// include guard — prevents this header from being included more than once
#ifndef RESIDENT_HPP
#define RESIDENT_HPP

#include <ctime>  // clock(), CLOCKS_PER_SEC (timing)

// ─── constants ────────────────────────────────────────────────────────────────

const int MAX_ID_LEN        = 10;
const int MAX_TRANSPORT_LEN = 20;

// dataset paths — run the program from the repository root
const char* const DATASET_A = "../datasets/dataset1_city_a.csv";
const char* const DATASET_B = "../datasets/dataset2_city_b.csv";
const char* const DATASET_C = "../datasets/dataset3_city_c.csv";

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
 * call start() before the operation, then get_elapsed_ms() after.
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

// closes the #ifndef guard — everything above is skipped if this header was already included
#endif
