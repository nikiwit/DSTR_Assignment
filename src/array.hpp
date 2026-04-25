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
  * age group categorization + carbon emission analysis
 */
int get_age_group_index(int age);
void display_age_group_categorization(Resident arr[], int n);

void display_total_emission_per_dataset(Resident arr[], int n);
void display_emission_per_transport_mode(Resident arr[], int n);
void display_emission_comparison_by_dataset_and_age_group(Resident arr[], int n);
void display_transport_breakdown_by_age_group(Resident arr[], int n);

void sort_by_monthly_emission_quick_sort(Resident arr[], int n);
void sort_by_age_quick_sort(Resident arr[], int n);
void sort_by_transport_mode_quick_sort(Resident arr[], int n);
void sort_by_monthly_emission_merge_sort(Resident arr[], int n);
void sort_by_age_bubble_sort(Resident arr[], int n);
void sort_by_transport_mode_merge_sort(Resident arr[], int n);
void searching_experiment_introduction();
void search_experiment_age_group_sorted(Resident arr[], int n, int min_age, int max_age);
void search_experiment_transport_mode_sorted(Resident arr[], int n, const char* transport_mode);
void search_experiment_distance_threshold_sorted(Resident arr[], int n, double distance_threshold);
void search_experiment_age_group_unsorted(Resident arr[], int n, int min_age, int max_age);
void search_experiment_transport_mode_unsorted(Resident arr[], int n, const char* transport_mode);
void search_experiment_distance_threshold_unsorted(Resident arr[], int n, double distance_threshold);

// ─── complexity comparison tables ─────────────────────────────────────────────
void display_sorting_comparison_table();
void display_searching_comparison_table();

// closes the #ifndef guard — everything above is skipped if this header was already included
#endif
