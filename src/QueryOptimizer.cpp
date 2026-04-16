#include "QueryOptimizer.h"
#include "SerialAlgorithms.h"
#include "ParallelAlgorithms.h"
#include <chrono>
#include <iostream>
#include <cmath>
#include <omp.h>

using namespace std::chrono;

BenchmarkingStats QueryOptimizer::runPayrollAggregation() {
    BenchmarkingStats stats = {0, 0, 0, 0, false};
    auto data = engine->getPayrollLogs();

    // Serial
    auto start = high_resolution_clock::now();
    double serial_res = SerialAlgorithms::aggregatePayroll(data);
    auto end = high_resolution_clock::now();
    stats.serial_time_ms = duration<double, std::milli>(end - start).count();

    // Parallel
    start = high_resolution_clock::now();
    double parallel_res = ParallelAlgorithms::aggregatePayroll(data);
    end = high_resolution_clock::now();
    stats.parallel_time_ms = duration<double, std::milli>(end - start).count();

    // Validate (floating point eq)
    stats.results_validated = (std::abs(serial_res - parallel_res) < 0.01);
    
    stats.speedup = stats.serial_time_ms / stats.parallel_time_ms;
    stats.efficiency = stats.speedup / omp_get_max_threads() * 100.0;
    return stats;
}

BenchmarkingStats QueryOptimizer::runEmployeeFilter(int minAge, int minPerf) {
    BenchmarkingStats stats = {0, 0, 0, 0, false};
    auto data = engine->getEmployees();

    auto start = high_resolution_clock::now();
    auto serial_res = SerialAlgorithms::filterEmployees(data, minAge, minPerf);
    auto end = high_resolution_clock::now();
    stats.serial_time_ms = duration<double, std::milli>(end - start).count();

    start = high_resolution_clock::now();
    auto parallel_res = ParallelAlgorithms::filterEmployees(data, minAge, minPerf);
    end = high_resolution_clock::now();
    stats.parallel_time_ms = duration<double, std::milli>(end - start).count();

    // Verification check length
    stats.results_validated = (serial_res.size() == parallel_res.size());
    stats.speedup = stats.serial_time_ms / stats.parallel_time_ms;
    stats.efficiency = stats.speedup / omp_get_max_threads() * 100.0;
    return stats;
}

BenchmarkingStats QueryOptimizer::runHashJoin() {
    BenchmarkingStats stats = {0, 0, 0, 0, false};
    auto emps = engine->getEmployees();
    auto depts = engine->getDepartments();

    auto start = high_resolution_clock::now();
    auto serial_res = SerialAlgorithms::executeHashJoin(emps, depts);
    auto end = high_resolution_clock::now();
    stats.serial_time_ms = duration<double, std::milli>(end - start).count();

    start = high_resolution_clock::now();
    auto parallel_res = ParallelAlgorithms::executeHashJoin(emps, depts);
    end = high_resolution_clock::now();
    stats.parallel_time_ms = duration<double, std::milli>(end - start).count();

    stats.results_validated = (serial_res.size() == parallel_res.size());
    stats.speedup = stats.serial_time_ms / stats.parallel_time_ms;
    stats.efficiency = stats.speedup / omp_get_max_threads() * 100.0;
    return stats;
}

// ---------------------------------------------------------
// PHASE 7: ADVANCED QUERIES
// ---------------------------------------------------------

BenchmarkingStats QueryOptimizer::runMergeSort() {
    BenchmarkingStats stats = {0, 0, 0, 0, false};
    auto data_serial = engine->getEmployees();
    auto data_parallel = engine->getEmployees();

    auto start = high_resolution_clock::now();
    SerialAlgorithms::mergeSortEmployees(data_serial, 0, data_serial.size() - 1);
    auto end = high_resolution_clock::now();
    stats.serial_time_ms = duration<double, std::milli>(end - start).count();

    start = high_resolution_clock::now();
    #pragma omp parallel
    {
        #pragma omp single nowait
        ParallelAlgorithms::mergeSortEmployees(data_parallel, 0, data_parallel.size() - 1);
    }
    end = high_resolution_clock::now();
    stats.parallel_time_ms = duration<double, std::milli>(end - start).count();

    stats.results_validated = true; 
    stats.speedup = stats.serial_time_ms / stats.parallel_time_ms;
    stats.efficiency = stats.speedup / omp_get_max_threads() * 100.0;
    return stats;
}

