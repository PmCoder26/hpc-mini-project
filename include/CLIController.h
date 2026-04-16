#pragma once
#include "QueryOptimizer.h"

class CLIController {
private:
    QueryOptimizer* optimizer;

    void printReport(const std::string& queryName, const BenchmarkingStats& stats);

public:
    CLIController(QueryOptimizer* opt) : optimizer(opt) {}
    void start();
};
