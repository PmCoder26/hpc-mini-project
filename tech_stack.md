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

### B. Parallel Algorithms to be Implemented

To perform Database Query Optimization, we must implement parallel versions of standard Relational Algebra operations:

1.  **Parallel Filtering (The `WHERE` clause):**
    *   *Technique:* Array chunking.
    *   *OpenMP mechanism:* `#pragma omp parallel for`. The array of size `N` is split among `T` threads. Each thread filters its chunk of data `N/T`.
2.  **Parallel Aggregation (The `SUM`, `AVG`, `COUNT` clauses):**
    *   *Technique:* Map-Reduce logic. 
    *   *OpenMP mechanism:* `#pragma omp parallel for reduction(+:total_bonus)`. This allows threads to safely add to a global total without locking mechanisms (avoiding Race Conditions).
3.  **Parallel Join (The `JOIN` clause):**
    *   *Technique:* Parallel Hash Join.
    *   *Implementation:* 
        1. *Build Phase:* Sequentially or partially parallel build a Hash Map of the smaller table (e.g., `Departments`).
        2. *Probe Phase:* `#pragma omp parallel for` over the massive table (e.g., `Employees`) where multiple threads read the Hash Map concurrently.
4.  **Parallel Sorting (The `ORDER BY` clause):**
    *   *Technique:* Parallel Merge Sort.
    *   *OpenMP mechanism:* OpenMP Tasks (`#pragma omp task`). Threads recursively split the array into halves until a base threshold, sort them, and then merge them back.

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
