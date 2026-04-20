import csv
import random
import os

NUM_DEPTS = 100
NUM_EMPLOYEES = 1_000_000
NUM_PAYROLL = 1_000_000
NUM_ATTENDANCE = 2_000_000

DATA_DIR = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'data')
os.makedirs(DATA_DIR, exist_ok=True)

def generate_departments():
    filepath = os.path.join(DATA_DIR, 'departments.csv')
    print(f"Generating Departments ({NUM_DEPTS} rows)...")
    with open(filepath, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['dept_id', 'dept_name'])
        for i in range(1, NUM_DEPTS + 1):
            writer.writerow([i, f"Department_{i}"])
    print("Departments done.")

def generate_employees():
    filepath = os.path.join(DATA_DIR, 'employees.csv')
    print(f"Generating Employees ({NUM_EMPLOYEES} rows)...")
    with open(filepath, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['emp_id', 'emp_name', 'age', 'dept_id', 'salary'])
        for i in range(1, NUM_EMPLOYEES + 1):
            name = f"Employee_{i}"
            age = random.randint(22, 65)
            dept_id = random.randint(1, NUM_DEPTS)
            salary = random.randint(30000, 200000)
            writer.writerow([i, name, age, dept_id, salary])
    print("Employees done.")

def generate_payroll():
    filepath = os.path.join(DATA_DIR, 'payroll.csv')
    print(f"Generating Payroll ({NUM_PAYROLL} rows)...")
    with open(filepath, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['payroll_id', 'emp_id', 'net_pay', 'tax_deduction'])
        for i in range(1, NUM_PAYROLL + 1):
            emp_id = random.randint(1, NUM_EMPLOYEES)
            gross = random.randint(30000, 200000)
            tax = int(gross * 0.25)
            net_pay = gross - tax
            writer.writerow([i, emp_id, net_pay, tax])
    print("Payroll done.")

def generate_attendance():
    filepath = os.path.join(DATA_DIR, 'attendance.csv')
    print(f"Generating Attendance ({NUM_ATTENDANCE} rows)...")
    statuses = ['Present', 'Absent', 'Leave']
    with open(filepath, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['log_id', 'emp_id', 'date_timestamp', 'status'])
        for i in range(1, NUM_ATTENDANCE + 1):
            emp_id = random.randint(1, NUM_EMPLOYEES)
            timestamp = 1700000000 + random.randint(0, 3000000)
            status = random.choice(statuses)
            writer.writerow([i, emp_id, timestamp, status])
    print("Attendance done.")

if __name__ == "__main__":
    print("Starting generation...")
    generate_departments()
    generate_employees()
    generate_payroll()
    generate_attendance()
    print("All mock data generated successfully!")
