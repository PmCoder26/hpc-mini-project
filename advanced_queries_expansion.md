# Phase 7: Advanced Query Expansion

To scale the scope and make the project significantly more enterprise-grade, we propose integrating more complex database operations utilizing pure PyCUDA hardware acceleration. These highlight different kinds of HPC architecture challenges (like warp-divergence and shared-memory constraints) far beyond simple block thread filtering.

## Proposed New PyCUDA Queries

### 1. Parallel Radix Sort (`ORDER BY`)
*   **The Query:** Order the entire `Employees` table results dynamically by `base_salary` descending before returning to the frontend.
*   **HPC Challenge:** Sorting millions of rows on a GPU is notoriously difficult due to sequential dependency. It requires multiple kernel launches (Parallel Radix Sort) and utilizing `__shared__` block memory to calculate prefixes and localized scans before broadcasting globally.

### 2. Multi-Table Hash Join (`JOIN`)
*   **The Query:** Link the `Employees` array to the `Departments` array to visually display the department's name instead of just the foreign key `dept_id`.
*   **HPC Challenge:** Executing a fully parallel GPU Hash Join. The GPU must first construct a lock-free Hash Map in its VRAM using aggressive memory alignment and atomic operations, then concurrently Probe that hash map across 10,000 parallel threads.

### 3. Deep Output Metrics Reduction (`SUM`, `AVG`)
*   **The Query:** Calculate the total payroll `net_paid` across all logs.
*   **HPC Challenge:** Building a Parallel Reduction tree. Instead of one thread looping and summing everything, arrays must be halved repeatedly using thread-local shared memory variables until one global thread produces the total scalar sum, effectively avoiding Memory Bank Conflicts.

### 4. Concurrent Hardware Searching (`LIKE '%str%'`)
*   **The Query:** Provide a text-box on the UI for administrators to perform fuzzy lookups on Employee `name` arrays.
*   **HPC Challenge:** Parallel String matching is highly irregular. Because names vary in byte length, it usually results in "GPU Warp Divergence" (where some threads finish instantly and others are stuck running string checks). This demonstrates the challenge of non-contiguous data alignment on parallel hardware.

---

## Web API Integration Plan

If implemented, these expansions will require the following architecture updates:

1. **Backend Integration:**
    * Add specific PyCUDA `SourceModule` strings containing `extern "C"` C++ kernels for reductions and radix sorting inside `parallel_algorithms.py`.
    * Update `query_optimizer.py` to correctly allocate temporary memory buffers necessary for sorting.
2. **Frontend Additions:**
    * Add new toggle buttons on the `index.html` Sidebar.
    * Expand `app.js` to parse these new complex outputs, specifically updating the chart logic to visualize multi-pass algorithms compared to CPU iteration.
