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

// ---------------------------------------------------------
// PHASE 7: ADVANCED QUERIES
// ---------------------------------------------------------

void ParallelAlgorithms::merge(std::vector<Employee>& arr, int left, int mid, int right) {
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

void ParallelAlgorithms::mergeSortEmployees(std::vector<Employee>& arr, int left, int right) {
    if (left < right) {
        if (right - left < 50000) {
            SerialAlgorithms::mergeSortEmployees(arr, left, right);
            return;
        }
        
        int mid = left + (right - left) / 2;
        #pragma omp task shared(arr)
        mergeSortEmployees(arr, left, mid);
        
        #pragma omp task shared(arr)
        mergeSortEmployees(arr, mid + 1, right);
        
        #pragma omp taskwait
        merge(arr, left, mid, right);
    }
}

std::vector<GroupByResult> ParallelAlgorithms::groupSalaryByDepartment(const std::vector<Employee>& employees, const std::vector<Department>& departments) {
    std::unordered_map<int, std::string> dept_map;
    for (const auto& d : departments) dept_map[d.dept_id] = d.dept_name;
    
    int num_threads = omp_get_max_threads();
    std::vector<std::unordered_map<std::string, double>> thread_sums(num_threads);
    
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        #pragma omp for
        for (size_t i = 0; i < employees.size(); ++i) {
            auto it = dept_map.find(employees[i].department_id);
            if (it != dept_map.end()) {
                thread_sums[tid][it->second] += employees[i].base_salary;
            }
        }
    }
    
    std::unordered_map<std::string, double> global_sums;
    for (int t = 0; t < num_threads; ++t) {
        for (const auto& pair : thread_sums[t]) {
            global_sums[pair.first] += pair.second;
        }
    }
    
    std::vector<GroupByResult> results;
    for (const auto& pair : global_sums) results.push_back({pair.first, pair.second});
    return results;
}

std::vector<int> ParallelAlgorithms::findHighAbsenceEmployees(const std::vector<Attendance>& logs, const std::vector<Employee>& emps, int absence_threshold) {
    (void)emps;
    int num_threads = omp_get_max_threads();
    std::vector<std::unordered_map<int, int>> thread_counts(num_threads);
    
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        #pragma omp for
        for (size_t i = 0; i < logs.size(); ++i) {
            if (logs[i].status == "Absent") {
                thread_counts[tid][logs[i].emp_id]++;
            }
        }
    }
    
    std::unordered_map<int, int> global_counts;
    for (int t = 0; t < num_threads; ++t) {
        for (const auto& pair : thread_counts[t]) {
            global_counts[pair.first] += pair.second;
        }
    }
    
    std::vector<int> results;
    for (const auto& pair : global_counts) {
        if (pair.second >= absence_threshold) results.push_back(pair.first);
    }
    return results;
}

std::vector<Employee> ParallelAlgorithms::runSubqueryAnalysis(const std::vector<Employee>& employees, const std::vector<Department>& departments) {
    std::unordered_map<int, std::string> dept_region_map;
    for(const auto& d : departments) dept_region_map[d.dept_id] = d.region;

    double total_perf = 0;
    #pragma omp parallel for reduction(+:total_perf)
    for(size_t i=0; i<employees.size(); ++i) {
        total_perf += employees[i].performance_score;
    }
    
    double avg_perf = employees.empty() ? 0 : total_perf / employees.size();
    std::vector<Employee> global_results;
    
    #pragma omp parallel
    {
        std::vector<Employee> local_results;
        #pragma omp for
        for(size_t i=0; i<employees.size(); ++i) {
            if(employees[i].performance_score < avg_perf) {
                auto it = dept_region_map.find(employees[i].department_id);
                if(it != dept_region_map.end() && it->second == "Europe") {
                    local_results.push_back(employees[i]);
                }
            }
        }
        #pragma omp critical
        {
            global_results.insert(global_results.end(), local_results.begin(), local_results.end());
        }
    }
    return global_results;
}

std::vector<int> ParallelAlgorithms::runAntiJoin(const std::vector<Attendance>& logs, const std::vector<Employee>& employees) {
    std::unordered_map<int, bool> emp_exists;
    for(const auto& e : employees) emp_exists[e.emp_id] = true;
    
    std::vector<int> global_ghosts;
    
    #pragma omp parallel
    {
        std::vector<int> local_ghosts;
        #pragma omp for
        for (size_t i = 0; i < logs.size(); ++i) {
            if(emp_exists.find(logs[i].emp_id) == emp_exists.end()) {
                local_ghosts.push_back(logs[i].log_id);
            }
        }
        #pragma omp critical
        {
            global_ghosts.insert(global_ghosts.end(), local_ghosts.begin(), local_ghosts.end());
        }
    }
    return global_ghosts;
}

std::vector<Employee> ParallelAlgorithms::textSearch(const std::vector<Employee>& employees, const std::string& pattern) {
    std::vector<Employee> global_matches;
    
    #pragma omp parallel
    {
        std::vector<Employee> local_matches;
        #pragma omp for
        for(size_t i = 0; i < employees.size(); ++i) {
            if(employees[i].base_salary > 100000 && employees[i].name.find(pattern) != std::string::npos) {
                local_matches.push_back(employees[i]);
            }
        }
        #pragma omp critical
        {
            global_matches.insert(global_matches.end(), local_matches.begin(), local_matches.end());
        }
    }
    return global_matches;
}
