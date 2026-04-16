#pragma once
#include <vector>
#include <string>
#include "models.h"

// Output struct for Hash Joins
struct JoinResult {
    std::string emp_name;
    std::string dept_name;
    float base_salary;
};

// For Group By output
struct GroupByResult {
    std::string dept_name;
    double total_salary;
};

class SerialAlgorithms {
public:
    // Basic Phase
    static double aggregatePayroll(const std::vector<Payroll>& records);
    static std::vector<int> filterEmployees(const std::vector<Employee>& records, int minAge, int minPerf);
    static std::vector<JoinResult> executeHashJoin(const std::vector<Employee>& employees, const std::vector<Department>& departments);

    // Advanced Phase
    static void mergeSortEmployees(std::vector<Employee>& arr, int left, int right);
    static std::vector<GroupByResult> groupSalaryByDepartment(const std::vector<Employee>& employees, const std::vector<Department>& departments);
    static std::vector<int> findHighAbsenceEmployees(const std::vector<Attendance>& logs, const std::vector<Employee>& emps, int absence_threshold);
    static std::vector<Employee> runSubqueryAnalysis(const std::vector<Employee>& employees, const std::vector<Department>& departments);
    static std::vector<int> runAntiJoin(const std::vector<Attendance>& logs, const std::vector<Employee>& employees);
    static std::vector<Employee> textSearch(const std::vector<Employee>& employees, const std::string& pattern);

private:
    static void merge(std::vector<Employee>& arr, int left, int mid, int right);
};
