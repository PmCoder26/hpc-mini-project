#include "SerialAlgorithms.h"
#include <unordered_map>

double SerialAlgorithms::aggregatePayroll(const std::vector<Payroll>& records) {
    double total_net_paid = 0.0;
    for (size_t i = 0; i < records.size(); ++i) {
        total_net_paid += records[i].net_paid;
    }
    return total_net_paid;
}

std::vector<int> SerialAlgorithms::filterEmployees(const std::vector<Employee>& records, int minAge, int minPerf) {
    std::vector<int> matching_ids;
    for (size_t i = 0; i < records.size(); ++i) {
        if (records[i].age >= minAge && records[i].performance_score >= minPerf) {
            matching_ids.push_back(records[i].emp_id);
        }
    }
    return matching_ids;
}

std::vector<JoinResult> SerialAlgorithms::executeHashJoin(const std::vector<Employee>& employees, const std::vector<Department>& departments) {
    std::vector<JoinResult> results;
    std::unordered_map<int, std::string> dept_map;
    
    // Hash Phase
    for (size_t i = 0; i < departments.size(); ++i) {
        dept_map[departments[i].dept_id] = departments[i].dept_name;
    }
    
    // Probe Phase
    for (size_t i = 0; i < employees.size(); ++i) {
        auto it = dept_map.find(employees[i].department_id);
        if (it != dept_map.end()) {
            results.push_back({employees[i].name, it->second, employees[i].base_salary});
        }
    }
    
    return results;
}
