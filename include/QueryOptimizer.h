#pragma once
#include "DatabaseEngine.h"

struct BenchmarkingStats {
    double serial_time_ms;
    double parallel_time_ms;
    double speedup;
    double efficiency;
    bool results_validated;
};

class QueryOptimizer {
private:
    DatabaseEngine* engine;

public:
    QueryOptimizer(DatabaseEngine* e) : engine(e) {}

    BenchmarkingStats runPayrollAggregation();
    BenchmarkingStats runEmployeeFilter(int minAge, int minPerf);
    BenchmarkingStats runHashJoin();

    // Phase 7 Advanced Features
    BenchmarkingStats runMergeSort();
    BenchmarkingStats runDepartmentBurden();
    BenchmarkingStats runAbsenceFilter(int threshold);
    BenchmarkingStats runSubqueryAnalysis();
    BenchmarkingStats runAntiJoin();
    BenchmarkingStats runTextSearch(const std::string& pattern);
};
