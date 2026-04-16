# Class Architecture & Data Models

This document outlines the complete Object-Oriented design, Data Structures (Entities), and Core Classes including their method signatures for the **OpenMP HPC Analytics Engine**.

## 1. Data Models (Entities / Structs)
We use `struct` for our data models to keep things lightweight, cache-friendly, and perfectly contiguous in memory. These map perfectly to the CSV data.

```cpp
// Represents a single employee record
struct Employee {
    int emp_id;
    std::string name;
    int age;
    int department_id;
    float base_salary;
    int hire_year;
    int performance_score;
};

// Represents a department entity
struct Department {
    int dept_id;
    std::string dept_name;
    std::string region;
};

// Represents a single payroll transaction
struct Payroll {
    int transaction_id;
    int emp_id;
    int fiscal_month;
    float bonus_amount;
    float tax_deduction;
    float net_paid;
};

// Represents an attendance log ping
struct Attendance {
    int log_id;
    int emp_id;
    int date_timestamp;
    float hours_worked;
    std::string status;
};
```

---

## 2. Core Functional Classes

The system logic is divided into modular classes. The central engine holds the state, while separate algorithm classes execute the logic to ensure a clean separation of concerns.

### `DataLoader`
Utility class abstracting the File I/O. Its only job is to parse huge CSV lists and return constructed vectors.
```cpp
class DataLoader {
public:
    static std::vector<Employee> loadEmployees(const std::string& filepath);
    static std::vector<Department> loadDepartments(const std::string& filepath);
    static std::vector<Payroll> loadPayrollLogs(const std::string& filepath);
    static std::vector<Attendance> loadAttendanceLogs(const std::string& filepath);
};
```

### `DatabaseEngine`
The central state hub that holds the loaded arrays in RAM. It acts as the "In-Memory Database" repository.
```cpp
class DatabaseEngine {
private:
    std::vector<Employee> employees;
    std::vector<Department> departments;
    std::vector<Payroll> payroll_logs;
    std::vector<Attendance> attendance_logs;

public:
    // Bootstraps the engine by invoking DataLoader
    void initialize(const std::string& directoryPath);
    
    // Getters for the algorithm classes to read data
    const std::vector<Employee>& getEmployees() const;
    const std::vector<Payroll>& getPayrollLogs() const;
    // ... other getters
};
```

### `SerialAlgorithms`
The baseline, single-threaded implementations of database operations used to measure normal execution time.
```cpp
class SerialAlgorithms {
public:
    // Returns indices of employees matching a filter
    static std::vector<int> filterEmployees(const std::vector<Employee>& records, int minAge, int minPerf);
    
    // Reduces payroll to calculate total financial output
    static double aggregatePayroll(const std::vector<Payroll>& records);
    
    // Standard sorting algorithm
    static void mergeSort(std::vector<Employee>& records, int left, int right);
    
    // Nested-loop or standard hash mapping for Joins
    static void executeHashJoin(const std::vector<Employee>& emp, const std::vector<Department>& dept);
};
```

### `ParallelAlgorithms`
The HPC Core. These methods mirror the actions of `SerialAlgorithms` but encapsulate OpenMP pragmas (`#pragma omp parallel for`, `reduction`, `task`).
```cpp
class ParallelAlgorithms {
public:
    // Utilizes #pragma omp parallel for
    static std::vector<int> filterEmployees(const std::vector<Employee>& records, int minAge, int minPerf);
    
    // Utilizes reduction(+:total)
    static double aggregatePayroll(const std::vector<Payroll>& records);
    
    // Utilizes #pragma omp task for recursive sorting
    static void parallelMergeSort(std::vector<Employee>& records, int left, int right);
    
    // Utilizes parallel array chunking over a Hash Map
    static void executeHashJoin(const std::vector<Employee>& emp, const std::vector<Department>& dept);
};
```

### `QueryOptimizer`
The orchestrator. It receives a query type, runs both the serial and parallel versions, records their timings using high-precision clocks, and calculates the speedup.
```cpp
struct BenchmarkingStats {
    double serial_time_ms;
    double parallel_time_ms;
    double speedup;
    bool results_validated;
};

class QueryOptimizer {
private:
    DatabaseEngine* engine;

public:
    QueryOptimizer(DatabaseEngine* e) : engine(e) {}

    // Executes the requested query (e.g., 1 for Filter, 2 for Join)
    BenchmarkingStats executeQuery(int queryType);

private:
    // Calculates Speedup = SerialTime / ParallelTime
    double calculateSpeedup(double t_serial, double t_parallel);
    
    // Ensures parallel threads didn't cause race conditions
    bool validateResults(const std::vector<int>& res1, const std::vector<int>& res2);
};
```

### `CLIController`
Manages the terminal interface loop, keeping the `main()` function clean.
```cpp
class CLIController {
private:
    QueryOptimizer* optimizer;

public:
    CLIController(QueryOptimizer* opt) : optimizer(opt) {}

    // Main loop mapping user input to execution
    void startInteractiveSession();

private:
    void displayMenu();
    void printMetricsReport(int queryType, const BenchmarkingStats& stats);
};
```
