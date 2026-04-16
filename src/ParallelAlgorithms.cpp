#include "ParallelAlgorithms.h"
#include <unordered_map>
#include <omp.h>

double ParallelAlgorithms::aggregatePayroll(const std::vector<Payroll>& records) {
    double total_net_paid = 0.0;
    
    // REDUCTION safely manages concurrent writes to total_net_paid
    #pragma omp parallel for reduction(+:total_net_paid)
    for (size_t i = 0; i < records.size(); ++i) {
        total_net_paid += records[i].net_paid;
    }
    
    return total_net_paid;
}

std::vector<int> ParallelAlgorithms::filterEmployees(const std::vector<Employee>& records, int minAge, int minPerf) {
    std::vector<int> global_matching_ids;
    
    #pragma omp parallel
    {
        // Thread-local storage
        std::vector<int> local_matching_ids;
        
        #pragma omp for
        for (size_t i = 0; i < records.size(); ++i) {
            if (records[i].age >= minAge && records[i].performance_score >= minPerf) {
                local_matching_ids.push_back(records[i].emp_id);
            }
        }
        
        // Safely merge local vector into global vector exactly as modelled in flow.md!
        #pragma omp critical
        {
            global_matching_ids.insert(global_matching_ids.end(), local_matching_ids.begin(), local_matching_ids.end());
        }
    }
    
    return global_matching_ids;
}

std::vector<JoinResult> ParallelAlgorithms::executeHashJoin(const std::vector<Employee>& employees, const std::vector<Department>& departments) {
    std::vector<JoinResult> global_results;
    std::unordered_map<int, std::string> dept_map;
    
    // Hash Phase (Sequential)
    for (size_t i = 0; i < departments.size(); ++i) {
        dept_map[departments[i].dept_id] = departments[i].dept_name;
    }
    
    // Probe Phase (Parallel)
    #pragma omp parallel
    {
        std::vector<JoinResult> local_results;
        
        #pragma omp for
        for (size_t i = 0; i < employees.size(); ++i) {
            // map::find is read-only, avoiding concurrent write faults!
            auto it = dept_map.find(employees[i].department_id);
            if (it != dept_map.end()) {
                local_results.push_back({employees[i].name, it->second, employees[i].base_salary});
            }
        }
        
        #pragma omp critical
        {
            global_results.insert(global_results.end(), local_results.begin(), local_results.end());
        }
    }
    
    return global_results;
}
