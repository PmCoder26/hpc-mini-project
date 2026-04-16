#pragma once
#include <vector>
#include <string>
#include "models.h"

class DatabaseEngine {
private:
    std::vector<Department> departments;
    std::vector<Employee> employees;
    std::vector<Attendance> attendance_logs;
    std::vector<Payroll> payroll_logs;

public:
    void initialize(const std::string& dataDirectory);

    const std::vector<Department>& getDepartments() const { return departments; }
    const std::vector<Employee>& getEmployees() const { return employees; }
    const std::vector<Attendance>& getAttendanceLogs() const { return attendance_logs; }
    const std::vector<Payroll>& getPayrollLogs() const { return payroll_logs; }
};
