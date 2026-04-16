import csv
import random
import os

def generate_departments(filename, num_records):
    regions = ['North America', 'Europe', 'Asia', 'South America', 'Africa', 'Oceania']
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['dept_id', 'dept_name', 'region'])
        for i in range(1, num_records + 1):
            writer.writerow([i, f"Dept_{i}", random.choice(regions)])

def generate_employees(filename, num_records, num_depts):
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['emp_id', 'name', 'age', 'department_id', 'base_salary', 'hire_year', 'performance_score'])
        for i in range(1, num_records + 1):
            writer.writerow([
                i,
                f"Employee_Name_{i}",
                random.randint(22, 65),
                random.randint(1, num_depts),
                round(random.uniform(40000.0, 150000.0), 2),
                random.randint(1995, 2023),
                random.randint(1, 100)
            ])

def generate_attendance(filename, num_records, num_emps):
    statuses = ['Present', 'Absent', 'Leave']
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['log_id', 'emp_id', 'date_timestamp', 'hours_worked', 'status'])
        for i in range(1, num_records + 1):
            status = random.choices(statuses, weights=[85, 10, 5])[0]
            hours = round(random.uniform(0.0, 12.0), 1) if status == 'Present' else 0.0
            writer.writerow([
                i,
                random.randint(1, num_emps),
                random.randint(1600000000, 1700000000), 
                hours,
                status
            ])

def generate_payroll(filename, num_records, num_emps):
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['transaction_id', 'emp_id', 'fiscal_month', 'bonus_amount', 'tax_deduction', 'net_paid'])
        for i in range(1, num_records + 1):
            bonus = round(random.uniform(0.0, 5000.0), 2) if random.random() > 0.8 else 0.0
            net = round(random.uniform(3000.0, 12000.0) + bonus, 2)
            tax = round(net * 0.25, 2)
            writer.writerow([
                i,
                random.randint(1, num_emps),
                random.randint(1, 12),
                bonus,
                tax,
                round(net - tax, 2)
            ])

if __name__ == "__main__":
    print("Generating Mock Data...")
    
    base_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(base_dir, '../data')
    os.makedirs(data_dir, exist_ok=True)
    
    # Starts with a smaller initial dataset for quick testing and validation!
    # Once the engine is built and verified, we will crank this up to millions.
    DEPT_COUNT = 500
    EMP_COUNT = 10000000         
    ATTENDANCE_COUNT = 25000000
    PAYROLL_COUNT = 15000000
    
    generate_departments(os.path.join(data_dir, 'departments.csv'), DEPT_COUNT)
    print(f"[{DEPT_COUNT}] Departments Generated")
    
    generate_employees(os.path.join(data_dir, 'employees.csv'), EMP_COUNT, DEPT_COUNT)
    print(f"[{EMP_COUNT}] Employees Generated")
    
    generate_attendance(os.path.join(data_dir, 'attendance.csv'), ATTENDANCE_COUNT, EMP_COUNT)
    print(f"[{ATTENDANCE_COUNT}] Attendance Logs Generated")
    
    generate_payroll(os.path.join(data_dir, 'payroll.csv'), PAYROLL_COUNT, EMP_COUNT)
    print(f"[{PAYROLL_COUNT}] Payroll Transactions Generated")
    
    print("\n✅ All mock datasets successfully generated in the data/ folder.")
