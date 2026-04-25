// include guard — prevents this header from being included more than once
#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "resident.hpp"

// ─── constants ────────────────────────────────────────────────────────────────

const int MAX_RECORDS = 500; // exact total across all 3 datasets

// ─── data loading ─────────────────────────────────────────────────────────────

/**
 * loads residents from a single CSV file into arr[] starting at start_index.
 * skips the header row and computes monthly_emission for each record.
 * returns the number of records loaded, or -1 on file error.
 */
int load_csv(const char* filename, Resident arr[], int start_index, char city_label);

// loads all three datasets into the array in order: A → B → C
int load_all_datasets(Resident arr[]);

// ─── sorting ──────────────────────────────────────────────────────────────────

/**
 * --- WINGY ---
 * declare sorting function prototypes here.
 * e.g. void bubble_sort_by_emission(Resident arr[], int n);
 */

// ─── searching ────────────────────────────────────────────────────────────────

/**
 * --- WINGY ---
 * declare searching function prototypes here.
 * e.g. int linear_search_by_age(Resident arr[], int n, int target_age);
 */

// ─── analysis ─────────────────────────────────────────────────────────────────

/**
 * --- AYESHA ---
 * declare age group categorization and carbon emission analysis prototypes here.
 */

// closes the #ifndef guard — everything above is skipped if this header was already included
#endif
