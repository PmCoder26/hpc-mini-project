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
