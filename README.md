# DSTR_Assignment

Asia Pacific University | 2026

Comparative analysis of arrays and singly linked lists for sorting and searching in urban carbon emission data. Two separate C++ programs are required: one array-based and one linked list-based.

## Repository Structure

```text
/
├── documents/          # Assignment briefs and marking schemes
├── datasets/           # Input CSV datasets (City A, B, C)
└── src/
    ├── array.cpp       # Array-based program
    └── linked_list.cpp # Linked list-based program
```

## Task Distribution

| Member | Program | Responsibilities |
| ------ | ------- | ---------------- |
| Nik | Both | Parse all 3 CSV datasets and load into arrays and linked lists |
| Ayesha | Array | Age group categorization, carbon emission analysis, performance analysis, insights and recommendations |
| Vionna | Linked List | Age group categorization, carbon emission analysis, performance analysis, insights and recommendations |
| Wingy | Array | Sorting algorithms, searching algorithms |
| Mariia | Linked List | Sorting algorithms, searching algorithms |

## Assignment Scope

- **Data loading** — read datasets into self-implemented arrays and linked lists (no STL containers)
- **Age categorization** — group residents into 5 age bands; calculate preferred transport, total and average emissions per group
- **Carbon emission analysis** — totals per dataset, per transport mode, and across age groups
- **Sorting** — sort by age, daily distance, or carbon emission; at least one algorithm per program
- **Searching** — search by age group, transport mode, or distance threshold; multiple algorithms on sorted and unsorted data
- **Performance analysis** — record execution time and memory usage; compare arrays vs linked lists
- **Insights and recommendations** — identify high-emission groups and suggest sustainable commuting policies

## Datasets

| File | City | Description |
| ---- | ---- | ----------- |
| `datasets/dataset1_city_a.csv` | City A (Metropolitan) | Adult commuters, ages 18-60, car/bus/bicycle/walking |
| `datasets/dataset2_city_b.csv` | City B (University Town) | Students, ages 6-25, bicycle/walking/school bus/carpool |
| `datasets/dataset3_city_c.csv` | City C (Suburban/Rural) | All ages, car/local bus/bicycle |

## Branches

- `main` — stable, reviewed code only. Merge here when a feature is complete and tested.
- `development` — active development. Branch off here for new features or fixes, then merge back via pull request.

## Workflow

1. Clone the repo and switch to `development`.
2. Create a feature branch from `development` (e.g. `feature/array-sorting`).
3. Commit your changes with clear messages.
4. Open a pull request into `development` when ready for review.
5. `main` is updated from `development` once a milestone is stable.
