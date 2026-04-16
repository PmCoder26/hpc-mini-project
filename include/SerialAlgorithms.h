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

class SerialAlgorithms {
public:
    static double aggregatePayroll(const std::vector<Payroll>& records);
    static std::vector<int> filterEmployees(const std::vector<Employee>& records, int minAge, int minPerf);
    static std::vector<JoinResult> executeHashJoin(const std::vector<Employee>& employees, const std::vector<Department>& departments);
};
