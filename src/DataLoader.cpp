#include "DataLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

// Quick CSV parsing helper
vector<Department> DataLoader::loadDepartments(const string& filepath) {
    vector<Department> data;
    ifstream file(filepath);
    if (!file.is_open()) { cerr << "Failed to open " << filepath << endl; return data; }
    
    string line;
    getline(file, line); // Skip header
    
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Department d;
        getline(ss, token, ','); d.dept_id = stoi(token);
        getline(ss, token, ','); d.dept_name = token;
        getline(ss, token, ','); d.region = token;
        data.push_back(d);
    }
    return data;
}

vector<Employee> DataLoader::loadEmployees(const string& filepath) {
    vector<Employee> data;
    ifstream file(filepath);
    if (!file.is_open()) return data;
    
    string line;
    getline(file, line);
    
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Employee e;
        getline(ss, token, ','); e.emp_id = stoi(token);
        getline(ss, token, ','); e.name = token;
        getline(ss, token, ','); e.age = stoi(token);
        getline(ss, token, ','); e.department_id = stoi(token);
        getline(ss, token, ','); e.base_salary = stof(token);
        getline(ss, token, ','); e.hire_year = stoi(token);
        getline(ss, token, ','); e.performance_score = stoi(token);
        data.push_back(e);
    }
    return data;
}

vector<Attendance> DataLoader::loadAttendance(const string& filepath) {
    vector<Attendance> data;
    ifstream file(filepath);
    if (!file.is_open()) return data;
    
    string line;
    getline(file, line);
    
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Attendance a;
        getline(ss, token, ','); a.log_id = stoi(token);
        getline(ss, token, ','); a.emp_id = stoi(token);
        getline(ss, token, ','); a.date_timestamp = stoi(token);
        getline(ss, token, ','); a.hours_worked = stof(token);
        getline(ss, token, ','); a.status = token;
        data.push_back(a);
    }
    return data;
}

vector<Payroll> DataLoader::loadPayroll(const string& filepath) {
    vector<Payroll> data;
    ifstream file(filepath);
    if (!file.is_open()) return data;
    
    string line;
    getline(file, line);
    
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Payroll p;
        getline(ss, token, ','); p.transaction_id = stoi(token);
        getline(ss, token, ','); p.emp_id = stoi(token);
        getline(ss, token, ','); p.fiscal_month = stoi(token);
        getline(ss, token, ','); p.bonus_amount = stof(token);
        getline(ss, token, ','); p.tax_deduction = stof(token);
        getline(ss, token, ','); p.net_paid = stof(token);
        data.push_back(p);
    }
    return data;
}
