#pragma once
#include <vector>
#include <string>
#include "models.h"

class DataLoader {
public:
    static std::vector<Department> loadDepartments(const std::string& filepath);
    static std::vector<Employee> loadEmployees(const std::string& filepath);
    static std::vector<Attendance> loadAttendance(const std::string& filepath);
    static std::vector<Payroll> loadPayroll(const std::string& filepath);
};
