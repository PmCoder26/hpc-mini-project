# Technology Stack & Technical Details

This document outlines the specific technologies, tools, and technical algorithms that will be used to implement the **HPC Enterprise HR & Payroll Analytics Engine**.

## 1. Primary Technology Stack

*   **Core Execution Language:** C++ (Standard C++17)
    *   *Why:* C++ provides absolute control over memory management (using raw arrays and pointers) and zero-overhead abstractions, which is critical for measuring raw computational speedup in High Performance Computing.
*   **Parallelization Framework:** OpenMP (Open Multi-Processing)
    *   *Why:* OpenMP is the industry standard for shared-memory multiprocessing programming in C/C++. It allows us to parallelize loops and regions using compiler pragmas (`#pragma omp ...`) without manually managing raw POSIX threads (pthreads).
*   **Compiler:** GCC / G++ (GNU Compiler Collection) or Clang
    *   *Why:* We must compile the code with the `-fopenmp` or `-Xpreprocessor -fopenmp` flag to activate the parallel threads.
*   **Data Generation:** Python 3
    *   *Why:* We will write a fast Python script utilizing the `Faker` library and `csv` module to generate the massive 100+ million rows of synthetic employee data. Python is significantly faster for text-generation than C++.

## 2. Technical Implementation Details

### A. Memory Management (The "In-Memory" Engine)
Rather than writing to disk pages, the engine will read the CSV files directly into RAM on startup. 
*   **Data Structures:** Data will be stored in highly cache-efficient `std::vector<Struct>` or contiguous single-dimensional arrays. 
*   *Example:* 
    ```cpp
    struct AttendanceLog {
        int log_id;
        int emp_id;
        int timestamp;
        float hours;
    };
    std::vector<AttendanceLog> logs; // 150 million items contiguous in RAM
    ```

### B. Parallel Algorithms Implemented (Phase 1-7)

To perform Database Query Optimization, we implemented parallel versions of 9 Relational Algebra operations:

1.  **Parallel Filtering (The `WHERE` clause):**
    *   *OpenMP mechanism:* `#pragma omp parallel for` with Thread-Local Vectors and `#pragma omp critical` recombinations.
2.  **Parallel Aggregation (The `SUM`, `AVG`, `COUNT` clauses):**
    *   *OpenMP mechanism:* `#pragma omp parallel for reduction(+:total_bonus)` avoiding Race Conditions.
3.  **Parallel Join (The `HASH JOIN` clause):**
    *   *Implementation:* Sequential Hash build followed by Parallel read-only concurrent Cache lookups.
4.  **Parallel Sorting (The `ORDER BY` clause):**
    *   *OpenMP mechanism:* OpenMP Tasks (`#pragma omp task` and `taskwait`). Threads recursively chunk the vectors memory.
5.  **Multi-Table Aggregation (`GROUP BY`):**
    *   *Implementation:* Array chunking dynamically generating Thread-Local Hash Maps that are reduced globally.
6.  **Deep Temporal Filtering (`HAVING COUNT`):**
    *   *Implementation:* Multi-threaded log parsing using lock-free thread array frequency calculations.
7.  **Multi-Phase Subqueries (The nested `SELECT`):**
    *   *Implementation:* Launching a parallel reduction, creating an implicit Barrier Sync, and immediately triggering array reads based on the aggregated state.
8.  **Anomaly Detection (`ANTI-JOIN`):**
    *   *Implementation:* Extracting parallel misses in Set Differences (`std::unordered_map::find == end()`).
9.  **Parallel Text Search (`LIKE`):**
    *   *Implementation:* Running raw `O(N*M)` string traversal dynamically chunked over processing nodes.

## 3. Benchmarking & Metrics Gathering

To prove the project is an HPC success, the system will record execution times using high-precision clocks.

*   **Timing Library:** `<chrono>` (specifically `std::chrono::high_resolution_clock`).
*   **Calculations:**
    *   **Speedup ($S$):** $T_{serial} / T_{parallel}$
    *   **Efficiency ($E$):** $Speedup / \text{Number of Cores}$
    *   **Amdahl's Law Baseline:** Estimating the maximum theoretical speedup based on the strict serial portion of our file-reading logic.

## 4. Run/Execution Environment
*   **OS:** macOS / Linux (Unix terminal required for easy Compiler flag usage).
*   **Compilation Command:** `g++ -O3 -fopenmp main.cpp -o hpc_engine` (Note: `-O3` ensures compiler optimizations are active).
