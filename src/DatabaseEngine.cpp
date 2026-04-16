#include "DatabaseEngine.h"
#include "DataLoader.h"
#include <iostream>

void DatabaseEngine::initialize(const std::string& dataDirectory) {
    std::cout << "[DatabaseEngine] Loading Data into RAM..." << std::endl;
    
    departments = DataLoader::loadDepartments(dataDirectory + "/departments.csv");
    std::cout << "  -> Loaded " << departments.size() << " Departments." << std::endl;
    
    employees = DataLoader::loadEmployees(dataDirectory + "/employees.csv");
    std::cout << "  -> Loaded " << employees.size() << " Employees." << std::endl;
    
    attendance_logs = DataLoader::loadAttendance(dataDirectory + "/attendance.csv");
    std::cout << "  -> Loaded " << attendance_logs.size() << " Attendance logs." << std::endl;
    
    payroll_logs = DataLoader::loadPayroll(dataDirectory + "/payroll.csv");
    std::cout << "  -> Loaded " << payroll_logs.size() << " Payroll logs." << std::endl;
    
    std::cout << "[DatabaseEngine] Initialization complete." << std::endl;
}