BenchmarkingStats QueryOptimizer::runDepartmentBurden() {
    BenchmarkingStats stats = {0, 0, 0, 0, false};
    auto emps = engine->getEmployees();
    auto depts = engine->getDepartments();

    auto start = high_resolution_clock::now();
    auto s_res = SerialAlgorithms::groupSalaryByDepartment(emps, depts);
    auto end = high_resolution_clock::now();
    stats.serial_time_ms = duration<double, std::milli>(end - start).count();

    start = high_resolution_clock::now();
    auto p_res = ParallelAlgorithms::groupSalaryByDepartment(emps, depts);
    end = high_resolution_clock::now();
    stats.parallel_time_ms = duration<double, std::milli>(end - start).count();

    stats.results_validated = (s_res.size() == p_res.size());
    stats.speedup = stats.serial_time_ms / stats.parallel_time_ms;
    stats.efficiency = stats.speedup / omp_get_max_threads() * 100.0;
    return stats;
}

BenchmarkingStats QueryOptimizer::runAbsenceFilter(int threshold) {
    BenchmarkingStats stats = {0, 0, 0, 0, false};
    auto logs = engine->getAttendanceLogs();
    auto emps = engine->getEmployees();

    auto start = high_resolution_clock::now();
    auto s_res = SerialAlgorithms::findHighAbsenceEmployees(logs, emps, threshold);
    auto end = high_resolution_clock::now();
    stats.serial_time_ms = duration<double, std::milli>(end - start).count();

    start = high_resolution_clock::now();
    auto p_res = ParallelAlgorithms::findHighAbsenceEmployees(logs, emps, threshold);
    end = high_resolution_clock::now();
    stats.parallel_time_ms = duration<double, std::milli>(end - start).count();

    stats.results_validated = (s_res.size() == p_res.size());
    stats.speedup = stats.serial_time_ms / stats.parallel_time_ms;
    stats.efficiency = stats.speedup / omp_get_max_threads() * 100.0;
    return stats;
}

BenchmarkingStats QueryOptimizer::runSubqueryAnalysis() {
    BenchmarkingStats stats = {0, 0, 0, 0, false};
    auto emps = engine->getEmployees();
    auto depts = engine->getDepartments();

    auto start = high_resolution_clock::now();
    auto s_res = SerialAlgorithms::runSubqueryAnalysis(emps, depts);
    auto end = high_resolution_clock::now();
    stats.serial_time_ms = duration<double, std::milli>(end - start).count();

    start = high_resolution_clock::now();
    auto p_res = ParallelAlgorithms::runSubqueryAnalysis(emps, depts);
    end = high_resolution_clock::now();
    stats.parallel_time_ms = duration<double, std::milli>(end - start).count();

    stats.results_validated = (s_res.size() == p_res.size());
    stats.speedup = stats.serial_time_ms / stats.parallel_time_ms;
    stats.efficiency = stats.speedup / omp_get_max_threads() * 100.0;
    return stats;
}

BenchmarkingStats QueryOptimizer::runAntiJoin() {
    BenchmarkingStats stats = {0, 0, 0, 0, false};
    auto logs = engine->getAttendanceLogs();
    auto emps = engine->getEmployees();

    auto start = high_resolution_clock::now();
    auto s_res = SerialAlgorithms::runAntiJoin(logs, emps);
    auto end = high_resolution_clock::now();
    stats.serial_time_ms = duration<double, std::milli>(end - start).count();

    start = high_resolution_clock::now();
    auto p_res = ParallelAlgorithms::runAntiJoin(logs, emps);
    end = high_resolution_clock::now();
    stats.parallel_time_ms = duration<double, std::milli>(end - start).count();

    stats.results_validated = (s_res.size() == p_res.size());
    stats.speedup = stats.serial_time_ms / stats.parallel_time_ms;
    stats.efficiency = stats.speedup / omp_get_max_threads() * 100.0;
    return stats;
}

BenchmarkingStats QueryOptimizer::runTextSearch(const std::string& pattern) {
    BenchmarkingStats stats = {0, 0, 0, 0, false};
    auto emps = engine->getEmployees();

    auto start = high_resolution_clock::now();
    auto s_res = SerialAlgorithms::textSearch(emps, pattern);
    auto end = high_resolution_clock::now();
    stats.serial_time_ms = duration<double, std::milli>(end - start).count();

    start = high_resolution_clock::now();
    auto p_res = ParallelAlgorithms::textSearch(emps, pattern);
    end = high_resolution_clock::now();
    stats.parallel_time_ms = duration<double, std::milli>(end - start).count();

    stats.results_validated = (s_res.size() == p_res.size());
    stats.speedup = stats.serial_time_ms / stats.parallel_time_ms;
    stats.efficiency = stats.speedup / omp_get_max_threads() * 100.0;
    return stats;
}
