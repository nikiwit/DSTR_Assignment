#include "linked_list.hpp"

#include <iostream>  // cout, cerr

using namespace std;

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
     * call age group categorization and carbon emission analysis here.
     * use: residents.head to traverse, residents.size for total count
     */

    // task 4
    display_age_group_categorization(residents);

    // task 5
    display_total_emission_per_dataset(residents);
    display_emission_per_transport_mode(residents);
    display_emission_comparison_by_dataset_and_age_group(residents);
    display_transport_breakdown_by_age_group(residents);

    /**
     * --- MARIIA ---
     * call sorting and searching experiments here.
     * use: residents.head to traverse, residents.size for total count
     *
     * experiment guide (the "science experiment" approach):
     *
     * 1. hypothesis — before each test, print what you expect to happen and why.
     *    e.g. "we expect merge sort O(n log n) to be faster than insertion sort O(n^2)"
     *
     * 2. lab data — use Timer to measure execution time in ms.
     *    Timer t;
     *    t.start();
     *    // ... run the sort or search ...
     *    cout << "time: " << t.get_elapsed_ms() << " ms" << endl;
     *
     * 3. justification — after each test, explain the result using Big O.
     *    if the result surprises you, explain why.
     *
     * design your own tests. vary these:
     *   - algorithm (e.g. insertion sort vs merge sort)
     *   - data state (sorted, random, reverse)
     *   - search target (existing vs non-existing value)
     */

    sort_by_monthly_emission_merge_sort(residents);
    sort_by_age_merge_sort(residents);
    sort_by_transport_mode_merge_sort(residents);
    sort_by_monthly_emission_quick_sort(residents);
    sort_by_age_quick_sort(residents);
    sort_by_transport_mode_quick_sort(residents);

    searching_experiment_introduction();
    search_experiment_age_group_sorted(residents, 20, 29);
    search_experiment_transport_mode_sorted(residents, "car");
    search_experiment_distance_threshold_sorted(residents, 10.0);
    search_experiment_age_group_unsorted(residents, 20, 29);
    search_experiment_transport_mode_unsorted(residents, "car");
    search_experiment_distance_threshold_unsorted(residents, 10.0);

    display_sorting_comparison_table();
    display_searching_comparison_table();

    residents.clear(); // free all heap memory before exit
    return 0;
}
