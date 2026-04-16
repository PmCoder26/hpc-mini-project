#pragma once
#include <string>

struct Employee {
    int emp_id;
    std::string name;
    int age;
    int department_id;
    float base_salary;
    int hire_year;
    int performance_score;
};

struct Department {
    int dept_id;
    std::string dept_name;
    std::string region;
};

struct Attendance {
    int log_id;
    int emp_id;
    int date_timestamp;
    float hours_worked;
    std::string status;
};

struct Payroll {
    int transaction_id;
    int emp_id;
    int fiscal_month;
    float bonus_amount;
    float tax_deduction;
    float net_paid;
};
