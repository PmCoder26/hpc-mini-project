#include "DatabaseEngine.h"
#include "QueryOptimizer.h"
#include "CLIController.h"
#include <iostream>
#include <omp.h>

int main() {
    std::cout << "============================================\n";
    std::cout << " Starting HPC OpenMP Database Engine...\n";
    std::cout << " Core Threads Limit: " << omp_get_max_threads() << "\n";
    std::cout << "============================================\n\n";
    
    DatabaseEngine engine;
    engine.initialize("data");
    
    QueryOptimizer optimizer(&engine);
    CLIController cli(&optimizer);
    
    cli.start();
    
    return 0;
}
