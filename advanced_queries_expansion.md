# Phase 7: Advanced Query Expansion

To scale the scope and make the project significantly more enterprise-grade, we will introduce 6 highly complex new database operations. These highlight different kinds of HPC architecture limits beyond simple array chunking.

## Proposed New Queries

### 1. Parallel Merge Sort (`ORDER BY`)
*   **The Query:** Order the entire `Employees` table by `base_salary` descending.
*   **HPC Challenge:** Sorting cannot be done with a simple `#pragma omp parallel for`. It requires OpenMP **Tasking** (`#pragma omp task`) to recursively split the array into halves, sort them concurrently across different threads, and merge them back together safely.

### 2. Multi-Table Aggregation (`JOIN` + `GROUP BY`)
*   **The Query:** Calculate the sum of all `base_salary` **grouped by** `department_name`.
*   **HPC Challenge:** This requires reading from the `Employees` array, looking up the `Department` Hash Map, and safely reducing the sums into a shared frequency map without causing Data Race collisions between threads.

### 3. Deep Temporal Filtering (`HAVING COUNT`)
*   **The Query:** Scan the massive `Attendance` log table to find the IDs of all employees who have been marked "Absent" more than `N` times.
*   **HPC Challenge:** Threads must build partial Thread-Local Histograms (frequency tables of absences) and then safely combine all 8 thread histograms into one global Master Histogram.

### 4. Multi-Phase Subqueries (The `IN` or nested `SELECT`)
*   **The Query:** Find all employees working in a 'Europe' Department whose `performance_score` is STRICTLY LESS THAN the company-wide average performance.
*   **HPC Challenge:** Execution depends on dependencies. The engine must first launch a parallel reduction to calculate the company average, set a memory barrier, and then immediately broadcast that average to a second wave of parallel threads checking the condition.

### 5. Anomaly Detection (`ANTI-JOIN`)
*   **The Query:** Scan the `Attendance` log to find any "Ghost Logs" (Logs belonging to an `emp_id` that does NOT exist in the `Employees` table).
*   **HPC Challenge:** It requires executing a parallel Set Difference ($A - B$). Threads must concurrently probe the Hash Map and extract the misses rather than the hits.

### 6. Parallel Text Semantic Searching (`LIKE '%str%'`)
*   **The Query:** Find all Employees whose `name` contains the string "Smith" and `base_salary > 100000`.
*   **HPC Challenge:** String traversal `std::string::find` is vastly more CPU-intensive than integer math. This will demonstrate how beautifully OpenMP scales when the workload per-row is extremely heavy.

---

## Code Execution Plan

#### 1. Modify `SerialAlgorithms` / `ParallelAlgorithms`
*   Add `executeMergeSort()`
*   Add `groupSalaryByDepartment()`
*   Add `findHighAbsenceEmployees()`
*   Add `runSubqueryAnalysis()`
*   Add `runAntiJoin()`
*   Add `runTextSearch()`

#### 2. Modify `QueryOptimizer`
*   Add the 6 target functions mapping to the underlying Serial/Parallel files to calculate benchmarking stats.

#### 3. Modify `CLIController`
*   Expand the terminal menu to support a massive list of **9 total analytics options**.
