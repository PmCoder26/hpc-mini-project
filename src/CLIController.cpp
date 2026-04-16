#include "CLIController.h"
#include <iostream>
#include <iomanip>
#include <omp.h>

void CLIController::printReport(const std::string& queryName, const BenchmarkingStats& stats) {
    std::cout << "\n============================================\n";
    std::cout << " HPC EXECUTION REPORT: " << queryName << "\n";
    std::cout << "============================================\n";
    std::cout << " Cores Available: " << omp_get_max_threads() << "\n\n";
    std::cout << " [1] Sequential Time: " << std::fixed << std::setprecision(4) << stats.serial_time_ms << " ms\n";
    std::cout << " [2] Parallel Time:   " << stats.parallel_time_ms << " ms\n";
    std::cout << "--------------------------------------------\n";
    std::cout << " -> Speedup Factor:   " << stats.speedup << "x faster!\n";
    std::cout << " -> Hardware Effic.:  " << stats.efficiency << "%\n";
    std::cout << " -> Data Validated:   " << (stats.results_validated ? "PASS (No Race Conditions)" : "FAIL") << "\n";
    std::cout << "============================================\n\n";
}

void CLIController::start() {
    int choice = 0;
    while (true) {
        std::cout << "\n=== Enterprise Analytics Engine (HPC) ===\n";
        std::cout << "1. Run Massive Filter (Age >= 40, Perf >= 80)\n";
        std::cout << "2. Run Deep Aggregation (Total Payroll)\n";
        std::cout << "3. Run Multi-Table Hash Join\n";
        std::cout << "4. Parallel Merge Sort (Order Employees by Salary)\n";
        std::cout << "5. Multi-Table Group By (Sum Salary per Department)\n";
        std::cout << "6. Deep Temporal Filtering (Find Employees Absent >= 3 Times)\n";
        std::cout << "7. Multi-Phase Subquery (Filter Europe Depts vs Avg Perf)\n";
        std::cout << "8. Anomaly Detection Anti-Join (Find Ghost Attendance Logs)\n";
        std::cout << "9. Thread Semantic Text Search (Employees named 'Smith')\n";
        std::cout << "0. Exit\n";
        std::cout << "Select Query: ";
        if (!(std::cin >> choice)) {
            break; // Handle bad input
        }

        if (choice == 1) {
            std::cout << "Running Benchmark (Chunking Array)...\n";
            auto stats = optimizer->runEmployeeFilter(40, 80);
            printReport("Employee Filtering", stats);
        } else if (choice == 2) {
            std::cout << "Running Benchmark (Map-Reduce Reduction)...\n";
            auto stats = optimizer->runPayrollAggregation();
            printReport("Payroll Aggregation", stats);
        } else if (choice == 3) {
            std::cout << "Running Benchmark (Concurrent Hash Read)...\n";
            auto stats = optimizer->runHashJoin();
            printReport("Employees -> Departments Hash Join", stats);
        } else if (choice == 4) {
            std::cout << "Running Benchmark (Recursive Tasking)...\n";
            auto stats = optimizer->runMergeSort();
            printReport("Merge Sort (Salary DESC)", stats);
        } else if (choice == 5) {
            std::cout << "Running Benchmark (Thread-Local Hash Maps)...\n";
            auto stats = optimizer->runDepartmentBurden();
            printReport("Department Salary Map-Reduce", stats);
        } else if (choice == 6) {
            std::cout << "Running Benchmark (Temporal High-Absence Histogram)...\n";
            auto stats = optimizer->runAbsenceFilter(3);
            printReport("Absence Thresholding", stats);
        } else if (choice == 7) {
            std::cout << "Running Benchmark (Memory Barrier Dependencies)...\n";
            auto stats = optimizer->runSubqueryAnalysis();
            printReport("Nested Europe Dept Avg Filter", stats);
        } else if (choice == 8) {
            std::cout << "Running Benchmark (Set-Difference Misses)...\n";
            auto stats = optimizer->runAntiJoin();
            printReport("Ghost Logs Anti-Join", stats);
        } else if (choice == 9) {
            std::cout << "Running Benchmark (Deep Text Search)...\n";
            auto stats = optimizer->runTextSearch("Smith");
            printReport("Text Search (O(N*M))", stats);
        } else {
            std::cout << "Exiting...\n";
            break;
        }
    }
}
