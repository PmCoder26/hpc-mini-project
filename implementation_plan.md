# System Design: HPC Enterprise HR & Payroll Analytics Engine

This document outlines the scope, data models, and execution flow for your High Performance Computing (HPC) mini-project: **Implement Parallelization of Database Query Optimization via an Enterprise HR & Payroll Analytics System**.

## 1. Project Scope

The project will simulate a massive, enterprise-level Human Resources and Payroll Data Warehouse. To demonstrate the necessity and power of parallelization, we will bypass standard SQL databases and build a custom, in-memory query execution engine using C++ and OpenMP capable of handling extremely complex, multi-table joins and data aggregations.

**In-Scope:**
*   **Massive Data Generation:** A script to generate extremely large synthetic datasets (e.g., 5-10 million employee records).
*   **In-Memory Database Engine:** A C++ engine that loads the data into arrays of structs to simulate database tables.
*   **Parallel Query Operators:** Building algorithm implementations of database operations (Filtering, Sorting, Aggregation, Grouping, Joining).
*   **Serial vs. Parallel Comparison:** Every operation will have a Serial (1 thread) and Parallel (N threads) implementation to calculate speedup and Amdahl's Law metrics.
*   **Basic Query Optimizer:** A mechanism that evaluates parallel execution plans (e.g., deciding whether to filter before joining or join before filtering).

**Out-of-Scope:**
*   A Graphical User Interface (GUI) or Frontend Web App (this is an HPC backend project).
*   Persistent disk-page management (data is loaded into RAM).
*   A full SQL string parser (we will trigger queries via command line flags or predefined C++ functions).

---

## 2. Expanded Data Model Schema

To support complex queries (Multi-way Joins, Deep Aggregates, Time-Series Filtering), we will use an expanded relational schema reflecting a real corporate data warehouse.

### Table 1: `Employees` (Target: 5,000,000 rows)
*   **`emp_id`** (Integer, Primary Key)
*   **`name`** (String)
*   **`age`** (Integer)
*   **`department_id`** (Integer, Foreign Key)
*   **`base_salary`** (Float)
*   **`hire_year`** (Integer)
*   **`performance_score`** (Integer, 1-100)

### Table 2: `Departments` (Target: 100 rows)
*   **`dept_id`** (Integer, Primary Key)
*   **`dept_name`** (String)
*   **`region`** (String)

### Table 3: `Payroll_Transactions` (Target: 60,000,000 rows)
*   **`transaction_id`** (Integer, Primary Key)
*   **`emp_id`** (Integer, Foreign Key)
*   **`fiscal_month`** (Integer)
*   **`bonus_amount`** (Float)
*   **`tax_deduction`** (Float)
*   **`net_paid`** (Float)

### Table 4: `Attendance_Logs` (Target: 150,000,000 rows)
*   **`log_id`** (Integer, Primary Key)
*   **`emp_id`** (Integer, Foreign Key)
*   **`date_timestamp`** (Integer)
*   **`hours_worked`** (Float)
*   **`status`** (String: "Present", "Absent", "Leave")

### Table 5: `Employee_Skills` (Target: 15,000,000 rows)
*   **`emp_id`** (Integer, Foreign Key)
*   **`skill_name`** (String)
*   **`proficiency_level`** (Integer, 1-5)

---

## 3. System Flow Architecture

The operational flow of the system during a demo or benchmark will follow these phases:

### Phase 1: Ingestion & Setup
1.  **Data Generation:** Run Python script once to generate massive CSV files.
2.  **Memory Load:** The C++ program starts and parses the CSV files, loading rows into contiguous memory blocks (`std::vector` of structs). 

### Phase 2: The Complex Query Inputs
The user selects a massive query to run. The complexity of these queries justifies the need for HPC:
*   *Query A (Deep Aggregation):* Calculate the total `net_paid` and average `bonus_amount` across all 60 million payroll records, grouped by `fiscal_month`.
*   *Query B (Time-Series Filter):* Scan 150 million `Attendance_Logs` to find the `emp_id` of everyone who worked more than 200 hours in month 12.
*   *Query C (Complex Multi-Way Join):* Find the names of Employees in the "Engineering" Department who have "C++" skill `proficiency_level == 5` and received a `bonus_amount` > $5000. (Requires joining Employees, Departments, Skills, and Payroll!).
*   *Query D (Sorting massive results):* Sort the results of Query C by `net_paid` descending using Parallel Merge Sort.

### Phase 3: The Optimizer & Execution (The HPC Core)
1.  **Thread Setup:** The system reads the `OMP_NUM_THREADS` environment variable to determine how many CPU cores to use.
2.  **Execution:** The system runs the chosen query through the OpenMP enabled algorithms:
    *   `#pragma omp parallel for` (for array chunking during filters).
    *   `#pragma omp parallel for reduction` (for safe aggregations).
    *   Parallel Hash-building (for joins).
    *   Parallel Merge-Sort (for ordering).

### Phase 4: Benchmarking and Performance Comparison
An integral part of this HPC project is demonstrating the hardware efficiency gained through OpenMP. For every database query executed, the system will systematically perform a rigorous comparison:

1.  **Sequential Execution:** The query is executed first using a standard, single-threaded algorithm (Sequential Algo). We will record the precise wall-clock execution time.
2.  **Parallel Execution:** The identical query is executed using our OpenMP optimized multi-threaded algorithms. We will record the execution time across `N` threads.
3.  **Validation & Metrics:** The system will verify that the parallel result exactly matches the sequential result, and then calculate High Performance Computing metrics like Speedup ($S = T_{serial} / T_{parallel}$) and Efficiency.

**Example Output:**
```text
Query Execution Complete.
-------------------------
Operation:       Filter & Aggregate
Rows processed:  10,000,000
Threads used:    8
Sequential Time: 1.45 seconds
Parallel Time:   0.22 seconds
Speedup Factor:  6.5x
Efficiency:      81.25%
-------------------------
Data Validated:  PASS (Results match)
```

---

## User Review Required

> [!IMPORTANT]
> Please review the defined scope and models. 
> 
> *   Does this scale of data and schema look acceptable for your mini-project requirements?
> *   Do you want to focus heavily on the C++ OpenMP implementation, or do you have a different language in mind?
> *   If approved, our next steps will be to generate the Python mock-data script. Do you approve moving forward with implementation?
