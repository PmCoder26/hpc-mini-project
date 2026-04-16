#pragma once
#include <vector>
#include "models.h"
#include "SerialAlgorithms.h" 

class ParallelAlgorithms {
public:
    static double aggregatePayroll(const std::vector<Payroll>& records);
    static std::vector<int> filterEmployees(const std::vector<Employee>& records, int minAge, int minPerf);
    static std::vector<JoinResult> executeHashJoin(const std::vector<Employee>& employees, const std::vector<Department>& departments);
};
