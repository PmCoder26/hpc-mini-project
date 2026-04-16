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

// ---------------------------------------------------------
// PHASE 7: ADVANCED QUERIES
// ---------------------------------------------------------

void SerialAlgorithms::merge(std::vector<Employee>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    std::vector<Employee> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].base_salary >= R[j].base_salary) arr[k++] = L[i++]; // Descending
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void SerialAlgorithms::mergeSortEmployees(std::vector<Employee>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortEmployees(arr, left, mid);
        mergeSortEmployees(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

std::vector<GroupByResult> SerialAlgorithms::groupSalaryByDepartment(const std::vector<Employee>& employees, const std::vector<Department>& departments) {
    std::unordered_map<int, std::string> dept_map;
    for (const auto& d : departments) dept_map[d.dept_id] = d.dept_name;
    
    std::unordered_map<std::string, double> sum_map;
    for (const auto& e : employees) {
        sum_map[dept_map[e.department_id]] += e.base_salary;
    }
    
    std::vector<GroupByResult> results;
    for (const auto& pair : sum_map) results.push_back({pair.first, pair.second});
    return results;
}

std::vector<int> SerialAlgorithms::findHighAbsenceEmployees(const std::vector<Attendance>& logs, const std::vector<Employee>& emps, int absence_threshold) {
    (void)emps; // Emps is passed for interface parity
    std::unordered_map<int, int> absence_counts;
    for (const auto& log : logs) {
        if (log.status == "Absent") absence_counts[log.emp_id]++;
    }
    
    std::vector<int> results;
    for (const auto& pair : absence_counts) {
        if (pair.second >= absence_threshold) results.push_back(pair.first);
    }
    return results;
}

std::vector<Employee> SerialAlgorithms::runSubqueryAnalysis(const std::vector<Employee>& employees, const std::vector<Department>& departments) {
    double total_perf = 0;
    for(const auto& e : employees) total_perf += e.performance_score;
    double avg_perf = employees.empty() ? 0 : total_perf / employees.size();
    
    std::unordered_map<int, std::string> dept_region_map;
    for(const auto& d : departments) dept_region_map[d.dept_id] = d.region;
    
    std::vector<Employee> results;
    for(const auto& e : employees) {
        if(e.performance_score < avg_perf && dept_region_map[e.department_id] == "Europe") {
            results.push_back(e);
        }
    }
    return results;
}

std::vector<int> SerialAlgorithms::runAntiJoin(const std::vector<Attendance>& logs, const std::vector<Employee>& employees) {
    std::unordered_map<int, bool> emp_exists;
    for(const auto& e : employees) emp_exists[e.emp_id] = true;
    
    std::vector<int> ghost_logs;
    for(const auto& log : logs) {
        if(emp_exists.find(log.emp_id) == emp_exists.end()) {
            ghost_logs.push_back(log.log_id);
        }
    }
    return ghost_logs;
}

std::vector<Employee> SerialAlgorithms::textSearch(const std::vector<Employee>& employees, const std::string& pattern) {
    std::vector<Employee> matches;
    for(const auto& e : employees) {
        if(e.base_salary > 100000 && e.name.find(pattern) != std::string::npos) {
            matches.push_back(e);
        }
    }
    return matches;
}
