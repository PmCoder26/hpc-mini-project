# System Design: GPU Accelerated HR Analytics Dashboard

This document outlines the scope, data models, and execution flow for your High Performance Computing (HPC) mini-project: **Parallelization of Database Query Optimization via an Enterprise HR Analytics System**.

## 1. Project Scope

The project simulates a massive, enterprise-level Human Resources and Payroll Data Warehouse. To demonstrate the necessity and power of GPU parallel computation, we bypass standard SQL databases and build a custom, in-memory query execution engine utilizing Python, NVIDIA PyCUDA, and a Web-based administrative dashboard.

**In-Scope:**
*   **Massive Data Generation:** Scripts generating large synthetic datasets (millions of records).
*   **In-Memory Database Engine:** A Python (Pandas/FastAPI) backend that loads data into RAM to eliminate Disk I/O bottlenecks.
*   **GPU Parallel Query Operators:** Compiling and running C++ CUDA kernels via PyCUDA to perform blazing-fast Database operations across grids and blocks.
*   **Web Dashboard:** An interactive HTML/JS frontend allowing a user to run queries and immediately visualize performance comparisons using Chart.js.
*   **Serial vs. Parallel Benchmarking:** Every operation intercepts the request, runs a single-threaded Python CPU fallback, runs the PyCUDA multi-processor system, and compares the Speedup execution time.

**Out-of-Scope:**
*   Persistent disk-page management (data is strictly loaded into RAM).
*   A full SQL string parser (queries are specific API endpoints triggered by UI interactions).

---

## 2. Expanded Data Model Schema

We use an expanded relational schema reflecting a real corporate data warehouse, processed natively as Columnar data.

### Table 1: `Employees`
*   **`emp_id`** (Integer, Primary Key)
*   **`name`** (String)
*   **`age`** (Integer)
*   **`department_id`** (Integer, Foreign Key)
*   **`salary`** (Float)

### Table 2: `Departments`
*   **`dept_id`** (Integer, Primary Key)
*   **`manager_id`** (Integer)
*   **`budget`** (Float)

### Table 3: `Payroll_Transactions`
*   **`payroll_id`** (Integer, Primary Key)
*   **`emp_id`** (Integer, Foreign Key)
*   **`net_paid`** (Float)
*   **`tax_deduction`** (Float)

### Table 4: `Attendance_Logs`
*   **`log_id`** (Integer, Primary Key)
*   **`emp_id`** (Integer, Foreign Key)
*   **`status`** (Integer: 1 = Present, 0 = Absent)

---

## 3. System Flow Architecture

The operational flow of the system during a demo or benchmark follows these phases:

### Phase 1: Ingestion & Server Booting
1.  **Data Generation:** The system ensures CSV files exist.
2.  **API Loading:** The FastAPI Python program starts, reading the entire CSV corpus into memory (`pandas.DataFrame`) via the `DatabaseEngine`.

### Phase 2: The Dashboard Control Panel
The user interacts with a secure, Dark-Glassmorphism styled HPC Admin Portal.
1. The user navigates to a subsystem (e.g. "Manage Employees").
2. The user inputs a numerical filter (e.g. `Filter Employees by minimum Salary: 50000`).
3. The user clicks "Execute Parallel Query" initiating a REST API POST Request to the FastAPI backend.

### Phase 3: The PyCUDA Accelerator (The HPC Core)
1.  **Hardware Transfer:** The `QueryOptimizer` marshals the column (e.g., `salary`) array to the NVIDIA graphical processing unit.
2.  **Execution Execution:** The system launches the chosen query through the PyCUDA compiled logic (`filter_gt_compact` or `filter_eq_compact`).
3.  **Result Aggregation:** The exact matched records are pulled off the GPU and returned to Python constraints.

### Phase 4: Benchmarking and Performance Comparison
An integral part of this HPC project is demonstrating the efficiency gained through CUDA grids.

1.  **Sequential Execution:** The query is executed first using simple Python iteration. We log the precise clock time.
2.  **Validation & Metrics:** The system calculates Speedup ($S = T_{serial} / T_{parallel}$).
3.  **Visual Output:** The Frontend renders a dynamic Bar Chart plotting the GPU hardware domination over the CPU, and generates a data-table of the exact matched results based on the `limit` query flag.
