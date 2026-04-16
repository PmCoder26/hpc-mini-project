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
    // Phase 3 & 4
    static std::vector<int> filterEmployees(const std::vector<Employee>& records, int minAge, int minPerf);
    static double aggregatePayroll(const std::vector<Payroll>& records);
    static std::vector<JoinResult> executeHashJoin(const std::vector<Employee>& emp, const std::vector<Department>& dept);
    
    // Phase 7 - Advanced Algorithms
    static void mergeSortEmployees(std::vector<Employee>& arr, int left, int right);
    static std::vector<GroupByResult> groupSalaryByDepartment(const std::vector<Employee>& e, const std::vector<Department>& d);
    static std::vector<int> findHighAbsenceEmployees(const std::vector<Attendance>& logs, const std::vector<Employee>& emps, int threshold);
    static std::vector<Employee> runSubqueryAnalysis(const std::vector<Employee>& e, const std::vector<Department>& d);
    static std::vector<int> runAntiJoin(const std::vector<Attendance>& logs, const std::vector<Employee>& e);
    static std::vector<Employee> textSearch(const std::vector<Employee>& e, const std::string& pattern);
};
```

### `ParallelAlgorithms`
The HPC Core. These methods mirror the actions of `SerialAlgorithms` but encapsulate OpenMP pragmas (`#pragma omp parallel for`, `reduction`, `task`).
```cpp
class ParallelAlgorithms {
public:
    // Phase 3 & 4
    static std::vector<int> filterEmployees(const std::vector<Employee>& records, int minAge, int minPerf);
    static double aggregatePayroll(const std::vector<Payroll>& records);
    static std::vector<JoinResult> executeHashJoin(const std::vector<Employee>& emp, const std::vector<Department>& dept);
    
    // Phase 7 - Advanced Algorithms
    static void mergeSortEmployees(std::vector<Employee>& arr, int left, int right);
    static std::vector<GroupByResult> groupSalaryByDepartment(const std::vector<Employee>& e, const std::vector<Department>& d);
    static std::vector<int> findHighAbsenceEmployees(const std::vector<Attendance>& logs, const std::vector<Employee>& emps, int threshold);
    static std::vector<Employee> runSubqueryAnalysis(const std::vector<Employee>& e, const std::vector<Department>& d);
    static std::vector<int> runAntiJoin(const std::vector<Attendance>& logs, const std::vector<Employee>& e);
    static std::vector<Employee> textSearch(const std::vector<Employee>& e, const std::string& pattern);
};
```

### `QueryOptimizer`
The orchestrator. It receives a query type, runs both the serial and parallel versions, records their timings using high-precision clocks, and calculates the speedup.
```cpp
struct BenchmarkingStats {
    double serial_time_ms;
    double parallel_time_ms;
    double speedup;
    double efficiency;
    bool results_validated;
};

class QueryOptimizer {
private:
    DatabaseEngine* engine;

public:
    QueryOptimizer(DatabaseEngine* e) : engine(e) {}

    // Phase 5 Methods
    BenchmarkingStats runPayrollAggregation();
    BenchmarkingStats runEmployeeFilter(int minAge, int minPerf);
    BenchmarkingStats runHashJoin();

    // Phase 7 Configured Pipelines
    BenchmarkingStats runMergeSort();
    BenchmarkingStats runDepartmentBurden();
    BenchmarkingStats runAbsenceFilter(int threshold);
    BenchmarkingStats runSubqueryAnalysis();
    BenchmarkingStats runAntiJoin();
    BenchmarkingStats runTextSearch(const std::string& pattern);
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
